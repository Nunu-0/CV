/*
이름 : 이연우
학과 : 휴먼지능정보공학과
학번 : 201810793
*/
/*
[과제] 동공영역 외접사각형 및 무게중심통과하는 크로스라인 그리기

pupil1,bmp, pupil2.bmp 파일에서 동공 영역을 검출한 후,

1) 동공 영역에 외접하는 직사각형을 그리고(라인 두께 1픽셀, 라인 밝기값 255)

2) 동공 영역의 무게중심을 통과하는 수평선/수직선을 그리는(라인 두께 1픽셀, 라인 밝기값 255)

프로그램을 구현하시오.
*/

#pragma warning(disable:4996) // 입출력 에러 막기
#include <stdio.h>
#include <stdlib.h> // 동적할당
#include <Windows.h> // 비트맵 헤더 관련 구조체(BITMAPFILEHEADER, BITMAPINFOHEADER, RGBQUAD)

// 이미지 역상(반전) (이미지, 결과, 가로, 세로)
void InverseImage(BYTE *Img, BYTE *Out, int W, int H) {
	int ImgSize = W * H;

	for (int i = 0; i < ImgSize; i++) {
		Out[i] = 255 - Img[i];
	}
}

// 이진화
void Binarization(BYTE *Img, BYTE *Out, int W, int H, BYTE Threshold) {
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) {
		if (Img[i] < Threshold) // 임계치보다 작으면 0
			Out[i] = 0;
		else // 크면 255
			Out[i] = 255;
	}
}

// 처리결과 출력하기
void SaveBMPFile(BITMAPFILEHEADER hf, BITMAPINFOHEADER hinfo, RGBQUAD *hRGB, BYTE *Output, int W, int H, const char *FileName) {
	FILE *fp = fopen(FileName, "wb"); //bmp파일 생성, wb = write binary
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp); // fwrite(저장할 변수, 크기, 횟수, 현재 파일 포인터)
	fwrite(&hinfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp); // 기록을 할때는 1byte 단위로 해야함
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(Output, sizeof(BYTE), W * H, fp); // 영상 처리 결과 출력
	fclose(fp); // 파일포인터와 영상파일 관계 끊기

}

int push(short* stackx, short* stacky, int arr_size, short vx, short vy, int* top)
{
	if (*top >= arr_size) return(-1);
	(*top)++;
	stackx[*top] = vx;
	stacky[*top] = vy;
	return(1);
}

int pop(short* stackx, short* stacky, short* vx, short* vy, int* top)
{
	if (*top == 0) return(-1);
	*vx = stackx[*top];
	*vy = stacky[*top];
	(*top)--;
	return(1);
}

