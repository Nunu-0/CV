/*
이름 : 이연우
학과 : 휴먼지능정보공학과
학번 : 201810793
*/

#pragma warning(disable:4996) // 입출력 에러 막기
#include <stdio.h>
#include <stdlib.h> // 동적할당
#include <Windows.h> // 비트맵 헤더 관련 구조체(BITMAPFILEHEADER, BITMAPINFOHEADER, RGBQUAD)
#include <math.h> // sin, cos

// week3 ----------------------------------------
// 이미지 역상(반전) (이미지, 결과, 가로, 세로)
void InverseImage(BYTE *Img, BYTE *Out, int W, int H) {
	int ImgSize = W * H;

	for (int i = 0; i < ImgSize; i++) {
		Out[i] = 255 - Img[i];
	}
}

// 이미지 밝기 조절 (이미지, 결과, 가로, 세로, 밝기)
void BrightnessAdj(BYTE *Img, BYTE *Out, int W, int H, int Val) {
	int ImgSize = W * H;

	for (int i = 0; i < ImgSize; i++) {
		if (Img[i] + Val > 255) {
			Out[i] = 255;
		}
		else if (Img[i] + Val < 0) {
			Out[i] = 0;
		}
		else
			Out[i] = Img[i] + Val;
	}
}

// 이미지 대비 조절 (이미지, 결과, 가로, 세로, 대비)
void ContrastAdj(BYTE *Img, BYTE *Out, int W, int H, double Val) {
	int ImgSize = W * H;

	for (int i = 0; i < ImgSize; i++) {
		// 0보다 작은 경우는 없다.
		if (Img[i] * Val > 255.0) {
			Out[i] = 255;
		}
		else
			Out[i] = (BYTE)Img[i] * Val; // 강제 형변환 해주기
	}
}

// 히스토그램 만들기 (막대그래프, 화소가 각 몇개인지)
void ObtainHistogram(BYTE *Img, int *Histo, int W, int H) {
	int ImgSize = W * H;

	for (int i = 0; i < ImgSize; i++) {
		Histo[Img[i]]++;
	}
}

// 히스토그램 스트레칭
void HistogramStretching(BYTE *Img, BYTE *Out, int *Histo, int W, int H) {
	int ImgSize = W * H;
	BYTE Low, High;

	for (int i = 0; i < 256; i++) {
		if (Histo[i] != 0) {
			Low = i;
			break;
		}
	}
	for (int i = 255; i >= 0; i--) {
		if (Histo[i] != 0) {
			High = i;
			break;
		}
	}
	for (int i = 0; i < ImgSize; i++) {
		Out[i] = (BYTE)((Img[i] - Low) / (double)(High - Low) * 255.0); // 정규화(normalize)
	}
}

// 누적 히스토그램
void ObtainAHistogram(int *Histo, int * AHisto) {
	for (int i = 0; i < 256; i++) {
		for (int j = 0; j <= i; j++) {
			AHisto[i] += Histo[j];
		}
	}
}

