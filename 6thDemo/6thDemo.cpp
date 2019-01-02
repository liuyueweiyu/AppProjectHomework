// 6thDemo.cpp : �������̨Ӧ�ó������ڵ㡣
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
	int nBlockSize = 256; //�ֿ��С 
						  //��������ֿ黺�� 
	unsigned char *pSrcData = new unsigned char[nBlockSize*nBlockSize*nBands]; //��������ֿ黺�� 
	unsigned char *pDstData = new unsigned char[nBlockSize*nBlockSize]; //�����ȡ����ͼ�񲨶�˳�� 
	int *pBandMaps = new int[nBands];
	for (int b = 0; b < nBands; b++) pBandMaps[b] = b + 1; //ѭ���ֿ鲢���д��� 
	for (int i = 0; i < nYSize; i += nBlockSize) {
		for (int j = 0; j < nXSize; j += nXSize) { //������������������ֿ��С
			int nXBK = nXSize;
			int nYBK = nBlockSize; //�������������ұߵĿ鲻��256��ʣ�¶��ٶ�ȡ���� 
			if (i + nBlockSize > nYSize) //�������ʣ��� 
				nYBK = nYSize - i;
			if (j + nXSize > nXSize) //���Ҳ��ʣ��� 
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
			/*���任*/
			for (int a = 0; a < 3; a++) {
				for (int b = 0; b < nXBK*nYBK; b++) {
					int sum = 0;
					for (int k = 0; k < 3; k++) {
						sum += bandOri[k][b] * transMat[a][k];
					}
					bandTrans[a][b] = sum;
				}
			}
			/*I�����滻*/
			for (int a = 0; a < nXBK*nYBK; a++) {
				bandTrans[0][a] = bandI[a];
			}
			/*��任*/
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
			//��ȡԭʼͼ��� 
			/*mulSrcData->GetRasterBand(1)->RasterIO(GF_Read, j, i, nXBK, nYBK, pSrcData, nXBK, nYBK, GDT_Byte, 0, 0); //��������д���Լ��Ĵ����㷨 																									//pSrcData ���Ƕ�ȡ���ķֿ����ݣ��洢˳��Ϊ�����к��У���󲨶� //pDstData ���Ǵ����Ķ�ֵͼ���ݣ��洢˳��Ϊ���к���
			memcpy(pDstData, pSrcData, sizeof(unsigned char)*nXBK*nYBK); //���������һ�����ԣ���ԭʼͼ��ĵ�һ���������ݸ��Ƶ������ͼ������ //д�����ͼ��
			poDstData->GetRasterBand(1)->RasterIO(GF_Write, j, i, nXBK, nYBK, pDstData, nXBK, nYBK, GDT_Byte, 0, 0);
			mulSrcData->GetRasterBand(2)->RasterIO(GF_Read, j, i, nXBK, nYBK, pSrcData, nXBK, nYBK, GDT_Byte, 0, 0); //��������д���Լ��Ĵ����㷨 																									//pSrcData ���Ƕ�ȡ���ķֿ����ݣ��洢˳��Ϊ�����к��У���󲨶� //pDstData ���Ǵ����Ķ�ֵͼ���ݣ��洢˳��Ϊ���к���
			memcpy(pDstData, pSrcData, sizeof(unsigned char)*nXBK*nYBK); //���������һ�����ԣ���ԭʼͼ��ĵ�һ���������ݸ��Ƶ������ͼ������ //д�����ͼ��
			poDstData->GetRasterBand(2)->RasterIO(GF_Write, j, i, nXBK, nYBK, pDstData, nXBK, nYBK, GDT_Byte, 0, 0);
			mulSrcData->GetRasterBand(3)->RasterIO(GF_Read, j, i, nXBK, nYBK, pSrcData, nXBK, nYBK, GDT_Byte, 0, 0); //��������д���Լ��Ĵ����㷨 																									//pSrcData ���Ƕ�ȡ���ķֿ����ݣ��洢˳��Ϊ�����к��У���󲨶� //pDstData ���Ǵ����Ķ�ֵͼ���ݣ��洢˳��Ϊ���к���
			memcpy(pDstData, pSrcData, sizeof(unsigned char)*nXBK*nYBK); //���������һ�����ԣ���ԭʼͼ��ĵ�һ���������ݸ��Ƶ������ͼ������ //д�����ͼ��
			poDstData->GetRasterBand(3)->RasterIO(GF_Write, j, i, nXBK, nYBK, pDstData, nXBK, nYBK, GDT_Byte, 0, 0);*/
		}
	}

	/*I��������*/
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
