#include "stdafx.h"
#include <iostream>
using namespace std;
#include "./gdal/gdal_priv.h"
#pragma comment(lib,"gdal_i.lib")

const int N = 300;

GDALDataset* poSrcpotato;
GDALDataset* poDstDS;
GByte* buffTmpPotato[6];
char* potato = "potato.jpg";
char* dstPath[7] = {"dist1.tif", "dist2.tif", "dist3.tif", "dist4.tif", "dist5.tif", "dist6.tif"};
double ma[N][N];
double mar[10][10][10];
int len[6];
/// 方向向量
int xx[30] = {-2, -2, -2, -2, -2, -1, -1, 0, -1, -1, -1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 2, 2, 2, 2, 2};
int yy[30] = {-2, -1, 0, 1, 2, -2, 2, -2, -1, 0, 1, -1, 0, 1, -1, 0, 1, 2, -2, 2, -2, -1, 0, 1, 2};
int imgXlen;
int imgYlen;
int bandNum;
int li1, li2, li3;

void Ini()
{
	len[0] = len[2] = len[3] = len[4] = 3;
	len[1] = 5;
	len[5] = 5;
	mar[0][0][0] = 0;   mar[0][0][1] = 0.2; mar[0][0][2] = 0;
	mar[0][1][0] = 0.2; mar[0][1][1] = 0.2; mar[0][1][2] = 0.2;
	mar[0][2][0] = 0;   mar[0][2][1] = 0.2; mar[0][2][2] = 0;
	mar[1][0][0] = 0.2; mar[1][0][1] = 0;   mar[1][0][2] = 0;   mar[1][0][3] = 0;   mar[1][0][4] = 0;
	mar[1][1][0] = 0;   mar[1][1][1] = 0.2; mar[1][1][2] = 0;   mar[1][1][3] = 0;   mar[1][1][4] = 0;
	mar[1][2][0] = 0;   mar[1][2][1] = 0;   mar[1][2][2] = 0.2; mar[1][2][3] = 0;   mar[1][2][4] = 0;
	mar[1][3][0] = 0;   mar[1][3][1] = 0;   mar[1][3][2] = 0;   mar[1][3][3] = 0.2; mar[1][3][4] = 0;
	mar[1][4][0] = 0;   mar[1][4][1] = 0;   mar[1][4][2] = 0;   mar[1][4][3] = 0;   mar[1][4][4] = 0.2;
	mar[2][0][0] = -1;  mar[2][0][1] = -1;  mar[2][0][2] = -1;
	mar[2][1][0] = -1;  mar[2][1][1] = 8;   mar[2][1][2] = -1;
	mar[2][2][0] = -1;  mar[2][2][1] = -1;  mar[2][2][2] = -1;
    mar[3][0][0] = -1;  mar[3][0][1] = -1;  mar[3][0][2] = -1;
	mar[3][1][0] = -1;  mar[3][1][1] = 9;   mar[3][1][2] = -1;
	mar[3][2][0] = -1;  mar[3][2][1] = -1;  mar[3][2][2] = -1;
	mar[4][0][0] = -1;  mar[4][0][1] = -1;  mar[4][0][2] = 0;
	mar[4][1][0] = -1;  mar[4][1][1] = 0;   mar[4][1][2] = 1;
	mar[4][2][0] = 0;   mar[4][2][1] = 1;   mar[4][2][2] = 1;
	mar[5][0][0] = 0.0120 / 25; mar[5][0][1] = 0.1253 / 25; mar[5][0][2] = 0.2736 / 25; mar[5][0][3] = 0.1253 / 25; mar[5][0][4] = 0.0120 / 25;
	mar[5][1][0] = 0.1253 / 25; mar[5][1][1] = 1.3054 / 25; mar[5][1][2] = 2.8514 / 25; mar[5][1][3] = 1.3054 / 25; mar[5][1][4] = 0.1253 / 25;
	mar[5][2][0] = 0.2376 / 25; mar[5][2][1] = 2.8514 / 25; mar[5][2][2] = 6.2279 / 25; mar[5][2][3] = 2.8514 / 25; mar[5][2][4] = 0.2376 / 25;
	mar[5][3][0] = 0.1253 / 25; mar[5][3][1] = 1.3054 / 25; mar[5][3][2] = 2.8514 / 25; mar[5][3][3] = 1.3054 / 25; mar[5][3][4] = 0.1253 / 25;
	mar[5][4][0] = 0.0120 / 25; mar[5][4][1] = 0.1253 / 25; mar[5][4][2] = 0.2736 / 25; mar[5][4][3] = 0.1253 / 25; mar[5][4][4] = 0.0120 / 25;
}

