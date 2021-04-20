/*
이름 : 이연우
학과 : 휴먼지능정보공학과
학번 : 201810793
*/
/*
[과제] Gonzalez 자동 이진화 임계치 결정 프로그램 구현

Gonzalez 알고리즘 내 종료조건에 해당하는 입실론 값은 3이하로 하세요.

경계값이 104가 나옴
*/
#pragma warning(disable:4996) // 입출력 에러 막기
#include <stdio.h>
#include <stdlib.h> // 동적할당
#include <Windows.h> // 비트맵 헤더 관련 구조체(BITMAPFILEHEADER, BITMAPINFOHEADER, RGBQUAD)

// 히스토그램 만들기 (막대그래프, 화소가 각 몇개인지)
void ObtainHistogram(BYTE *Img, int *Histo, int W, int H) {
	int ImgSize = W * H;

	for (int i = 0; i < ImgSize; i++) {
		Histo[Img[i]]++;
	}
}

// 이진화
void Binarization(BYTE *Img, BYTE *Out,int W, int H, BYTE Threshold) {
	int ImgSize = W * H;

	for (int i = 0; i < ImgSize; i++) {
		if (Img[i] < Threshold) { // 임계치보다 작으면 0
			Out[i] = 0;
		}
		else { // 크면 255
			Out[i] = 255;
		}
	}
}

int GozalezBinThresh(BYTE *Img, int W, int H, int T) {
	int ImgSize = W * H;
	int low = 0, high = 0;
	int lowS = 0, highS = 0;

	for (int i = 0; i < ImgSize; i++) {
		if (Img[i] < T) { // T보다 어두운 값
			lowS += Img[i];
			low++;
		}
		else { // T보다 밝은 값
			highS += Img[i];
			high++;
		}
	}
	int Thres = ((lowS / low) + (highS / high)) / 2;


	if (abs(T - Thres) >= 3) {
		return GozalezBinThresh(Img, W, H, Thres);
	}
	else
		return Thres;
}

int main() {
	// -----------------------------------
	// 영상 입력
	BITMAPFILEHEADER hf; // 14 bytes
	BITMAPINFOHEADER hinfo; // 40 bytes
	RGBQUAD hRGB[256]; // 4 * 256 = 1024 bytes

	FILE *fp; // 파일 오픈을 위한 파일 포인터 변수
	fp = fopen("lenna.bmp", "rb"); // 파일 오픈, 읽기 위해 rb(read binary) , 이미지 파일은 전부 binary
	if (fp == NULL) { // 파일 포인터가 파일을 가르키지 않고있다면
		printf("File not found!/n");
		return -1;
	}
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp); // fread로 파일 읽기
	fread(&hinfo, sizeof(BITMAPINFOHEADER), 1, fp); // fread(저장할 변수, 크기, 횟수, 현재 파일 포인터)
	fread(hRGB, sizeof(RGBQUAD), 256, fp); // 배열이름 자체가 주소라 &사용 x
	int ImgSize = hinfo.biWidth * hinfo.biHeight; // 영상의 화소(인포헤더에서 가로, 세로 정보 가져오기)
	BYTE *Image = (BYTE *)malloc(ImgSize); // 동적할당, BYTE(0 ~ 255)
	// = BYTE Image[ImgSize], 원본영상 담기
	BYTE *Output = (BYTE *)malloc(ImgSize); // 영상 처리 결과 담기
	fread(Image, sizeof(BYTE), ImgSize, fp); // 영상의 화수 정보 담기
	fclose(fp); //파일 포인터와 영성파일 간의 연결 끊기


	//---------------------------------------
	// 영상 처리

	int Histo[256] = { 0 }; // 히스토그램
	int min, max;

	ObtainHistogram(Image, Histo, hinfo.biWidth, hinfo.biHeight); // 히스토그램
	for (int i = 0; i < 256; i++) {	// 밝기 최소값	
		if (Histo[i] > 0) {
			min = i;
			break;
		}
	}
	for (int i = 255; i >= 0; i--) { // 밝기 최대값	
		if (Histo[i] > 0) {
			max = i;
			break;
		}
	}

	int T = (min + max) / 2; // 경계값 T 초기값

	int Thres = GozalezBinThresh(Image, hinfo.biWidth, hinfo.biHeight, T);
	Binarization(Image, Output, hinfo.biWidth, hinfo.biHeight, Thres); // 이진화

	//---------------------------------------
	// 처리 결과 출력
	fp = fopen("output.bmp", "wb"); //bmp파일 생성, wb = write binary
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp); // fwrite(저장할 변수, 크기, 횟수, 현재 파일 포인터)
	fwrite(&hinfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp); // 기록을 할때는 1byte 단위로 해야함
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(Output, sizeof(BYTE), ImgSize, fp); // 영상 처리 결과 출력
	fclose(fp); // 파일포인터와 영상파일 관계 끊기

	free(Image); // 동적할당 해제 
	free(Output); // 하지않으면 memory leak 현상발생

	return 0;
}
