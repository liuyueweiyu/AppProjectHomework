// 6thDemo.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include <iostream>
#include<time.h>
using namespace std;
#include ".\gdal\gdal_priv.h"
#pragma comment(lib, "gdal_i.lib")

void module5() {
	char* mulSrc = "Mul_large.tif";
	char* panSrc = "Pan_large.tif";
	char* poDst = "fus_large2.tif";
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
	int tol = 0;
	mulSrcData = (GDALDataset*)GDALOpenShared(mulSrc, GA_ReadOnly);
	panSrcData = (GDALDataset*)GDALOpenShared(panSrc, GA_ReadOnly);
	imgXlen = mulSrcData->GetRasterXSize();
	imgYlen = mulSrcData->GetRasterYSize();
	bandNum = mulSrcData->GetRasterCount();
	int nBands = mulSrcData->GetRasterCount();
	int nXSize = mulSrcData->GetRasterXSize();
	int nYSize = mulSrcData->GetRasterYSize();
	poDstData = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(poDst, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);
	int nBlockSize = 256; //分块大小 
						  //分配输入分块缓存 
	unsigned char *pSrcData = new unsigned char[nBlockSize*nBlockSize*nBands]; //分配输出分块缓存 
	unsigned char *pDstData = new unsigned char[nBlockSize*nBlockSize]; //定义读取输入图像波段顺序 
	int *pBandMaps = new int[nBands];
	for (int b = 0; b < nBands; b++) pBandMaps[b] = b + 1; //循环分块并进行处理 
	for (int i = 0; i < nYSize; i += nBlockSize) {
		for (int j = 0; j < nXSize; j += nXSize) { //定义两个变量来保存分块大小
			int nXBK = nXSize;
			int nYBK = nBlockSize; //如果最下面和最右边的块不够256，剩下多少读取多少 
			if (i + nBlockSize > nYSize) //最下面的剩余块 
				nYBK = nYSize - i;
			if (j + nXSize > nXSize) //最右侧的剩余块 
				nXBK = nXSize - j;
			for (int a = 0; a < 3; a++) {
				bandOri[a] = (float*)CPLMalloc(nXBK*nYBK * sizeof(float));
				bandTrans[a] = (float*)CPLMalloc(nXBK*nYBK * sizeof(float));
				bandAns[a] = (float*)CPLMalloc(nXBK*nYBK * sizeof(float));
			}
			bandI = (float*)CPLMalloc(nXBK*nYBK * sizeof(float));
			for (int a = 0; a < 3; a++) {
				mulSrcData->GetRasterBand(a + 1)->RasterIO(GF_Read, j, i, nXBK, nYBK, bandOri[a], nXBK, nYBK, GDT_Float32, 0, 0);
			}
			panSrcData->GetRasterBand(1)->RasterIO(GF_Read, j, i, nXBK, nYBK, bandI, nXBK, nYBK, GDT_Float32, 0, 0);
			/*正变换*/
			for (int a = 0; a < 3; a++) {
				for (int b = 0; b < nXBK*nYBK; b++) {
					int sum = 0;
					for (int k = 0; k < 3; k++) {
						sum += bandOri[k][b] * transMat[a][k];
					}
					bandTrans[a][b] = sum;
				}
			}
			/*I分量替换*/
			for (int a = 0; a < nXBK*nYBK; a++) {
				bandTrans[0][a] = bandI[a];
			}
			/*逆变换*/
			for (int a = 0; a < 3; a++) {
				for (int b = 0; b < nXBK*nYBK; b++) {
					int sum = 0;
					for (int k = 0; k < 3; k++) {
						sum += bandTrans[k][b] * conTransMat[a][k];
					}
					bandAns[a][b] = sum;
				}
			}
			for (int a = 0; a < 3; a++) {
				poDstData->GetRasterBand(a + 1)->RasterIO(GF_Write, j, i, nXBK, nYBK, bandAns[a], nXBK, nYBK, GDT_Float32, 0, 0);
			}
			for (int a = 0; a < 3; a++) {
				CPLFree(bandOri[a]);
				CPLFree(bandTrans[a]);
				CPLFree(bandAns[a]);
			}
			tol++;
			cout << "This is the " << tol << "picee" << endl;
			CPLFree(bandI);
			//读取原始图像块 
			/*mulSrcData->GetRasterBand(1)->RasterIO(GF_Read, j, i, nXBK, nYBK, pSrcData, nXBK, nYBK, GDT_Byte, 0, 0); //再这里填写你自己的处理算法 																									//pSrcData 就是读取到的分块数据，存储顺序为，先行后列，最后波段 //pDstData 就是处理后的二值图数据，存储顺序为先行后列
			memcpy(pDstData, pSrcData, sizeof(unsigned char)*nXBK*nYBK); //上面这句是一个测试，将原始图像的第一个波段数据复制到输出的图像里面 //写到结果图像
			poDstData->GetRasterBand(1)->RasterIO(GF_Write, j, i, nXBK, nYBK, pDstData, nXBK, nYBK, GDT_Byte, 0, 0);
			mulSrcData->GetRasterBand(2)->RasterIO(GF_Read, j, i, nXBK, nYBK, pSrcData, nXBK, nYBK, GDT_Byte, 0, 0); //再这里填写你自己的处理算法 																									//pSrcData 就是读取到的分块数据，存储顺序为，先行后列，最后波段 //pDstData 就是处理后的二值图数据，存储顺序为先行后列
			memcpy(pDstData, pSrcData, sizeof(unsigned char)*nXBK*nYBK); //上面这句是一个测试，将原始图像的第一个波段数据复制到输出的图像里面 //写到结果图像
			poDstData->GetRasterBand(2)->RasterIO(GF_Write, j, i, nXBK, nYBK, pDstData, nXBK, nYBK, GDT_Byte, 0, 0);
			mulSrcData->GetRasterBand(3)->RasterIO(GF_Read, j, i, nXBK, nYBK, pSrcData, nXBK, nYBK, GDT_Byte, 0, 0); //再这里填写你自己的处理算法 																									//pSrcData 就是读取到的分块数据，存储顺序为，先行后列，最后波段 //pDstData 就是处理后的二值图数据，存储顺序为先行后列
			memcpy(pDstData, pSrcData, sizeof(unsigned char)*nXBK*nYBK); //上面这句是一个测试，将原始图像的第一个波段数据复制到输出的图像里面 //写到结果图像
			poDstData->GetRasterBand(3)->RasterIO(GF_Write, j, i, nXBK, nYBK, pDstData, nXBK, nYBK, GDT_Byte, 0, 0);*/
		}
	}

	/*I分量保存*/
	//panSrcData->GetRasterBand(1)->RasterIO(GF_Read, 0, 0, imgXlen, imgYlen, bandI, imgXlen, imgYlen, GDT_Float32, 0, 0);

	GDALClose(mulSrcData);
	GDALClose(panSrcData);
	GDALClose(poDstData);
}

int main() {
	clock_t startTime, endTime;
	startTime = clock();
	module5();
	endTime = clock();
	cout << "Totle Time : " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
	return 0;
}