// 히스토크램 평활화
void HistogramEqualization(BYTE *Img, BYTE *Out, int *AHisto, int W, int H) {
	int ImgSize = W * H;
	int Nt = W * H, Gmax = 255; // Nt = 화소 개수, Gmax = 최대 밝기
	double Ratio = Gmax / (double)Nt;

	BYTE NormSum[256]; // 정규화 합
	for (int i = 0; i < 256; i++) {
		NormSum[i] = (BYTE)(Ratio * AHisto[i]);
	}
	for (int i = 0; i < ImgSize; i++) {
		Out[i] = NormSum[Img[i]];
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

// week3 HW02----------------------------------------
//Gonzalez 자동이진화 임계치 구하기
BYTE DetermThGonzales(int *H) {
	BYTE ep = 3;
	BYTE Low, High;
	BYTE Th, NewTh;
	int G1 = 0, G2 = 0, cnt1 = 0, cnt2 = 0, mu1, mu2;

	// 초기 임계치
	for (int i = 0; i < 256; i++) {	// 밝기 최소값	
		if (H[i] != 0) {
			Low = i;
			break;
		}
	}
	for (int i = 255; i >= 0; i--) { // 밝기 최대값	
		if (H[i] != 0) {
			High = i;
			break;
		}
	}

	Th = (Low + High) / 2;

	while (1) {
		for (int i = Th + 1; i <= High; i++) {
			G1 += (H[i] * i); // i라는 밝기값의 개수 * 밝기값
			cnt1 += H[i];
		}

		for (int i = Low; i <= Th; i++) {
			G2 += (H[i] * i);
			cnt2 += H[i];
		}
		if (cnt1 == 0) cnt1 = 1;
		if (cnt2 == 0) cnt2 = 1;
		mu1 = G1 / cnt1;
		mu2 = G2 / cnt2;

		NewTh = (mu1 + mu2) / 2;

		if (abs(NewTh - Th) < ep) {
			Th = NewTh;
			break;
		}
		else {
			Th = NewTh;
		}
		G1 = G2 = cnt1 = cnt2 = 0;
	}
	return Th;
}

// week4 ----------------------------------------
// Convolution (평균화, 영상을 블러시킴), 박스평활화
void AverageConv(BYTE *Img, BYTE *Out, int W, int H) {
	double Kernel[3][3] = { 0.11111, 0.11111, 0.11111,
							0.11111, 0.11111, 0.11111,
							0.11111, 0.11111, 0.11111 };

	double SumProduct = 0.0;

	for (int i = 1; i < H - 1; i++) { //  영상 가로(Y좌표, 행) / 마진을 줘야해서 1, H - 1
		for (int j = 1; j < W - 1; j++) { // 영상 세로(X좌표, 열)
			for (int m = -1; m <= 1; m++) { // 커널의 가로 (행)
				for (int n = -1; n <= 1; n++) { // 커널의 세로 (열)
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
					// Img[i * W + j ] = 1차원배열에서 (j, i)의 위치를 구한 것 (Y * W + X)
					// 화소들은 1차원배열에 들어가 있기 때문
				}
			}
			Out[i * W + j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}

// Convolution, 가우시안 평활화 (센터에 가중치를 많이 줌)
void GaussAvrConv(BYTE *Img, BYTE *Out, int W, int H) {
	double Kernel[3][3] = { 0.0625, 0.125, 0.0625,
							0.125, 0.25, 0.125,
							0.0625, 0.125, 0.0625 };

	double SumProduct = 0.0;

	for (int i = 1; i < H - 1; i++) { //  영상 가로(Y좌표, 행) / 마진을 줘야해서 1, H - 1
		for (int j = 1; j < W - 1; j++) { // 영상 세로(X좌표, 열)
			for (int m = -1; m <= 1; m++) { // 커널의 가로 (행)
				for (int n = -1; n <= 1; n++) { // 커널의 세로 (열)
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
					// Img[i * W + j ] = 1차원배열에서 (j, i)의 위치를 구한 것 (Y * W + X)
					// 화소들은 1차원배열에 들어가 있기 때문
				}
			}
			Out[i * W + j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}

// Prewitt 마스크 X (가로축만 경계선)
void Prewitt_X_Conv(BYTE *Img, BYTE *Out, int W, int H) {
	double Kernel[3][3] = { -1.0, 0.0, 1.0,
							-1.0, 0.0, 1.0,
							-1.0, 0.0, 1.0 };

	double SumProduct = 0.0;

	for (int i = 1; i < H - 1; i++) { //  영상 가로(Y좌표, 행) / 마진을 줘야해서 1, H - 1
		for (int j = 1; j < W - 1; j++) { // 영상 세로(X좌표, 열)
			for (int m = -1; m <= 1; m++) { // 커널의 가로 (행)
				for (int n = -1; n <= 1; n++) { // 커널의 세로 (열)
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
					// Img[i * W + j ] = 1차원배열에서 (j, i)의 위치를 구한 것 (Y * W + X)
					// 화소들은 1차원배열에 들어가 있기 때문
				}
			}
			Out[i * W + j] = abs((long)SumProduct) / 3; // 0 ~ 755 -> 0 ~ 255 노멀라이즈
			SumProduct = 0.0;
		}
	}
}

// Prewitt 마스크 Y (세로축만 경계선)
void Prewitt_Y_Conv(BYTE *Img, BYTE *Out, int W, int H) {
	double Kernel[3][3] = { -1.0, -1.0, -1.0,
							0.0, 0.0, 0.0,
							1.0, 1.0, 1.0 };

	double SumProduct = 0.0;

	for (int i = 1; i < H - 1; i++) { //  영상 가로(Y좌표, 행) / 마진을 줘야해서 1, H - 1
		for (int j = 1; j < W - 1; j++) { // 영상 세로(X좌표, 열)
			for (int m = -1; m <= 1; m++) { // 커널의 가로 (행)
				for (int n = -1; n <= 1; n++) { // 커널의 세로 (열)
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
					// Img[i * W + j ] = 1차원배열에서 (j, i)의 위치를 구한 것 (Y * W + X)
					// 화소들은 1차원배열에 들어가 있기 때문
				}
			}
			Out[i * W + j] = abs((long)SumProduct) / 3; // 0 ~ 755 -> 0 ~ 255 노멀라이즈
			SumProduct = 0.0;
		}
	}
}

// Sobel 마스크 X (가로축만 경계선) prewitt보다 날카로운 경계
void Sobel_X_Conv(BYTE *Img, BYTE *Out, int W, int H) {
	double Kernel[3][3] = { -1.0, 0.0, 1.0,
							-2.0, 0.0, 2.0,
							-1.0, 0.0, 1.0 };

	double SumProduct = 0.0;

	for (int i = 1; i < H - 1; i++) { //  영상 가로(Y좌표, 행) / 마진을 줘야해서 1, H - 1
		for (int j = 1; j < W - 1; j++) { // 영상 세로(X좌표, 열)
			for (int m = -1; m <= 1; m++) { // 커널의 가로 (행)
				for (int n = -1; n <= 1; n++) { // 커널의 세로 (열)
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
					// Img[i * W + j ] = 1차원배열에서 (j, i)의 위치를 구한 것 (Y * W + X)
					// 화소들은 1차원배열에 들어가 있기 때문
				}
			}
			Out[i * W + j] = abs((long)SumProduct) / 4; // 0 ~ 1020 -> 0 ~ 255 노멀라이즈
			SumProduct = 0.0;
		}
	}
}

// Sobel 마스크 Y (세로축만 경계선)
void Sobel_Y_Conv(BYTE *Img, BYTE *Out, int W, int H) {
	double Kernel[3][3] = { -1.0, -2.0, -1.0,
							0.0, 0.0, 0.0,
							1.0, 2.0, 1.0 };

	double SumProduct = 0.0;

	for (int i = 1; i < H - 1; i++) { //  영상 가로(Y좌표, 행) / 마진을 줘야해서 1, H - 1
		for (int j = 1; j < W - 1; j++) { // 영상 세로(X좌표, 열)
			for (int m = -1; m <= 1; m++) { // 커널의 가로 (행)
				for (int n = -1; n <= 1; n++) { // 커널의 세로 (열)
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
					// Img[i * W + j ] = 1차원배열에서 (j, i)의 위치를 구한 것 (Y * W + X)
					// 화소들은 1차원배열에 들어가 있기 때문
				}
			}
			Out[i * W + j] = abs((long)SumProduct) / 4; // 0 ~ 1020 -> 0 ~ 255 노멀라이즈
			SumProduct = 0.0;
		}
	}
}

// Laplace(라플라시안) 마스크
void Laplace_Conv(BYTE *Img, BYTE *Out, int W, int H) {
	double Kernel[3][3] = { -1.0, -1.0, -1.0,
							-1.0, 8.0, -1.0,
							-1.0, -1.0, -1.0 };

	double SumProduct = 0.0;

	for (int i = 1; i < H - 1; i++) { //  영상 가로(Y좌표, 행) / 마진을 줘야해서 1, H - 1
		for (int j = 1; j < W - 1; j++) { // 영상 세로(X좌표, 열)
			for (int m = -1; m <= 1; m++) { // 커널의 가로 (행)
				for (int n = -1; n <= 1; n++) { // 커널의 세로 (열)
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
					// Img[i * W + j ] = 1차원배열에서 (j, i)의 위치를 구한 것 (Y * W + X)
					// 화소들은 1차원배열에 들어가 있기 때문
				}
			}
			Out[i * W + j] = abs((long)SumProduct) / 8; // 0 ~ 2040 -> 0 ~ 255 노멀라이즈
			SumProduct = 0.0;
		}
	}
}

// Laplace(라플라시안) 마스크 (영상의 밝기를 유지함)
void Laplace_Conv_DC(BYTE *Img, BYTE *Out, int W, int H) {
	double Kernel[3][3] = { -1.0, -1.0, -1.0,
							-1.0, 9.0, -1.0,
							-1.0, -1.0, -1.0 };

	double SumProduct = 0.0;

	for (int i = 1; i < H - 1; i++) { //  영상 가로(Y좌표, 행) / 마진을 줘야해서 1, H - 1
		for (int j = 1; j < W - 1; j++) { // 영상 세로(X좌표, 열)
			for (int m = -1; m <= 1; m++) { // 커널의 가로 (행)
				for (int n = -1; n <= 1; n++) { // 커널의 세로 (열)
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
					// Img[i * W + j ] = 1차원배열에서 (j, i)의 위치를 구한 것 (Y * W + X)
					// 화소들은 1차원배열에 들어가 있기 때문
				}
			}
			//Out[i * W + j] = abs((long)SumProduct) / 8; // 0 ~ 2040 -> 0 ~ 255 노멀라이즈
			if ((long)SumProduct > 255) //
				Out[i * W + j] = 255;
			else if (SumProduct < 0.0)
				Out[i * W + j] = 0;
			else
				Out[i * W * j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
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

// week5 ------------------
void swap(BYTE *a, BYTE *b) {
	BYTE temp = *a;
	*a = *b;
	*b = temp;
}

BYTE Median(BYTE *arr, int size) {
	// bubble sorting 사용
	// 오름차순 정렬
	const int S = size;
	for (int i = 0; i < size - 1; i++) { // pivot index
		for (int j = i + 1; j < size; j++) { // 비교대상 index
			if (arr[i] > arr[j]) {
				swap(&arr[i], &arr[j]); // 위치 바꾸기
			}
		}
	}
	return arr[S / 2]; // 중간값 return
}

BYTE MaxPooling(BYTE *arr, int size) {
	// bubble sorting 사용
	// 오름차순 정렬
	const int S = size;
	for (int i = 0; i < size - 1; i++) { // pivot index
		for (int j = i + 1; j < size; j++) { // 비교대상 index
			if (arr[i] > arr[j]) {
				swap(&arr[i], &arr[j]); // 위치 바꾸기
			}
		}
	}
	return arr[S - 1]; // 가장 큰 값 return
}

BYTE MinPooling(BYTE *arr, int size) {
	// bubble sorting 사용
	// 오름차순 정렬
	const int S = size;
	for (int i = 0; i < size - 1; i++) { // pivot index
		for (int j = i + 1; j < size; j++) { // 비교대상 index
			if (arr[i] > arr[j]) {
				swap(&arr[i], &arr[j]); // 위치 바꾸기
			}
		}
	}
	return arr[S - 1]; // 가장 작은 값 return
}


// week6 -----------
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

// 경계선 출력
void BinaryImageEdgeDetection(BYTE *Bin, BYTE *Out, int W, int H) {
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			if (Bin[i * W + j] == 0) { // 전경화소라면
				if (!(Bin[(i - 1) * W + j] == 0 && Bin[(i + 1) * W + j] == 0 &&
					Bin[i * W + j - 1] == 0 && Bin[i * W + j + 1] == 0)) { // 4방향 화소 중 전경화소가 있다면
					Out[i * W + j] = 255;
				}
			}
		}
	}
}

// week7----------------
// 위아래 플립
void VerticalFlip(BYTE *Img, int W, int H) {
	for (int i = 0; i < H / 2; i++) {
		for (int j = 0; j < W; j++) { // 한 행
			swap(&Img[i * W + j], &Img[(H - 1 - i) * W + j]);
		}
	}
}
// 좌우 플립
void HorizontalFlip(BYTE *Img, int W, int H) {
	for (int i = 0; i < W / 2; i++) { // X 좌표
		for (int j = 0; j < H; j++) { // Y 좌표
			swap(&Img[j * W + i], &Img[j * W + (W - 1 -j)]);
		}
	}
}
// 이동, 순방향사상
void Translation(BYTE *Image, BYTE *Output, int W, int H, int Tx, int Ty) {
	Ty *= -1; // bmp 영상은 반대로 돼 있어서 y축 -1

	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			if ((i + Ty < H && i + Ty >= 0) && (j + Tx < W && j + Tx >= 0)) //영상의 범위를 벗어난 경우
				Output[(i + Ty) * W + (j + Tx)] = Image[i * W + j]; //순반향사상
		}
	}
}
// 확대, 역방향사상 Hole문제 해결
void Scaling(BYTE *Image, BYTE *Output, int W, int H, double SF_X, double SF_Y) {
	int tmpX, tmpY;

	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			tmpX = (int)(j / SF_X); //역방향이라 *가 아니라 /를 한다
			tmpY = (int)(j / SF_Y);
			if (tmpY < H && tmpX < W) //영상의 범위를 벗어난 경우
				Output[i * W + j] = Image[tmpY * W + tmpX];
		}
	}
}
// 영상의 원점을 기준으로 회전, 역방향사상 Hole문제 해결
void Rotation(BYTE *Image, BYTE *Output, int W, int H, int Angle) {
	int tmpX, tmpY;
	double Radian = Angle * 3.141592 / 180.0;

	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			tmpX = (int)(cos(Radian) * j + sin(Radian) * i); //역방향이라 -sin를 +sin로
			tmpY = (int)(-sin(Radian) * j + cos(Radian) * i); // +sin를 -sin로
			if ((tmpY < H && tmpY >= 0) && (tmpX < W && tmpX >= 0)) //영상의 범위를 벗어난 경우
				Output[i * W + j] = Image[tmpY * W + tmpX];
		}
	}
}
// 영상의 C_X, C_Y를 기준으로 회전
void RotationCenter(BYTE *Image, BYTE *Output, int W, int H, int Angle, int C_X, int C_Y) {
	int tmpX, tmpY;
	double Radian = Angle * 3.141592 / 180.0;

	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			tmpX = (int)(cos(Radian) * (j - C_X) + sin(Radian) * (i - C_Y) + C_X); //역방향이라 -sin를 +sin로
			tmpY = (int)(-sin(Radian) * (j - C_X) + cos(Radian) * (i - C_Y) + C_Y); // +sin를 -sin로
			if ((tmpY < H && tmpY >= 0) && (tmpX < W && tmpX >= 0)) //영상의 범위를 벗어난 경우
				Output[i * W + j] = Image[tmpY * W + tmpX];
		}
	}
}

// week5 HW03----------
void MedianFiltering(BYTE *Image, BYTE *Output, int W, int H, int size) {

	int Length = size;  // 마스크의 한 변의 길이
	int Margin = Length / 2; // 원본 영상 밖은 처리하지 않기 위해 length의 중앙값을 margin으로 설정
	int WSize = Length * Length; // median filtering을 할 윈도우 크기
	BYTE* temp = (BYTE*)malloc(sizeof(BYTE) * WSize); // median  filtering을  위한 임시 변수

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
}

// week6 HW04----------
// 외접사각형 가장 겉 픽셀찾기
void Obtain2DBoundingBox(BYTE *Img, int W, int H, int *LU_X, int *LU_Y, int *RD_X, int *RD_Y) {
	//LU_X, LU_Y, RD_X, RD_Y
	int flag = 0;

	for (int i = 0; i < H; i++) { // y1(LU_Y) 좌측상단부터 우방향 가로우선 탐색
		for (int j = 0; j < W; j++) {
			if (Img[i * W + j] == 0) {
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
			if (Img[i * W + j] == 0) {
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
			if (Img[i * W + j] == 0) {
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
			if (Img[i * W + j] == 0) {
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
			Out[i * W + j] = Img[i * W + j];
		}
	}

	for (int i = LU_X; i <= RD_X; i++) { // 윗면
		Out[(LU_Y)* W + i] = 255;
	}
	for (int i = LU_Y; i <= RD_Y; i++) { // 왼쪽면
		Out[i * W + LU_X] = 255;
	}
	for (int i = RD_Y; i >= LU_Y; i--) { // 오른쪽면
		Out[i * W + RD_X] = 255;
	}
	for (int i = RD_X; i >= LU_X; i--) { // 아랫면
		Out[(RD_Y)* W + i] = 255;
	}
}

// 무게중심 구하기
void Obtain2DCenter(BYTE *Img, int W, int H, int *Cx, int *Cy) {
	int cnt = 0;
	int SumX = 0, SumY = 0;

	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			if (Img[i * W + j] == 0) { // 검출한 구역(동공영역이면)
				SumX += j;
				SumY += i;
				cnt++;
			}
		}
	}
	if (cnt == 0) cnt = 1;

	*Cx = SumX / cnt;
	*Cy = SumY / cnt;
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

// week 10---------------
// 특정칼라로 바꿔주기
void FillColor(BYTE *Image,int X, int Y, int W, int H, BYTE R, BYTE G, BYTE B) {
	Image[Y * W * 3 + X * 3] = B; // 한 픽셀에 rgb 3픽셀을 사용해 *3 Blue 성분
	Image[Y * W * 3 + X * 3 + 1] = G; // Green 성분
	Image[Y * W * 3 + X * 3 + 2] = R; // Red 성분
	// BGR 순서이다 (거꾸로)
}

// week 11----------------

void RGB2YCbCr(BYTE *Image, BYTE *Y, BYTE *Cb, BYTE *Cr, int W, int H) {
	// Red 값이 큰 화소만 masking (R, G, B, 모델 기준)
	for (int i = 0; i < H; i++) { // Y좌표
		for (int j = 0; j < W; j++) { // X좌표
			Y[i * W + j] = (BYTE)(0.299 * Image[i * W * 3 + j * 3 + 2] + 0.587 * Image[i * W * 3 + j * 3 + 1] + 0.114 * Image[i * W * 3 + j * 3]);
			Cb[i * W + j] = (BYTE)(-0.16874 * Image[i * W * 3 + j * 3 + 2] -0.3313 * Image[i * W * 3 + j * 3 + 1] + 0.5 * Image[i * W * 3 + j * 3] + 128.0);
			Cr[i * W + j] = (BYTE)(0.5 * Image[i * W * 3 + j * 3 + 2] - 0.4187 * Image[i * W * 3 + j * 3 + 1] - 0.0813 * Image[i * W * 3 + j * 3] + 128.0);
		}
	}
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

// week 10 HW---------------
//그라데이션 만들기
void gradation(BYTE *Image, BYTE *Output, int W, int H) {
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
}

// week 11 HW---------------
void DrawColorRectOutline(BYTE * Img, int W, int H, int LU_X, int LU_Y, int RD_X, int RD_Y, BYTE R, BYTE G, BYTE B) {
	// LU_X: 사각형의 좌측상단 X좌표, LU_Y: 사각형의 좌측상단 Y좌표,
	// RD_X: 사각형의 우측하단 X좌표, LU_Y: 사각형의 우측하단 Y좌표.

	for (int i = LU_X; i <= RD_X; i++) { // 윗면
		Img[(LU_Y)* W * 3 + i * 3] = B; // 한 픽셀에 rgb 3픽셀을 사용해 *3 Blue 성분
		Img[(LU_Y)* W * 3 + i * 3 + 1] = G; // Green 성분
		Img[(LU_Y)* W * 3 + i * 3 + 2] = R; // Red 성분
	}
	for (int i = LU_Y; i <= RD_Y; i++) { // 왼쪽면
		Img[i * W * 3 + LU_X * 3] = B; // 한 픽셀에 rgb 3픽셀을 사용해 *3 Blue 성분
		Img[i * W * 3 + LU_X * 3 + 1] = G; // Green 성분
		Img[i * W * 3 + LU_X * 3 + 2] = R; // Red 성분
	}
	for (int i = RD_Y; i >= LU_Y; i--) { // 오른쪽면
		Img[i * W * 3 + RD_X * 3] = B; // 한 픽셀에 rgb 3픽셀을 사용해 *3 Blue 성분
		Img[i * W * 3 + RD_X * 3 + 1] = G; // Green 성분
		Img[i * W * 3 + RD_X * 3 + 2] = R; // Red 성분
	}
	for (int i = RD_X; i >= LU_X; i--) { // 아랫면
		Img[(RD_Y)* W * 3 + i * 3] = B; // 한 픽셀에 rgb 3픽셀을 사용해 *3 Blue 성분
		Img[(RD_Y)* W * 3 + i * 3 + 1] = G; // Green 성분
		Img[(RD_Y)* W * 3 + i * 3 + 2] = R; // Red 성분
	}
}

// week 12 HW---------------
void FeatureExtractThinImage(BYTE * Image, BYTE * Output, int W, int H) {
	for (int i = 0; i < W * H; i++) Output[i] = Image[i];
	int cnt = 0;
	for (int i = 2; i < H - 2; i++) {
		for (int j = 2; j < W - 2; j++) {
			int cnt = 0;
			if (Image[i * W + j] == 0) {
				if (Image[(i - 1) * W + j - 1] == 0 && Image[(i - 1) * W + j] == 255) cnt++;
				if (Image[(i - 1) * W + j] == 0 && Image[(i - 1) * W + j + 1] == 255) cnt++;
				if (Image[(i - 1) * W + j + 1] == 0 && Image[i * W + j + 1] == 255) cnt++;
				if (Image[i * W + j + 1] == 0 && Image[(i + 1) * W + j + 1] == 255) cnt++;
				if (Image[i * W + j - 1] == 0 && Image[(i - 1) * W + j - 1] == 255) cnt++;
				if (Image[(i + 1) * W + j + 1] == 0 && Image[(i + 1) * W + j] == 255) cnt++;
				if (Image[(i + 1) * W + j] == 0 && Image[(i + 1) * W + j - 1] == 255) cnt++;
				if (Image[(i + 1) * W + j - 1] == 0 && Image[i * W + j - 1] == 255) cnt++;
			}
			if (cnt == 1) Output[i * W + j] = 128;
			else if (cnt >= 3) Output[i * W + j] = 128;
			cnt = 0;
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

	// week3 ----------------------------------------
	/*
	InverseImage(Image, Output, hinfo.biWidth, hinfo.biHeight); // 영상 반전시키기

	BrightnessAdj(Image, Output, hinfo.biWidth, hinfo.biHeight, 70); // 영상 밝기 조절하기

	BrightnessAdj(Image, Output, hinfo.biWidth, hinfo.biHeight, 0.5); // 영상 대비 변경하기

	int Histo[256] = { 0 }; // 히스토그램
	int AHisto[256] = { 0 }; // 누적 히스토그램

	ObtainHistogram(Image, Histo, hinfo.biWidth, hinfo.biHeight); // 히스토그램
	//	for (int i = 0; i < 256; i++) {		printf("%d", &Histo[i]);	} // 히스토그램 출력

	HistogramStretching(Image, Output, Histo, hinfo.biWidth, hinfo.biHeight); // 히스토그램 스트레칭

	ObtainAHistogram(Histo, AHisto); // 누적히스토그램 구하기

	HistogramEqualization(Image, Output, AHisto, hinfo.biWidth, hinfo.biHeight); // 히스토그램 평활화
	*/

	// week4 ----------------------------------------
	/*
	AverageConv(Image, Output, hinfo.biWidth, hinfo.biHeight); // 박스평활화

	GaussAvrConv(Image, Output, hinfo.biWidth, hinfo.biHeight); // 가우시안 평활화

																// x, y 축의 경계를 모두 출력하고 싶다면
	Prewitt_X_Conv(Image, Temp, hinfo.biWidth, hinfo.biHeight); // Prewitt 마스크 X
	Prewitt_Y_Conv(Temp, Output, hinfo.biWidth, hinfo.biHeight); // Prewitt 마스크 Y
	for (int i = 0; i < ImgSize; i++) {
		if (Temp[i] > Output[i])
			Output[i] = Temp[i];
	}
	Binarization(Image, Output, hinfo.biWidth, hinfo.biHeight, 80); // 이진화로 경계를 뚜렷하게

	Laplace_Conv(Image, Output, hinfo.biWidth, hinfo.biHeight); // 라플라시안

	GaussAvrConv(Image, Temp, hinfo.biWidth, hinfo.biHeight); // 가우시안 평활화로 노이즈 없애기
	Laplace_Conv_DC(Temp, Output, hinfo.biWidth, hinfo.biHeight); // 라플라시안

	*/

	// week5----------
	/*/ Median filtering
	BYTE temp[9];
	int W = hinfo.biWidth, H = hinfo.biHeight;
	int i, j;
	for (i = 1; i < H - 1; i++) {
		for (j = 1; i < W - 1; j++) {
			temp[0] = Image[(i - 1) * W + j - 1];
			temp[1] = Image[(i - 1) * W + j];
			temp[2] = Image[(i - 1) * W + j + 1];
			temp[3] = Image[i * W + j - 1];
			temp[4] = Image[i * W + j]; // 3 x 3 의 center
			temp[5] = Image[i * W + j + 1];
			temp[6] = Image[(i + 1) * W + j - 1];
			temp[7] = Image[(i + 1) * W + j];
			temp[8] = Image[(i + 1) * W + j + 1];
			Output[i * W + j] = Median(temp, 9); // temp을 정렬 후 중간에 있는 값을 return
			//Output[i * W + j] = MaxPooling(temp, 9); // papper 노이즈가 많이 껴있으면
			//Output[i * W + j] = MinPooling(temp, 9); // Papper 노이즈가 많이 생김
		}
	}
	*/

	//week2 HW01-------
	/*
	// 영상 처리 결과 출력 Output1
	SaveBMPFile(hf, hinfo, hRGB, Image, hinfo.biWidth, hinfo.biHeight, "output1.bmp");

	// Output2
	for (int i = 0; i < ImgSize; i++) { //원본 영상의 밝기값 50 증가
		Output[i] = Image[i] + 50; // (255보다 큰 값이 나오면 overflow(검하게 나옴))
	}
	SaveBMPFile(hf, hinfo, hRGB, Output, hinfo.biWidth, hinfo.biHeight, "output2.bmp");

	// output3
	for (int i = 0; i < ImgSize; i++) { //원본 영상의 반전 결과 영상
		Output[i] = 255 - Image[i];
	}
	SaveBMPFile(hf, hinfo, hRGB, Output, hinfo.biWidth, hinfo.biHeight, "output3.bmp");
	*/

	// week6----------
	/*
	// 라벨링
	Binarization(Image, Temp, W, H, 50); // 이진화
	InverseImage(Temp, Temp, W, H); // 색상 반전
	m_BlobColoring(Temp, H, W); // Output에 라벨링한 결과 출력

	// 경계값 4주변화소
	for (int i = 0; i < ImgSize; i++) {
		Output[i] = Image[i];
	}

	BinaryImageEdgeDetection(Temp, Output, W, H);
	*/

	// week3 HW02-----
	/*
	int Histo[256] = { 0 }; // 히스토그램
	BYTE Th;

	ObtainHistogram(Image, Histo, hinfo.biWidth, hinfo.biHeight); // 히스토그램
	Th = DetermThGonzales(Histo); // 경계값 T 초기값
	Binarization(Image, Output, hinfo.biWidth, hinfo.biHeight, Th); // 이진화
	*/

	// week5 HW03-----
	/*
	 Median filtering

	MedianFiltering(Image, Output, W, H, 11);

	*/

	// week7-----------
	/*
	// VerticalFlip(Image, W, H); // 영상 상하 뒤집기
	//  HorizontalFlip(Image, W, H); // 영상 좌우 뒤집기
	// Translation(Image, Output, W, H, 100, 40); // 이동
	// Scaling(Image, Output, W, H, 2.0, 0.7); // 확대
	//Rotation(Image, Output, W, H, 60); // 원점을 중심으로 회전
	RotationCenter(Image, Output, W, H, 45, 256, 256); //영상의 센터를 중심으로 회전하는 법을 실습..
	*/

	// week6 HW04-------
	/*
	int Cx, Cy;
	int LU_X, LU_Y, RD_X, RD_Y;

	// 라벨링
	Binarization(Image, Temp, W, H, 50); // 이진화
	InverseImage(Temp, Temp, W, H); // 색상 반전
	m_BlobColoring(Temp, H, W); // Output에 라벨링한 결과 출력

	Obtain2DBoundingBox(Temp, W, H, &LU_X, &LU_Y, &RD_X, &RD_Y); // 외접사각형 꼭지점 좌표 찾기
	DrawRectOutline(Image, Output, W, H, LU_X, LU_Y, RD_X, RD_Y); // 외접사각형

	Obtain2DCenter(Temp, W, H, &Cx, &Cy); // 무게중심 구하기
	DrawCrossLine(Output, Output, W, H, Cx, Cy); // 무게중심 수평선
	*/

	// week10 ----------
	/*
	// 한 줄 색칠하기
	for (int i = 0; i < W; i++) {
		FillColor(Image, i, 200, W, H, 255, 255, 0); // 특정 위치 컬러 바꾸기
	}

	// (50, 100) - (300, 400) 박스채우기
	for (int i = 100; i <= 400; i++) {
		for (int j = 50; j <= 300; j++) {
			FillColor(Image, j, i, W, H, 255, 255, 0); // 특정 위치 컬러 바꾸기
		}
	}
	// 4등분을 한후 RGB로 색 겹치기, 가로 띠 만들기
	// 초기화
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			Image[i * W * 3 + j * 3] = 0;
			Image[i * W * 3 + j * 3 + 1] = 0;
			Image[i * W * 3 + j * 3 + 2] = 0;
		}
	}
	// y좌표 기준 0 ~ 239 (아래)
	for (int i = 0; i < 240; i++) { // RED
		for (int j = 0; j < W; j++) {
			Image[i * W * 3 + j * 3 + 2] = 255;
		}
	}
	// y좌표 기준 120 ~ 359
	for (int i = 120; i < 360; i++) { // GREEN
		for (int j = 0; j < W; j++) {
			Image[i * W * 3 + j * 3 + 1] = 255;
		}
	}
	// y좌표 기준 340 ~ 479
	for (int i = 340; i < 480; i++) { // BLUE
		for (int j = 0; j < W; j++) {
			Image[i * W * 3 + j * 3] = 255;
		}
	}

	//그라데이션 만들기
	double wt; // 가중치
	for (int a = 0; a < H; a++) {
		for (int i = 0; i < W; i++) {
			wt = i / (double)(W - 1);
			Image[a * W * 3 + i * 3] = (BYTE)255 * (1.0 - wt); // Blue
			Image[a * W * 3 + i * 3 + 1] = 0; // Green
			Image[a * W * 3 + i * 3 + 2] = (BYTE)255 * wt; // Red
		}
	}
	*/


	// week11-----------
	/*
	// Red 값이 큰 화소만 masking (R, G, B, 모델 기준)
	for (int i = 0; i < H; i++) { // Y좌표
		for (int j = 0; j < W; j++) { // X좌표
			if (Image[i * W * 3 + j * 3 + 2] > 150 && Image[i * W * 3 + j * 3 + 1] < 50 && Image[i * W * 3 + j * 3] < 100) {
				Output[i * W * 3 + j * 3] = Image[i * W * 3 + j * 3];
				Output[i * W * 3 + j * 3 + 1] = Image[i * W * 3 + j * 3 + 1];
				Output[i * W * 3 + j * 3 + 2] = Image[i * W * 3 + j * 3 + 2];
			}
			else
				Output[i * W * 3 + j * 3] = Output[i * W * 3 + j * 3 + 1] = Output[i * W * 3 + j * 3 + 2] = 0;
		}
	}

	BYTE*Y = (BYTE*)malloc(ImgSize); 
	BYTE*Cb = (BYTE*)malloc(ImgSize);
	BYTE*Cr = (BYTE*)malloc(ImgSize);
	
	RGB2YCbCr(Image, Y, Cb, Cr, W, H);
	// 빨간색 딸기 영역 만 masking(Y, Cb, Cr, 모델 기준)
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			if (Cb[i * W + j] > 90 && Cr[i * W + j] > 180) {
				Output[i * W * 3 + j * 3] = Image[i * W * 3 + j * 3];
				Output[i * W * 3 + j * 3 + 1] = Image[i * W * 3 + j * 3 + 1];
				Output[i * W * 3 + j * 3 + 2] = Image[i * W * 3 + j * 3 + 2];
			}
			else
				Output[i * W * 3 + j * 3] = Output[i * W * 3 + j * 3 + 1] = Output[i * W * 3 + j * 3 + 2] = 0;
		}
	}

	fp = fopen("Y.raw", "wb");
	fwrite(Y, sizeof(BYTE), W * H, fp);
	fclose(fp);
	fp = fopen("Cb.raw", "wb");
	fwrite(Cb, sizeof(BYTE), W * H, fp);
	fclose(fp);
	fp = fopen("Cr.raw", "wb");
	fwrite(Cr, sizeof(BYTE), W * H, fp);
	fclose(fp);

	free(Y);
	free(Cb);
	free(Cr);
	*/

	// week12-----------
	/*
	Dliation(Image, Output, W, H);
	Dliation(Output, Image, W, H);
	Dliation(Image, Output, W, H);
	Erosion(Output, Image, W, H);
	Erosion(Image, Output, W, H);
	Erosion(Output, Image, W, H);
	InverseImage(Image, Image, W, H);
	zhangSuen(Image, Output, H, W); // 지문인식에 많이 사용 
	*/

	// week 11 HW---------------
	/*
	// 얼굴영역검출
	BYTE*Y = (BYTE*)malloc(ImgSize);
	BYTE*Cb = (BYTE*)malloc(ImgSize);
	BYTE*Cr = (BYTE*)malloc(ImgSize);

	RGB2YCbCr(Image, Y, Cb, Cr, W, H);
	// 얼굴 영역 만 masking(Y, Cb, Cr, 모델 기준)
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			if (Cb[i * W + j] > 95 && Cb[i * W + j] < 125 && Cr[i * W + j] > 90 && Y[i * W + j] > 90) {
				Output[i * W * 3 + j * 3] = Image[i * W * 3 + j * 3];
				Output[i * W * 3 + j * 3 + 1] = Image[i * W * 3 + j * 3 + 1];
				Output[i * W * 3 + j * 3 + 2] = Image[i * W * 3 + j * 3 + 2];
			}
			else
				Output[i * W * 3 + j * 3] = Output[i * W * 3 + j * 3 + 1] = Output[i * W * 3 + j * 3 + 2] = 0;
		}
	}

	BYTE *temp3 = (BYTE*)malloc(ImgSize);
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			if (Output[i * W * 3 + j * 3] == 0) {
				temp3[i * W + j] = 0;
			}
			else
				temp3[i * W + j] = 255;
		}
	}
	m_BlobColoring(temp3, H, W); //라벨링 결과가 가장 큰 영역만 남김

	int Cx, Cy;
	int LU_X, LU_Y, RD_X, RD_Y;

	Obtain2DBoundingBox(temp3, W, H, &LU_X, &LU_Y, &RD_X, &RD_Y); // 외접사각형 꼭지점 좌표 찾기
	DrawColorRectOutline(Image, W, H, LU_X, LU_Y, RD_X, RD_Y, 255, 0, 0); // 외접사각형
	*/
	
	// week 12 HW---------------
	Dliation(Image, Output, W, H);
	Dliation(Output, Image, W, H);
	Dliation(Image, Output, W, H);
	Erosion(Output, Image, W, H);
	Erosion(Image, Output, W, H);
	Erosion(Output, Image, W, H);
	InverseImage(Image, Image, W, H);
	zhangSuen(Image, Output, H, W); // 지문인식에 많이 사용 
	FeatureExtractThinImage(Output, Image, W, H);
//---------------------------------------
	// 처리 결과 출력
	SaveBMPFile(hf, hinfo, hRGB, Image, hinfo.biWidth, hinfo.biHeight, "output4.bmp");

	free(Image); // 동적할당 해제 
	free(Output); // 하지않으면 memory leak 현상발생
	free(Temp);
	//free(Y);
	//free(Cb);
	//free(Cr);
	//free(temp3);

	return 0;
}