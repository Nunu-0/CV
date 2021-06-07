/*
이름 : 이연우
학과 : 휴먼지능정보공학과
학번 : 201810793
*/
/*
[과제] 얼굴영역 추출 후 외접 사각형 그리기
첨부한 BMP 파일에서 피부 영역을 검출하고, 피부 영역에 외접하는 바운딩 박스를 빨간색으로 표시하는 프로그램 작성.

참고사항: 개인적으로 파악하고 있는 Cb, Cr 도메인에서의 피부색 range는

95 < Cb < 125
155 < Cr < 185
입니다. 참고하시고, 잘 안맞을 경우 경험적으로 조정해서 쓰세요.

참고논문: HSI, RGB, YCbCr등 다양한 컬러모델에서의 피부색 range에 대한 논문입니다.
*/

#pragma warning(disable:4996) // 입출력 에러 막기
#include <stdio.h>
#include <stdlib.h> // 동적할당
#include <Windows.h> // 비트맵 헤더 관련 구조체(BITMAPFILEHEADER, BITMAPINFOHEADER, RGBQUAD)

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

// 외접사각형 가장 겉 픽셀찾기
void Obtain2DBoundingBox(BYTE *Img, int W, int H, int *LU_X, int *LU_Y, int *RD_X, int *RD_Y) {
	//LU_X, LU_Y, RD_X, RD_Y
	int flag = 0;

	for (int i = 0; i < H; i++) { // y1(LU_Y) 좌측상단부터 우방향 가로우선 탐색
		for (int j = 0; j < W; j++) {
			if (Img[i * W * 3 + j * 3] != 0) {
				*LU_Y = i - 1;
				flag = 1;
				break;
			}
		}
		if (flag == 1) break;
	}
	flag = 0;
	for (int i = H - 1; i >= 0; i--) { // y2(RD_Y) 우측하단부터 좌방향 가로우선 탐색
		for (int j = W - 1; j >= 0; j--) {
			if (Img[i * W * 3 + j * 3] != 0) {
				*RD_Y = i + 1;
				flag = 1;
				break;
			}
		}
		if (flag == 1) break;
	}
	flag = 0;
	for (int j = 0; j < W; j++) { // x1(LU_X) 좌측상단부터 하방향 세로우선 탐색
		for (int i = 0; i < H; i++) {
			if (Img[i * W * 3 + j * 3] != 0) {
				*LU_X = j - 1;
				flag = 1;
				break;
			}
		}
		if (flag == 1) break;
	}
	flag = 0;
	for (int j = W - 1; j >= 0; j--) { // x2(RD_X) 우측하단부터 상방향 세로우선 탐색
		for (int i = H - 1; i >= 0; i--) {
			if (Img[i * W * 3 + j * 3] != 0) {
				*RD_X = j + 1;
				flag = 1;
				break;
			}
		}
		if (flag == 1) break;
	}
}

// 외접사각형 외곽라인 그리기
void DrawRectOutline(BYTE * Img, BYTE *Out, int W, int H, int LU_X, int LU_Y, int RD_X, int RD_Y) {
	// LU_X: 사각형의 좌측상단 X좌표, LU_Y: 사각형의 좌측상단 Y좌표,
	// RD_X: 사각형의 우측하단 X좌표, LU_Y: 사각형의 우측하단 Y좌표.
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			Out[i * W * 3 + j * 3] = Img[i * W * 3 + j * 3]; // 한 픽셀에 rgb 3픽셀을 사용해 *3 Blue 성분
			Out[i * W * 3 + j * 3 + 1] = Img[i * W * 3 + j * 3 + 1]; // Green 성분
			Out[i * W * 3 + j * 3 + 2] = Img[i * W * 3 + j * 3 + 2]; // Red 성분
		}
	}

	for (int i = LU_X; i <= RD_X; i++) { // 윗면
		Out[(LU_Y)* W * 3 + i * 3] = 0; // 한 픽셀에 rgb 3픽셀을 사용해 *3 Blue 성분
		Out[(LU_Y)* W * 3 + i * 3 + 1] = 0; // Green 성분
		Out[(LU_Y)* W * 3 + i * 3 + 2] = 255; // Red 성분
	}
	for (int i = LU_Y; i <= RD_Y; i++) { // 왼쪽면
		Out[i * W * 3 + LU_X * 3] = 0; // 한 픽셀에 rgb 3픽셀을 사용해 *3 Blue 성분
		Out[i * W * 3 + LU_X * 3 + 1] = 0; // Green 성분
		Out[i * W * 3 + LU_X * 3 + 2] = 255; // Red 성분
	}
	for (int i = RD_Y; i >= LU_Y; i--) { // 오른쪽면
		Out[i * W * 3 + RD_X * 3] = 0; // 한 픽셀에 rgb 3픽셀을 사용해 *3 Blue 성분
		Out[i * W * 3 + RD_X * 3 + 1] = 0; // Green 성분
		Out[i * W * 3 + RD_X * 3 + 2] = 255; // Red 성분
	}
	for (int i = RD_X; i >= LU_X; i--) { // 아랫면
		Out[(RD_Y)* W * 3 + i * 3] = 0; // 한 픽셀에 rgb 3픽셀을 사용해 *3 Blue 성분
		Out[(RD_Y)* W * 3 + i * 3 + 1] = 0; // Green 성분
		Out[(RD_Y)* W * 3 + i * 3 + 2] = 255; // Red 성분
	}
}

