/*
이름 : 이연우
학과 : 휴먼지능정보공학과
학번 : 201810793
*/
/*
[과제] 세선화 후 분기점 및 끝점 검출 및 표시하기
최근 소스코드와 함께 제공된 dilation.bmp파일에 대해
세선화까지 완료 후,
분기점과 끝점을 검출하여 표시해주는 함수 구현하기.
이 함수를 main함수에서 호출해서 실제 결과이미지 출력까지 수행하는 프로그램 작성.

함수원형
void FeatureExtractThinImage(BYTE * Image, BYTE * Output, int W, int H);
parameter 설명
- Image: 세선화된 이진 영상
- Output: 분기점 및 끝점이 표시된 영상
*/

#pragma warning(disable:4996) // 입출력 에러 막기
#include <stdio.h>
#include <stdlib.h> // 동적할당
#include <Windows.h> // 비트맵 헤더 관련 구조체(BITMAPFILEHEADER, BITMAPINFOHEADER, RGBQUAD)
#include <math.h> // sin, cos

// 이미지 역상(반전) (이미지, 결과, 가로, 세로)
void InverseImage(BYTE *Img, BYTE *Out, int W, int H) {
	int ImgSize = W * H;

	for (int i = 0; i < ImgSize; i++) {
		Out[i] = 255 - Img[i];
	}
}

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

// week 12--------------------(10주차때 배운 내용)
// 침식
void Erosion(BYTE *Image, BYTE *Output, int W, int H) {
	for (int i = 1; i < H - 1; i++) {
		for (int j = 1; j < W - 1; j++) {
			if (Image[i * W + j] == 255) { // 전경화소라면
				if (!(Image[(i - 1) * W + j] == 255 && Image[(i + 1) * W + j] == 255 &&
					Image[i * W + j - 1] == 255 && Image[i * W + j + 1] == 255)) { //4주변화소가 모두 전경 화소가 아니라면
					Output[i * W + j] = 0;
				}
				else Output[i * W + j] = 255;
			}
			else Output[i * W + j] = 0;
		}
	}
}

// 팽창
void Dliation(BYTE *Image, BYTE *Output, int W, int H) {
	for (int i = 1; i < H - 1; i++) {
		for (int j = 1; j < W - 1; j++) {
			if (Image[i * W + j] == 0) { // 배경화소라면
				if (!(Image[(i - 1) * W + j] == 0 && Image[(i + 1) * W + j] == 0 &&
					Image[i * W + j - 1] == 0 && Image[i * W + j + 1] == 0)) { //4주변화소가 모두 배경화소가 아니라면
					Output[i * W + j] = 255;
				}
				else Output[i * W + j] = 0;
			}
			else Output[i * W + j] = 255;
		}
	}
}

// 2차원 배열 동적할당 위함
unsigned char** imageMatrix;

// 이진영상에서 
unsigned char blankPixel = 255, imagePixel = 0;

typedef struct {
	int row, col;
}pixel;

int getBlackNeighbours(int row, int col) {

	int i, j, sum = 0;

	for (i = -1; i <= 1; i++) {
		for (j = -1; j <= 1; j++) {
			if (i != 0 || j != 0)
				sum += (imageMatrix[row + i][col + j] == imagePixel);
		}
	}

	return sum;
}

int getBWTransitions(int row, int col) {
	return 	((imageMatrix[row - 1][col] == blankPixel && imageMatrix[row - 1][col + 1] == imagePixel)
		+ (imageMatrix[row - 1][col + 1] == blankPixel && imageMatrix[row][col + 1] == imagePixel)
		+ (imageMatrix[row][col + 1] == blankPixel && imageMatrix[row + 1][col + 1] == imagePixel)
		+ (imageMatrix[row + 1][col + 1] == blankPixel && imageMatrix[row + 1][col] == imagePixel)
		+ (imageMatrix[row + 1][col] == blankPixel && imageMatrix[row + 1][col - 1] == imagePixel)
		+ (imageMatrix[row + 1][col - 1] == blankPixel && imageMatrix[row][col - 1] == imagePixel)
		+ (imageMatrix[row][col - 1] == blankPixel && imageMatrix[row - 1][col - 1] == imagePixel)
		+ (imageMatrix[row - 1][col - 1] == blankPixel && imageMatrix[row - 1][col] == imagePixel));
}

int zhangSuenTest1(int row, int col) {
	int neighbours = getBlackNeighbours(row, col);

	return ((neighbours >= 2 && neighbours <= 6)
		&& (getBWTransitions(row, col) == 1)
		&& (imageMatrix[row - 1][col] == blankPixel || imageMatrix[row][col + 1] == blankPixel || imageMatrix[row + 1][col] == blankPixel)
		&& (imageMatrix[row][col + 1] == blankPixel || imageMatrix[row + 1][col] == blankPixel || imageMatrix[row][col - 1] == blankPixel));
}

