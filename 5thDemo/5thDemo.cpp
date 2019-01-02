// 5thDemo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>

#include "./gdal/gdal_priv.h"
#pragma comment(lib, "gdal_i.lib")

using namespace std;

int main() {
	cout << "program begins...." << endl;
	char* mulSrc = "American_mul.bmp";
	char* panSrc = "American_pan.bmp";
	char* poDst = "American_fus.tif";
	GDALDataset* mulSrcData;
	GDALDataset* panSrcData;
	GDALDataset* poDstData;
	int imgXlen, imgYlen, bandNum;
	float* bandOri[3];
	float* bandTrans[3];
	float* bandAns[3];
	float* bandI;
	float transMat[3][3] = { 1 / 3, 1 / 3, 1 / 3,-sqrt(2) / 6, -sqrt(2) / 6, sqrt(2) / 3,1 / sqrt(2), -1 / sqrt(2), 0 };
	float conTransMat[3][3] = { 1,  -1 / sqrt(2),  1 / sqrt(2), 1,  -1 / sqrt(2),  -1 / sqrt(2),	1,  sqrt(2),     0 };
	GDALAllRegister();
	mulSrcData = (GDALDataset*)GDALOpenShared(mulSrc, GA_ReadOnly);
	panSrcData = (GDALDataset*)GDALOpenShared(panSrc, GA_ReadOnly);
	imgXlen = mulSrcData->GetRasterXSize();
	imgYlen = mulSrcData->GetRasterYSize();
	bandNum = mulSrcData->GetRasterCount();
	poDstData = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(poDst, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);
	for (int i = 0; i < 3; i++) {
		bandOri[i] = (float*)CPLMalloc(imgXlen*imgYlen * sizeof(float));
		bandTrans[i] = (float*)CPLMalloc(imgXlen*imgYlen * sizeof(float));
		bandAns[i] = (float*)CPLMalloc(imgXlen*imgYlen * sizeof(float));
	}
	bandI = (float*)CPLMalloc(imgXlen*imgYlen * sizeof(float));
	for (int i = 0; i < 3; i++) {
		mulSrcData->GetRasterBand(i + 1)->RasterIO(GF_Read, 0, 0, imgXlen, imgYlen, bandOri[i], imgXlen, imgYlen, GDT_Float32, 0, 0);
	}
	/*I分量保存*/
	panSrcData->GetRasterBand(1)->RasterIO(GF_Read, 0, 0, imgXlen, imgYlen, bandI, imgXlen, imgYlen, GDT_Float32, 0, 0);
	/*正变换*/
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < imgXlen*imgYlen; j++) {
			int sum = 0;
			for (int k = 0; k < 3; k++) {
				sum += bandOri[k][j] * transMat[i][k];
			}
			bandTrans[i][j] = sum;
		}
	}
	/*I分量替换*/
	for (int i = 0; i < imgXlen*imgYlen; i++) {
		bandTrans[0][i] = bandI[i];
	}
	/*逆变换*/
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < imgXlen*imgYlen; j++) {
			int sum = 0;
			for (int k = 0; k < 3; k++) {
				sum += bandTrans[k][j] * conTransMat[i][k];
			}
			bandAns[i][j] = sum;
		}
	}
	for (int i = 0; i < 3; i++) {
		poDstData->GetRasterBand(i + 1)->RasterIO(GF_Write, 0, 0, imgXlen, imgYlen, bandAns[i], imgXlen, imgYlen, GDT_Float32, 0, 0);
	}
	for (int i = 0; i < 3; i++) {
		CPLFree(bandOri[i]);
		CPLFree(bandTrans[i]);
	}
	CPLFree(bandI);
	GDALClose(mulSrcData);
	GDALClose(panSrcData);
	GDALClose(poDstData);

	cout << "program ends...." << endl;
	system("pause");
	return 0;
}