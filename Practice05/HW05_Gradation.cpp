/*
이름 : 이연우
학과 : 휴먼지능정보공학과
학번 : 201810793
*/
/*
[과제] 3개 그라데이션 출력 프로그램
위와 같이 출력하는 프로그램 작성 후 cpp 소스파일만 제출
Red ~~~~Cyan
Green ~~~~~Magenta
Blue ~~~~Yellow
*/

#pragma warning(disable:4996) // 입출력 에러 막기
#include <stdio.h>
#include <stdlib.h> // 동적할당
#include <Windows.h> // 비트맵 헤더 관련 구조체(BITMAPFILEHEADER, BITMAPINFOHEADER, RGBQUAD)
#include <math.h> // sin, cos

// 처리결과 출력하기
void SaveBMPFile(BITMAPFILEHEADER hf, BITMAPINFOHEADER hinfo, RGBQUAD *hRGB, BYTE *Output, int W, int H, const char *FileName) {
	FILE *fp = fopen(FileName, "wb"); //bmp파일 생성, wb = write binary
	if (hinfo.biBitCount == 24) {
		fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp); // fwrite(저장할 변수, 크기, 횟수, 현재 파일 포인터)
		fwrite(&hinfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp); // 기록을 할때는 1byte 단위로 해야함
		fwrite(Output, sizeof(BYTE), W * H * 3, fp); // 영상 처리 결과 출력
	}
	else if (hinfo.biBitCount == 8) {
		fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp); // fwrite(저장할 변수, 크기, 횟수, 현재 파일 포인터)
		fwrite(&hinfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp); // 기록을 할때는 1byte 단위로 해야함
		fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
		fwrite(Output, sizeof(BYTE), W * H, fp); // 영상 처리 결과 출력	
	}
	fclose(fp); // 파일포인터와 영상파일 관계 끊기
}

int main() {
	// -----------------------------------
	// 영상 입력
	BITMAPFILEHEADER hf; // 14 bytes
	BITMAPINFOHEADER hinfo; // 40 bytes
	RGBQUAD hRGB[256]; // 4 * 256 = 1024 bytes

	FILE *fp; // 파일 오픈을 위한 파일 포인터 변수
	fp = fopen("TcSample.bmp", "rb"); // 파일 오픈, 읽기 위해 rb(read binary) , 이미지 파일은 전부 binary
	if (fp == NULL) { // 파일 포인터가 파일을 가르키지 않고있다면
		printf("File not found!/n");
		return -1;
	}
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp); // fread로 파일 읽기
	fread(&hinfo, sizeof(BITMAPINFOHEADER), 1, fp); // fread(저장할 변수, 크기, 횟수, 현재 파일 포인터)
	int ImgSize = hinfo.biWidth * hinfo.biHeight; // 영상의 화소(인포헤더에서 가로, 세로 정보 가져오기)
	int H = hinfo.biHeight, W = hinfo.biWidth;
	BYTE *Image;
	BYTE *Output;
	BYTE *Temp;
	if (hinfo.biBitCount == 24) { // 트루 컬러
		Image = (BYTE *)malloc(ImgSize * 3); // 동적할당, BYTE(0 ~ 255)
		Temp = (BYTE *)malloc(ImgSize * 3); // 임시
		Output = (BYTE *)malloc(ImgSize * 3); // 영상 처리 결과 담기
		fread(Image, sizeof(BYTE), ImgSize * 3, fp); // 영상의 화수 정보 담기
	}
	else { // (hinfo.biBitCount == 8) 인덱스(그레이)
		fread(hRGB, sizeof(RGBQUAD), 256, fp); // 배열이름 자체가 주소라 &사용 x
		Image = (BYTE *)malloc(ImgSize); // 동적할당, BYTE(0 ~ 255)
		Temp = (BYTE *)malloc(ImgSize); // 임시
		Output = (BYTE *)malloc(ImgSize); // 영상 처리 결과 담기
		fread(Image, sizeof(BYTE), ImgSize, fp); // 영상의 화수 정보 담기
	}

	fclose(fp); //파일 포인터와 영성파일 간의 연결 끊기


	//---------------------------------------
	// 영상 처리

	// week10 HW ----------

	// 초기화
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			Image[i * W * 3 + j * 3] = 0;
			Image[i * W * 3 + j * 3 + 1] = 0;
			Image[i * W * 3 + j * 3 + 2] = 0;
		}
	}

	//그라데이션 만들기
	double wt; // 가중치
	// y좌표 기준 0 ~ 239 (위) Red -> Cyan
	for (int i = H / 3 * 2; i < H; i++) {
		for (int j = 0; j < W; j++) {
			wt = j / (double)(W - 1);
			Image[i * W * 3 + j * 3] = (BYTE)255 * wt; // Blue
			Image[i * W * 3 + j * 3 + 1] = (BYTE)255 * wt; // Green
			Image[i * W * 3 + j * 3 + 2] = (BYTE)255 * (1.0 - wt); // Red
		}
	}
	// y좌표 기준 120 ~ 359 Green -> Magenta
	for (int i = H / 3; i < H / 3 * 2; i++) {
		for (int j = 0; j < W; j++) {
			wt = j / (double)(W - 1);
			Image[i * W * 3 + j * 3] = (BYTE)255 * wt; // Blue
			Image[i * W * 3 + j * 3 + 1] = (BYTE)255 * (1.0 - wt); // Green
			Image[i * W * 3 + j * 3 + 2] = (BYTE)255 * wt; // Red
		}
	}
	// y좌표 기준 340 ~ 479 Blue -> Yellow
	for (int i = 0; i < H / 3; i++) {
		for (int j = 0; j < W; j++) {
			wt = j / (double)(W - 1);
			Image[i * W * 3 + j * 3] = (BYTE)255 * (1.0 - wt); // Blue
			Image[i * W * 3 + j * 3 + 1] = (BYTE)255 * wt; // Green
			Image[i * W * 3 + j * 3 + 2] = (BYTE)255 * wt; // Red
		}
	}
	//---------------------------------------
	// 처리 결과 출력
	SaveBMPFile(hf, hinfo, hRGB, Image, hinfo.biWidth, hinfo.biHeight, "Gradation.bmp");

	free(Image); // 동적할당 해제 
	free(Output); // 하지않으면 memory leak 현상발생
	free(Temp);

	return 0;
}