int zhangSuenTest2(int row, int col) {
	int neighbours = getBlackNeighbours(row, col);

	return ((neighbours >= 2 && neighbours <= 6)
		&& (getBWTransitions(row, col) == 1)
		&& (imageMatrix[row - 1][col] == blankPixel || imageMatrix[row][col + 1] == blankPixel || imageMatrix[row][col - 1] == blankPixel)
		&& (imageMatrix[row - 1][col] == blankPixel || imageMatrix[row + 1][col] == blankPixel || imageMatrix[row][col + 1] == blankPixel));
}

// Tining 알고리즘
void zhangSuen(unsigned char* image, unsigned char* output, int rows, int cols) {

	int startRow = 1, startCol = 1, endRow, endCol, i, j, count, processed;

	pixel* markers;

	imageMatrix = (unsigned char**)malloc(rows * sizeof(unsigned char*));

	for (i = 0; i < rows; i++) {
		imageMatrix[i] = (unsigned char*)malloc((cols + 1) * sizeof(unsigned char));
		for (int k = 0; k < cols; k++) imageMatrix[i][k] = image[i * cols + k];
	}

	endRow = rows - 2;
	endCol = cols - 2;
	do {
		markers = (pixel*)malloc((endRow - startRow + 1) * (endCol - startCol + 1) * sizeof(pixel));
		count = 0;

		for (i = startRow; i <= endRow; i++) {
			for (j = startCol; j <= endCol; j++) {
				if (imageMatrix[i][j] == imagePixel && zhangSuenTest1(i, j) == 1) {
					markers[count].row = i;
					markers[count].col = j;
					count++;
				}
			}
		}

		processed = (count > 0);

		for (i = 0; i < count; i++) {
			imageMatrix[markers[i].row][markers[i].col] = blankPixel;
		}

		free(markers);
		markers = (pixel*)malloc((endRow - startRow + 1) * (endCol - startCol + 1) * sizeof(pixel));
		count = 0;

		for (i = startRow; i <= endRow; i++) {
			for (j = startCol; j <= endCol; j++) {
				if (imageMatrix[i][j] == imagePixel && zhangSuenTest2(i, j) == 1) {
					markers[count].row = i;
					markers[count].col = j;
					count++;
				}
			}
		}

		if (processed == 0)
			processed = (count > 0);

		for (i = 0; i < count; i++) {
			imageMatrix[markers[i].row][markers[i].col] = blankPixel;
		}

		free(markers);
	} while (processed == 1);


	for (i = 0; i < rows; i++) {
		for (j = 0; j < cols; j++) {
			output[i * cols + j] = imageMatrix[i][j];
		}
	}
}

// week12 HW----------------
void FeatureExtractThinImage(BYTE * Image, BYTE * Output, int W, int H) {
	int Length = 3;
	int Margin = Length / 2;
	for (int i = 1; i < H - 1; i++) {
		for (int j = 1; j < W - 1; j++) {
			int cnt = 0;
			if(Image[i * W + j] == 255){
				for (int m = -Margin; m <= Margin; m++) {
					for (int n = -Margin; n <= Margin; n++) {
						if (Image[(i + m) * W + (j + n)] == 255)
							cnt++;
					}
				}
				cnt--;
			}

			// 분기점과 끝점 표시
			if (cnt > 2 || cnt == 1) {
				for (int m = -Margin; m <= Margin; m++) {
					for (int n = -Margin; n <= Margin; n++) {
						Output[(i + m) * W + (j + n)] = 130;
						if (m == 0 && n == 0)
							Output[(i + m) * W + (j + n)] = 0;
					}
				}
			}
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
	fp = fopen("dilation.bmp", "rb"); // 파일 오픈, 읽기 위해 rb(read binary) , 이미지 파일은 전부 binary
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

	// week12 HW-----------
	Dliation(Image, Output, W, H);
	Dliation(Output, Image, W, H);
	Dliation(Image, Output, W, H);
	Erosion(Output, Image, W, H);
	Erosion(Image, Output, W, H);
	Erosion(Output, Image, W, H);
	InverseImage(Image, Image, W, H);
	zhangSuen(Image, Output, H, W); // 지문인식에 많이 사용 
	InverseImage(Output, Image, W, H);
	FeatureExtractThinImage(Image, Output, H, W);

	//---------------------------------------
	// 처리 결과 출력
	SaveBMPFile(hf, hinfo, hRGB, Output, hinfo.biWidth, hinfo.biHeight, "output.bmp");

	free(Image); // 동적할당 해제 
	free(Output); // 하지않으면 memory leak 현상발생
	free(Temp);

	return 0;
}