// GlassFire 알고리즘을 이용한 라벨링 함수
void m_BlobColoring(BYTE* CutImage, int height, int width)
{
	int i, j, m, n, top, area, Out_Area, index, BlobArea[1000];
	long k;
	short curColor = 0, r, c;
	//	BYTE** CutImage2;
	Out_Area = 1;


	// 스택으로 사용할 메모리 할당
	short* stackx = new short[height * width];
	short* stacky = new short[height * width];
	short* coloring = new short[height * width];

	int arr_size = height * width;

	// 라벨링된 픽셀을 저장하기 위해 메모리 할당

	for (k = 0; k < height * width; k++) coloring[k] = 0;  // 메모리 초기화

	for (i = 0; i < height; i++)
	{
		index = i * width;
		for (j = 0; j < width; j++)
		{
			// 이미 방문한 점이거나 픽셀값이 255가 아니라면 처리 안함
			if (coloring[index + j] != 0 || CutImage[index + j] != 255) continue;
			r = i; c = j; top = 0; area = 1;
			curColor++;

			while (1)
			{
			GRASSFIRE:
				for (m = r - 1; m <= r + 1; m++)
				{
					index = m * width;
					for (n = c - 1; n <= c + 1; n++)
					{
						//관심 픽셀이 영상경계를 벗어나면 처리 안함
						if (m < 0 || m >= height || n < 0 || n >= width) continue;

						if ((int)CutImage[index + n] == 255 && coloring[index + n] == 0)
						{
							coloring[index + n] = curColor; // 현재 라벨로 마크
							if (push(stackx, stacky, arr_size, (short)m, (short)n, &top) == -1) continue;
							r = m; c = n; area++;
							goto GRASSFIRE;
						}
					}
				}
				if (pop(stackx, stacky, &r, &c, &top) == -1) break;
			}
			if (curColor < 1000) BlobArea[curColor] = area;
		}
	}

	float grayGap = 255.0f / (float)curColor; // 각각의 라벨 색

	// 가장 면적이 넓은 영역을 찾아내기 위함 
	for (i = 1; i <= curColor; i++)
	{
		if (BlobArea[i] >= BlobArea[Out_Area]) Out_Area = i;
	}
	// CutImage 배열 클리어~
	for (k = 0; k < width * height; k++) CutImage[k] = 255;

	// coloring에 저장된 라벨링 결과중 (Out_Area에 저장된) 영역이 가장 큰 것만 CutImage에 저장
	for (k = 0; k < width * height; k++)
	{
		if (coloring[k] == Out_Area) CutImage[k] = 0;  // 가장 큰 것만 저장(Size Filtering)
		//if (BlobArea[coloring[k]] > 500) CutImage[k] = 0;  // 특정 면적이상되는 영역(500)만 저장(Size Filtering)
		// CutImage[k] = (unsigned char)(coloring[k] * grayGap); // labeling된 모든 결과 출력
	}

	delete[] coloring;
	delete[] stackx;
	delete[] stacky;
}
// 라벨링 후 가장 넓은 영역에 대해서만 뽑아내는 코드 포함

// 외접사각형 꼭지점 찾기
void QuadVertex(BYTE *Img, int H, int W, int *LRxy){
	//LU_X, LU_Y, RD_X, RD_Y

	for (int i = 0; i < H; i++) { // y1(LU_Y) 좌측상단부터 우방향 가로우선 탐색
				if (LRxy[1] != 0) {
			break;
		}
		for (int j = 0; j < W; j++) {
			if (Img[i * W + j] == 0 && LRxy[1] == 0) {
				LRxy[1] = i - 1;
				break;
			}
		}
	}
	for (int i = H - 1; i >= 0; i--) { // y2(RD_Y) 우측하단부터 좌방향 가로우선 탐색
		if (LRxy[3] != 0) {
			break;
		}
		for (int j = W - 1; j >= 0; j--) {
			if (Img[i * W + j] == 0 && LRxy[3] == 0) {
				LRxy[3] = i + 1;
				break;
			}
		}
	}
	for (int i = 0; i < W; i++) { // x1(LU_X) 좌측상단부터 하방향 세로우선 탐색
		if (LRxy[0] != 0) {
			break;
		}
		for (int j = 0; j < H; j++) {
			if (Img[j * W + i] == 0 && LRxy[0] == 0) {
				LRxy[0] = i - 1;
				break;
			}
		}
	}
	for (int i = W - 1; i >= 0; i--) { // x2(RD_X) 우측하단부터 상방향 세로우선 탐색
		if (LRxy[2] != 0) {
			break;
		}
		for (int j = H - 1; j >= 0; j--) {
			if (Img[j * W + i] == 0 && LRxy[2] == 0) {
				LRxy[2] = i + 1;
				break;
			}
		}
	}
}