void RGB2YCbCr(BYTE *Image, BYTE *Y, BYTE *Cb, BYTE *Cr, int W, int H) {
	// Red 값이 큰 화소만 masking (R, G, B, 모델 기준)
	for (int i = 0; i < H; i++) { // Y좌표
		for (int j = 0; j < W; j++) { // X좌표
			Y[i * W + j] = (BYTE)(0.299 * Image[i * W * 3 + j * 3 + 2] + 0.587 * Image[i * W * 3 + j * 3 + 1] + 0.114 * Image[i * W * 3 + j * 3]);
			Cb[i * W + j] = (BYTE)(-0.16874 * Image[i * W * 3 + j * 3 + 2] - 0.3313 * Image[i * W * 3 + j * 3 + 1] + 0.5 * Image[i * W * 3 + j * 3] + 128.0);
			Cr[i * W + j] = (BYTE)(0.5 * Image[i * W * 3 + j * 3 + 2] - 0.4187 * Image[i * W * 3 + j * 3 + 1] - 0.0813 * Image[i * W * 3 + j * 3] + 128.0);
		}
	}
}

int main() {
	// -----------------------------------
	// 영상 입력
	BITMAPFILEHEADER hf; // 14 bytes
	BITMAPINFOHEADER hinfo; // 40 bytes
	RGBQUAD hRGB[256]; // 4 * 256 = 1024 bytes

	FILE *fp; // 파일 오픈을 위한 파일 포인터 변수
	fp = fopen("face.bmp", "rb"); // 파일 오픈, 읽기 위해 rb(read binary) , 이미지 파일은 전부 binary
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

	BYTE*Y = (BYTE*)malloc(ImgSize);
	BYTE*Cb = (BYTE*)malloc(ImgSize);
	BYTE*Cr = (BYTE*)malloc(ImgSize);

	RGB2YCbCr(Image, Y, Cb, Cr, W, H);
	// 얼굴 영역 만 masking(Y, Cb, Cr, 모델 기준)
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			if (Cb[i * W + j] > 95 && Cb[i * W + j] < 125 && Cr[i * W + j] > 90 && Y[i * W + j] > 90) {
				Temp[i * W * 3 + j * 3] = Image[i * W * 3 + j * 3];
				Temp[i * W * 3 + j * 3 + 1] = Image[i * W * 3 + j * 3 + 1];
				Temp[i * W * 3 + j * 3 + 2] = Image[i * W * 3 + j * 3 + 2];
			}
			else
				Temp[i * W * 3 + j * 3] = Output[i * W * 3 + j * 3 + 1] = Output[i * W * 3 + j * 3 + 2] = 0;
		}
	}

	int Cx, Cy;
	int LU_X, LU_Y, RD_X, RD_Y;

	Obtain2DBoundingBox(Temp, W, H, &LU_X, &LU_Y, &RD_X, &RD_Y); // 외접사각형 꼭지점 좌표 찾기
	DrawRectOutline(Image, Output, W, H, LU_X, LU_Y, RD_X, RD_Y); // 외접사각형

	//---------------------------------------
	// 처리 결과 출력
	SaveBMPFile(hf, hinfo, hRGB, Output, hinfo.biWidth, hinfo.biHeight, "output.bmp");

	free(Image); // 동적할당 해제 
	free(Output); // 하지않으면 memory leak 현상발생
	free(Temp);
	free(Y);
	free(Cb);
	free(Cr);

	return 0;
}