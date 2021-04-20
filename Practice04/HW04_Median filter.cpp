/*
이름 : 이연우
학과 : 휴먼지능정보공학과
학번 : 201810793
*/
/*
[과제] Median filter 사이즈에 따른 결과비교
강의를 통해 3*3 사이즈 미디언필터링을 배워보았습니다.

먼저, 위의 코드의 의미를 이해하고, 행별로 의미하는 바를 주석(//)표시로 설명하시오.

5*5, 7*7, 9*9사이즈 미디언필터링한 수행 결과를
median_5.bmp, median_7.bmp, median_9.bmp 파일로 제출하세요.
*/

#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

// 처리결과 출력 함수
void SaveBMPFile(BITMAPFILEHEADER hf, BITMAPINFOHEADER hInfo,
	RGBQUAD* hRGB, BYTE* Output, int W, int H, const char* FileName)
{
	FILE * fp = fopen(FileName, "wb");
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(Output, sizeof(BYTE), W*H, fp);
	fclose(fp);
}

// a, b 서로 바꾸기
void swap(BYTE* a, BYTE* b)
{
	BYTE temp = *a;
	*a = *b;
	*b = temp;
}

BYTE Median(BYTE* arr, int size)
{
	// bubble sorting 사용
	// 오름차순 정렬
	const int S = size;
	for (int i = 0; i < size - 1; i++) // pivot index
	{
		for (int j = i + 1; j < size; j++) // 비교대상 index
		{
			if (arr[i] > arr[j]) 	swap(&arr[i], &arr[j]);  // 위치 바꾸기
		}
	}
	return arr[S / 2]; // 중간값 return
}


int main()
{
	BITMAPFILEHEADER hf; // 14바이트
	BITMAPINFOHEADER hInfo; // 40바이트
	RGBQUAD hRGB[256]; // 1024바이트
	FILE* fp;
	fp = fopen("lenna_impulse.bmp", "rb");
	if (fp == NULL) {
		printf("File not found!\n");
		return -1;
	}
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp);
	fread(hRGB, sizeof(RGBQUAD), 256, fp);
	int ImgSize = hInfo.biWidth * hInfo.biHeight;
	BYTE * Image = (BYTE *)malloc(ImgSize);
	BYTE * Temp = (BYTE*)malloc(ImgSize); // 임시배열
	BYTE* Output = (BYTE*)malloc(ImgSize);
	fread(Image, sizeof(BYTE), ImgSize, fp);
	fclose(fp);

	/* Median filtering */

	int Length = 3;  // 마스크의 한 변의 길이
	int Margin = Length / 2; // 원본 영상 밖은 처리하지 않기 위해 length의 중앙값을 margin으로 설정
	int WSize = Length * Length; // median filtering을 할 윈도우 크기
	BYTE* temp = (BYTE*)malloc(sizeof(BYTE) * WSize); // median  filtering을  위한 임시 변수
	int W = hInfo.biWidth, H = hInfo.biHeight; // 원본 이미지의 가로(W), 세로(H) 사이즈

	int i, j, m, n; // for문에 사용할 변수 선언
	for (i = Margin; i < H - Margin; i++) { // 원본영상의 열(세로방향이동)
		for (j = Margin; j < W - Margin; j++) { // 원본영상의 행(가로방향이동)
			//Length X Length(윈도우) 화소값을 temp배열에 넣기
			for (m = -Margin; m <= Margin; m++) { // 윈도우의 열(세로방향이동)
				for (n = -Margin; n <= Margin; n++) { // 윈도우의 행(가로방향이동)
					temp[(m + Margin) * Length + (n + Margin)] = Image[(i + m)*W + j + n]; // 윈도우 위치마다 화소값을 temp에 입력 
				}
			}
			Output[i * W + j] = Median(temp, WSize); // Median() = temp배열을 정렬 후 중간 값을 return하여 i행j열 위치의 Output에 넣는다
		}
	}
	free(temp);

	/* Median filtering */

	SaveBMPFile(hf, hInfo, hRGB, Output, hInfo.biWidth, hInfo.biHeight, "median_3.bmp");

	free(Image);
	free(Output);
	free(Temp);
	return 0;
}