int main()
{
	Ini();
	GDALAllRegister();
	poSrcpotato = (GDALDataset*)GDALOpenShared(potato, GA_ReadOnly);
	imgXlen = poSrcpotato -> GetRasterXSize();
	imgYlen = poSrcpotato -> GetRasterYSize();
	bandNum = poSrcpotato -> GetRasterCount();
	for(int i = 0; i < 6; i ++){
		buffTmpPotato[i] = (GByte*)CPLMalloc(imgXlen*imgYlen * sizeof(GByte));
		for(int j = 1; j <= 3; j ++){
			poSrcpotato -> GetRasterBand(j) -> RasterIO(GF_Read, 0, 0, imgXlen, imgYlen, buffTmpPotato[i], imgXlen, imgYlen, GDT_Byte, 0, 0);
		}
	}
	for(int i = 0; i < imgXlen; i ++){
		for(int j = 0; j < imgXlen; j ++){
			// printf("%f\n", (double)buffTmpPotato[j * imgXlen + i]);
			ma[i][j] = (double)buffTmpPotato[0][j * imgXlen + i];
		}
	}
	for(int ncase = 0; ncase < 6; ncase ++){
		poDstDS = GetGDALDriverManager() -> GetDriverByName("GTiff") -> Create(dstPath[ncase], imgXlen, imgYlen, bandNum, GDT_Byte, NULL);
		for(int i = 1; i <= bandNum; i ++){
			// poDstDS -> GetRasterBand(i) -> RasterIO(GF_Write, 0, 0, imgXlen, imgYlen, buffTmpPotato[ncase], imgXlen, imgYlen, GDT_Byte, 0, 0);
		}
		li1 = imgXlen - len[ncase] / 2;
		li2 = imgYlen - len[ncase] / 2;
		li3 = (len[ncase] * len[ncase] - 1) / 2;
		// printf("%d %d %d\n", li1, li2, li3);
		for(int i = len[ncase] / 2; i < li1; i ++){
			for(int j = len[ncase] / 2; j < li2; j ++){
				double t;

				t = 0;
				for(int k = -li3; k <= li3; k ++){
					// printf("%d %d\n", xx[12 + k], yy[12 + k]);
					// int x; scanf("%d", &x);
					t += (double)ma[i + xx[12 + k]][j + yy[12 + k]] * mar[ncase][len[ncase] / 2 + xx[12 + k]][len[ncase] / 2 + yy[12 + k]];
				}
				if(t > 255){
					buffTmpPotato[ncase][j * imgXlen + i] = (GByte)255;
				}
				else if(t < 0){
					buffTmpPotato[ncase][j * imgXlen + i] = (GByte)0;
				}
				else{
					buffTmpPotato[ncase][j * imgXlen + i] = (GByte)t;
				}
			}
		}
		for (int i = 1; i <= bandNum; i ++) {
			poDstDS -> GetRasterBand(i) -> RasterIO(GF_Write, 0, 0, imgXlen, imgYlen, buffTmpPotato[ncase], imgXlen, imgYlen, GDT_Byte, 0, 0);
		}
		CPLFree(buffTmpPotato[ncase]);
		GDALClose(poDstDS);
	}
	system("pause");

	return 0;
}