// 외접사각형 외곽라인 그리기
void DrawRectOutline(BYTE * Img, BYTE *Out, int W, int H, int LU_X, int LU_Y, int RD_X, int RD_Y) {
	// LU_X: 사각형의 좌측상단 X좌표, LU_Y: 사각형의 좌측상단 Y좌표,
	// RD_X: 사각형의 우측하단 X좌표, LU_Y: 사각형의 우측하단 Y좌표.
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			Out[i * W + j] = Img[i * W + j];
		}
	}

	for (int i = LU_X; i <= RD_X; i++) { // 윗면
		Out[(LU_Y ) * W + i] = 255;
	}
	for (int i = LU_Y; i <= RD_Y; i++) { // 왼쪽면
		Out[i * W + LU_X ] = 255;
	}
	for (int i = RD_Y; i >= LU_Y; i--) { // 오른쪽면
		Out[i * W + RD_X] = 255;
	}
	for (int i = RD_X; i >= LU_X; i--) { // 아랫면
		Out[(RD_Y) * W + i] = 255;
	}
}

// 무게중심 구하기
void CenterGravity(BYTE *Img, int H, int W, int *Cxy) {
	int cnt = 0;

	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			if (Img[i * W + j] == 0) { // 검출한 구역
				Cxy[0] += j;
				Cxy[1] += i;
				cnt++;
			}
		}
	}
	Cxy[0] /= cnt;
	Cxy[1] /= cnt;
}

// 무게중심을 통과하는 수직선
void DrawCrossLine(BYTE * Img, BYTE *Out, int W, int H, int Cx, int Cy) {
	// Cx: 가로/세로 라인이 교차되는 지점의 X좌표
	// Cy: 가로/세로 라인이 교차되는 지점의 Y좌표
	for (int i = 0; i < H; i++) { // 세로축
		Out[i * W + Cx] = 255;
	}
	for (int i = 0; i < W; i++) { // 가로축
		Out[Cy * W + i] = 255;
	}
}

int main() {
	// -----------------------------------
	// 영상 입력
	BITMAPFILEHEADER hf; // 14 bytes
	BITMAPINFOHEADER hinfo; // 40 bytes
	RGBQUAD hRGB[256]; // 4 * 256 = 1024 bytes

	FILE *fp; // 파일 오픈을 위한 파일 포인터 변수
	fp = fopen("pupil1.bmp", "rb"); // 파일 오픈, 읽기 위해 rb(read binary) , 이미지 파일은 전부 binary
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
	BYTE *Temp = (BYTE *)malloc(ImgSize); // 임시
	BYTE *Output = (BYTE *)malloc(ImgSize); // 영상 처리 결과 담기
	fread(Image, sizeof(BYTE), ImgSize, fp); // 영상의 화수 정보 담기
	fclose(fp); //파일 포인터와 영성파일 간의 연결 끊기
	int H = hinfo.biHeight, W = hinfo.biWidth;

	//---------------------------------------
	/*
	문제
	파일에서 동공 영역을 검출한 후,
	1) 동공 영역에 외접하는 직사각형을 그리고(라인 두께 1픽셀, 라인 밝기값 255)
	2) 동공 영역의 무게중심을 통과하는 수평선/수직선을 그리는(라인 두께 1픽셀, 라인 밝기값 255)
	*/

	// 영상 처리
	int Cxy[2] = {};
	int LRxy[4] = {};

	// 라벨링
	Binarization(Image, Temp, W, H, 50); // 이진화
	InverseImage(Temp, Temp, W, H); // 색상 반전
	m_BlobColoring(Temp, H, W); // Output에 라벨링한 결과 출력

	QuadVertex(Temp, H, W, LRxy); // 외접사각형 꼭지점 좌표 찾기
	DrawRectOutline(Image, Output, W, H, LRxy[0], LRxy[1], LRxy[2], LRxy[3]); // 외접사각형

	CenterGravity(Temp, H, W, Cxy); // 무게중심 구하기
	DrawCrossLine(Output, Output, W, H, Cxy[0], Cxy[1]); // 무게중심 수평선
	//---------------------------------------
	// 처리 결과 출력
	SaveBMPFile(hf, hinfo, hRGB, Output, hinfo.biWidth, hinfo.biHeight, "output_pupil1.bmp");

	free(Image); // 동적할당 해제 
	free(Output); // 하지않으면 memory leak 현상발생
	free(Temp);

	return 0;
}