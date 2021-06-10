/*
�̸� : �̿���
�а� : �޸������������а�
�й� : 201810793
*/

#pragma warning(disable:4996) // ����� ���� ����
#include <stdio.h>
#include <stdlib.h> // �����Ҵ�
#include <Windows.h> // ��Ʈ�� ��� ���� ����ü(BITMAPFILEHEADER, BITMAPINFOHEADER, RGBQUAD)
#include <math.h> // sin, cos

// week3 ----------------------------------------
// �̹��� ����(����) (�̹���, ���, ����, ����)
void InverseImage(BYTE *Img, BYTE *Out, int W, int H) {
	int ImgSize = W * H;

	for (int i = 0; i < ImgSize; i++) {
		Out[i] = 255 - Img[i];
	}
}

// �̹��� ��� ���� (�̹���, ���, ����, ����, ���)
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

// �̹��� ��� ���� (�̹���, ���, ����, ����, ���)
void ContrastAdj(BYTE *Img, BYTE *Out, int W, int H, double Val) {
	int ImgSize = W * H;

	for (int i = 0; i < ImgSize; i++) {
		// 0���� ���� ���� ����.
		if (Img[i] * Val > 255.0) {
			Out[i] = 255;
		}
		else
			Out[i] = (BYTE)Img[i] * Val; // ���� ����ȯ ���ֱ�
	}
}

// ������׷� ����� (����׷���, ȭ�Ұ� �� �����)
void ObtainHistogram(BYTE *Img, int *Histo, int W, int H) {
	int ImgSize = W * H;

	for (int i = 0; i < ImgSize; i++) {
		Histo[Img[i]]++;
	}
}

// ������׷� ��Ʈ��Ī
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
		Out[i] = (BYTE)((Img[i] - Low) / (double)(High - Low) * 255.0); // ����ȭ(normalize)
	}
}

// ���� ������׷�
void ObtainAHistogram(int *Histo, int * AHisto) {
	for (int i = 0; i < 256; i++) {
		for (int j = 0; j <= i; j++) {
			AHisto[i] += Histo[j];
		}
	}
}

// ������ũ�� ��Ȱȭ
void HistogramEqualization(BYTE *Img, BYTE *Out, int *AHisto, int W, int H) {
	int ImgSize = W * H;
	int Nt = W * H, Gmax = 255; // Nt = ȭ�� ����, Gmax = �ִ� ���
	double Ratio = Gmax / (double)Nt;

	BYTE NormSum[256]; // ����ȭ ��
	for (int i = 0; i < 256; i++) {
		NormSum[i] = (BYTE)(Ratio * AHisto[i]);
	}
	for (int i = 0; i < ImgSize; i++) {
		Out[i] = NormSum[Img[i]];
	}
}

// ����ȭ
void Binarization(BYTE *Img, BYTE *Out, int W, int H, BYTE Threshold) {
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) {
		if (Img[i] < Threshold) // �Ӱ�ġ���� ������ 0
			Out[i] = 0;
		else // ũ�� 255
			Out[i] = 255;
	}
}

// week3 HW02----------------------------------------
//Gonzalez �ڵ�����ȭ �Ӱ�ġ ���ϱ�
BYTE DetermThGonzales(int *H) {
	BYTE ep = 3;
	BYTE Low, High;
	BYTE Th, NewTh;
	int G1 = 0, G2 = 0, cnt1 = 0, cnt2 = 0, mu1, mu2;

	// �ʱ� �Ӱ�ġ
	for (int i = 0; i < 256; i++) {	// ��� �ּҰ�	
		if (H[i] != 0) {
			Low = i;
			break;
		}
	}
	for (int i = 255; i >= 0; i--) { // ��� �ִ밪	
		if (H[i] != 0) {
			High = i;
			break;
		}
	}

	Th = (Low + High) / 2;

	while (1) {
		for (int i = Th + 1; i <= High; i++) {
			G1 += (H[i] * i); // i��� ��Ⱚ�� ���� * ��Ⱚ
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
// Convolution (���ȭ, ������ ����Ŵ), �ڽ���Ȱȭ
void AverageConv(BYTE *Img, BYTE *Out, int W, int H) {
	double Kernel[3][3] = { 0.11111, 0.11111, 0.11111,
							0.11111, 0.11111, 0.11111,
							0.11111, 0.11111, 0.11111 };

	double SumProduct = 0.0;

	for (int i = 1; i < H - 1; i++) { //  ���� ����(Y��ǥ, ��) / ������ ����ؼ� 1, H - 1
		for (int j = 1; j < W - 1; j++) { // ���� ����(X��ǥ, ��)
			for (int m = -1; m <= 1; m++) { // Ŀ���� ���� (��)
				for (int n = -1; n <= 1; n++) { // Ŀ���� ���� (��)
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
					// Img[i * W + j ] = 1�����迭���� (j, i)�� ��ġ�� ���� �� (Y * W + X)
					// ȭ�ҵ��� 1�����迭�� �� �ֱ� ����
				}
			}
			Out[i * W + j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}

// Convolution, ����þ� ��Ȱȭ (���Ϳ� ����ġ�� ���� ��)
void GaussAvrConv(BYTE *Img, BYTE *Out, int W, int H) {
	double Kernel[3][3] = { 0.0625, 0.125, 0.0625,
							0.125, 0.25, 0.125,
							0.0625, 0.125, 0.0625 };

	double SumProduct = 0.0;

	for (int i = 1; i < H - 1; i++) { //  ���� ����(Y��ǥ, ��) / ������ ����ؼ� 1, H - 1
		for (int j = 1; j < W - 1; j++) { // ���� ����(X��ǥ, ��)
			for (int m = -1; m <= 1; m++) { // Ŀ���� ���� (��)
				for (int n = -1; n <= 1; n++) { // Ŀ���� ���� (��)
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
					// Img[i * W + j ] = 1�����迭���� (j, i)�� ��ġ�� ���� �� (Y * W + X)
					// ȭ�ҵ��� 1�����迭�� �� �ֱ� ����
				}
			}
			Out[i * W + j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}

// Prewitt ����ũ X (�����ุ ��輱)
void Prewitt_X_Conv(BYTE *Img, BYTE *Out, int W, int H) {
	double Kernel[3][3] = { -1.0, 0.0, 1.0,
							-1.0, 0.0, 1.0,
							-1.0, 0.0, 1.0 };

	double SumProduct = 0.0;

	for (int i = 1; i < H - 1; i++) { //  ���� ����(Y��ǥ, ��) / ������ ����ؼ� 1, H - 1
		for (int j = 1; j < W - 1; j++) { // ���� ����(X��ǥ, ��)
			for (int m = -1; m <= 1; m++) { // Ŀ���� ���� (��)
				for (int n = -1; n <= 1; n++) { // Ŀ���� ���� (��)
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
					// Img[i * W + j ] = 1�����迭���� (j, i)�� ��ġ�� ���� �� (Y * W + X)
					// ȭ�ҵ��� 1�����迭�� �� �ֱ� ����
				}
			}
			Out[i * W + j] = abs((long)SumProduct) / 3; // 0 ~ 755 -> 0 ~ 255 ��ֶ�����
			SumProduct = 0.0;
		}
	}
}

// Prewitt ����ũ Y (�����ุ ��輱)
void Prewitt_Y_Conv(BYTE *Img, BYTE *Out, int W, int H) {
	double Kernel[3][3] = { -1.0, -1.0, -1.0,
							0.0, 0.0, 0.0,
							1.0, 1.0, 1.0 };

	double SumProduct = 0.0;

	for (int i = 1; i < H - 1; i++) { //  ���� ����(Y��ǥ, ��) / ������ ����ؼ� 1, H - 1
		for (int j = 1; j < W - 1; j++) { // ���� ����(X��ǥ, ��)
			for (int m = -1; m <= 1; m++) { // Ŀ���� ���� (��)
				for (int n = -1; n <= 1; n++) { // Ŀ���� ���� (��)
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
					// Img[i * W + j ] = 1�����迭���� (j, i)�� ��ġ�� ���� �� (Y * W + X)
					// ȭ�ҵ��� 1�����迭�� �� �ֱ� ����
				}
			}
			Out[i * W + j] = abs((long)SumProduct) / 3; // 0 ~ 755 -> 0 ~ 255 ��ֶ�����
			SumProduct = 0.0;
		}
	}
}

// Sobel ����ũ X (�����ุ ��輱) prewitt���� ��ī�ο� ���
void Sobel_X_Conv(BYTE *Img, BYTE *Out, int W, int H) {
	double Kernel[3][3] = { -1.0, 0.0, 1.0,
							-2.0, 0.0, 2.0,
							-1.0, 0.0, 1.0 };

	double SumProduct = 0.0;

	for (int i = 1; i < H - 1; i++) { //  ���� ����(Y��ǥ, ��) / ������ ����ؼ� 1, H - 1
		for (int j = 1; j < W - 1; j++) { // ���� ����(X��ǥ, ��)
			for (int m = -1; m <= 1; m++) { // Ŀ���� ���� (��)
				for (int n = -1; n <= 1; n++) { // Ŀ���� ���� (��)
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
					// Img[i * W + j ] = 1�����迭���� (j, i)�� ��ġ�� ���� �� (Y * W + X)
					// ȭ�ҵ��� 1�����迭�� �� �ֱ� ����
				}
			}
			Out[i * W + j] = abs((long)SumProduct) / 4; // 0 ~ 1020 -> 0 ~ 255 ��ֶ�����
			SumProduct = 0.0;
		}
	}
}

// Sobel ����ũ Y (�����ุ ��輱)
void Sobel_Y_Conv(BYTE *Img, BYTE *Out, int W, int H) {
	double Kernel[3][3] = { -1.0, -2.0, -1.0,
							0.0, 0.0, 0.0,
							1.0, 2.0, 1.0 };

	double SumProduct = 0.0;

	for (int i = 1; i < H - 1; i++) { //  ���� ����(Y��ǥ, ��) / ������ ����ؼ� 1, H - 1
		for (int j = 1; j < W - 1; j++) { // ���� ����(X��ǥ, ��)
			for (int m = -1; m <= 1; m++) { // Ŀ���� ���� (��)
				for (int n = -1; n <= 1; n++) { // Ŀ���� ���� (��)
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
					// Img[i * W + j ] = 1�����迭���� (j, i)�� ��ġ�� ���� �� (Y * W + X)
					// ȭ�ҵ��� 1�����迭�� �� �ֱ� ����
				}
			}
			Out[i * W + j] = abs((long)SumProduct) / 4; // 0 ~ 1020 -> 0 ~ 255 ��ֶ�����
			SumProduct = 0.0;
		}
	}
}

// Laplace(���ö�þ�) ����ũ
void Laplace_Conv(BYTE *Img, BYTE *Out, int W, int H) {
	double Kernel[3][3] = { -1.0, -1.0, -1.0,
							-1.0, 8.0, -1.0,
							-1.0, -1.0, -1.0 };

	double SumProduct = 0.0;

	for (int i = 1; i < H - 1; i++) { //  ���� ����(Y��ǥ, ��) / ������ ����ؼ� 1, H - 1
		for (int j = 1; j < W - 1; j++) { // ���� ����(X��ǥ, ��)
			for (int m = -1; m <= 1; m++) { // Ŀ���� ���� (��)
				for (int n = -1; n <= 1; n++) { // Ŀ���� ���� (��)
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
					// Img[i * W + j ] = 1�����迭���� (j, i)�� ��ġ�� ���� �� (Y * W + X)
					// ȭ�ҵ��� 1�����迭�� �� �ֱ� ����
				}
			}
			Out[i * W + j] = abs((long)SumProduct) / 8; // 0 ~ 2040 -> 0 ~ 255 ��ֶ�����
			SumProduct = 0.0;
		}
	}
}

// Laplace(���ö�þ�) ����ũ (������ ��⸦ ������)
void Laplace_Conv_DC(BYTE *Img, BYTE *Out, int W, int H) {
	double Kernel[3][3] = { -1.0, -1.0, -1.0,
							-1.0, 9.0, -1.0,
							-1.0, -1.0, -1.0 };

	double SumProduct = 0.0;

	for (int i = 1; i < H - 1; i++) { //  ���� ����(Y��ǥ, ��) / ������ ����ؼ� 1, H - 1
		for (int j = 1; j < W - 1; j++) { // ���� ����(X��ǥ, ��)
			for (int m = -1; m <= 1; m++) { // Ŀ���� ���� (��)
				for (int n = -1; n <= 1; n++) { // Ŀ���� ���� (��)
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
					// Img[i * W + j ] = 1�����迭���� (j, i)�� ��ġ�� ���� �� (Y * W + X)
					// ȭ�ҵ��� 1�����迭�� �� �ֱ� ����
				}
			}
			//Out[i * W + j] = abs((long)SumProduct) / 8; // 0 ~ 2040 -> 0 ~ 255 ��ֶ�����
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

// ó����� ����ϱ�
void SaveBMPFile(BITMAPFILEHEADER hf, BITMAPINFOHEADER hinfo, RGBQUAD *hRGB, BYTE *Output, int W, int H, const char *FileName) {
	FILE *fp = fopen(FileName, "wb"); //bmp���� ����, wb = write binary
	if (hinfo.biBitCount == 24) {
		fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp); // fwrite(������ ����, ũ��, Ƚ��, ���� ���� ������)
		fwrite(&hinfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp); // ����� �Ҷ��� 1byte ������ �ؾ���
		fwrite(Output, sizeof(BYTE), W * H * 3, fp); // ���� ó�� ��� ���
	}
	else if (hinfo.biBitCount == 8) {
		fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp); // fwrite(������ ����, ũ��, Ƚ��, ���� ���� ������)
		fwrite(&hinfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp); // ����� �Ҷ��� 1byte ������ �ؾ���
		fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
		fwrite(Output, sizeof(BYTE), W * H, fp); // ���� ó�� ��� ���	
	}
	fclose(fp); // ���������Ϳ� �������� ���� ����
}

// week5 ------------------
void swap(BYTE *a, BYTE *b) {
	BYTE temp = *a;
	*a = *b;
	*b = temp;
}

BYTE Median(BYTE *arr, int size) {
	// bubble sorting ���
	// �������� ����
	const int S = size;
	for (int i = 0; i < size - 1; i++) { // pivot index
		for (int j = i + 1; j < size; j++) { // �񱳴�� index
			if (arr[i] > arr[j]) {
				swap(&arr[i], &arr[j]); // ��ġ �ٲٱ�
			}
		}
	}
	return arr[S / 2]; // �߰��� return
}

BYTE MaxPooling(BYTE *arr, int size) {
	// bubble sorting ���
	// �������� ����
	const int S = size;
	for (int i = 0; i < size - 1; i++) { // pivot index
		for (int j = i + 1; j < size; j++) { // �񱳴�� index
			if (arr[i] > arr[j]) {
				swap(&arr[i], &arr[j]); // ��ġ �ٲٱ�
			}
		}
	}
	return arr[S - 1]; // ���� ū �� return
}

BYTE MinPooling(BYTE *arr, int size) {
	// bubble sorting ���
	// �������� ����
	const int S = size;
	for (int i = 0; i < size - 1; i++) { // pivot index
		for (int j = i + 1; j < size; j++) { // �񱳴�� index
			if (arr[i] > arr[j]) {
				swap(&arr[i], &arr[j]); // ��ġ �ٲٱ�
			}
		}
	}
	return arr[S - 1]; // ���� ���� �� return
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

// GlassFire �˰����� �̿��� �󺧸� �Լ�
void m_BlobColoring(BYTE* CutImage, int height, int width)
{
	int i, j, m, n, top, area, Out_Area, index, BlobArea[1000];
	long k;
	short curColor = 0, r, c;
	//	BYTE** CutImage2;
	Out_Area = 1;


	// �������� ����� �޸� �Ҵ�
	short* stackx = new short[height * width];
	short* stacky = new short[height * width];
	short* coloring = new short[height * width];

	int arr_size = height * width;

	// �󺧸��� �ȼ��� �����ϱ� ���� �޸� �Ҵ�

	for (k = 0; k < height * width; k++) coloring[k] = 0;  // �޸� �ʱ�ȭ

	for (i = 0; i < height; i++)
	{
		index = i * width;
		for (j = 0; j < width; j++)
		{
			// �̹� �湮�� ���̰ų� �ȼ����� 255�� �ƴ϶�� ó�� ����
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
						//���� �ȼ��� �����踦 ����� ó�� ����
						if (m < 0 || m >= height || n < 0 || n >= width) continue;

						if ((int)CutImage[index + n] == 255 && coloring[index + n] == 0)
						{
							coloring[index + n] = curColor; // ���� �󺧷� ��ũ
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

	float grayGap = 255.0f / (float)curColor; // ������ �� ��

	// ���� ������ ���� ������ ã�Ƴ��� ���� 
	for (i = 1; i <= curColor; i++)
	{
		if (BlobArea[i] >= BlobArea[Out_Area]) Out_Area = i;
	}
	// CutImage �迭 Ŭ����~
	for (k = 0; k < width * height; k++) CutImage[k] = 255;

	// coloring�� ����� �󺧸� ����� (Out_Area�� �����) ������ ���� ū �͸� CutImage�� ����
	for (k = 0; k < width * height; k++)
	{
		if (coloring[k] == Out_Area) CutImage[k] = 0;  // ���� ū �͸� ����(Size Filtering)
		//if (BlobArea[coloring[k]] > 500) CutImage[k] = 0;  // Ư�� �����̻�Ǵ� ����(500)�� ����(Size Filtering)
		// CutImage[k] = (unsigned char)(coloring[k] * grayGap); // labeling�� ��� ��� ���
	}

	delete[] coloring;
	delete[] stackx;
	delete[] stacky;
}
// �󺧸� �� ���� ���� ������ ���ؼ��� �̾Ƴ��� �ڵ� ����

// ��輱 ���
void BinaryImageEdgeDetection(BYTE *Bin, BYTE *Out, int W, int H) {
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			if (Bin[i * W + j] == 0) { // ����ȭ�Ҷ��
				if (!(Bin[(i - 1) * W + j] == 0 && Bin[(i + 1) * W + j] == 0 &&
					Bin[i * W + j - 1] == 0 && Bin[i * W + j + 1] == 0)) { // 4���� ȭ�� �� ����ȭ�Ұ� �ִٸ�
					Out[i * W + j] = 255;
				}
			}
		}
	}
}

// week7----------------
// ���Ʒ� �ø�
void VerticalFlip(BYTE *Img, int W, int H) {
	for (int i = 0; i < H / 2; i++) {
		for (int j = 0; j < W; j++) { // �� ��
			swap(&Img[i * W + j], &Img[(H - 1 - i) * W + j]);
		}
	}
}
// �¿� �ø�
void HorizontalFlip(BYTE *Img, int W, int H) {
	for (int i = 0; i < W / 2; i++) { // X ��ǥ
		for (int j = 0; j < H; j++) { // Y ��ǥ
			swap(&Img[j * W + i], &Img[j * W + (W - 1 -j)]);
		}
	}
}
// �̵�, ��������
void Translation(BYTE *Image, BYTE *Output, int W, int H, int Tx, int Ty) {
	Ty *= -1; // bmp ������ �ݴ�� �� �־ y�� -1

	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			if ((i + Ty < H && i + Ty >= 0) && (j + Tx < W && j + Tx >= 0)) //������ ������ ��� ���
				Output[(i + Ty) * W + (j + Tx)] = Image[i * W + j]; //��������
		}
	}
}
// Ȯ��, �������� Hole���� �ذ�
void Scaling(BYTE *Image, BYTE *Output, int W, int H, double SF_X, double SF_Y) {
	int tmpX, tmpY;

	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			tmpX = (int)(j / SF_X); //�������̶� *�� �ƴ϶� /�� �Ѵ�
			tmpY = (int)(j / SF_Y);
			if (tmpY < H && tmpX < W) //������ ������ ��� ���
				Output[i * W + j] = Image[tmpY * W + tmpX];
		}
	}
}
// ������ ������ �������� ȸ��, �������� Hole���� �ذ�
void Rotation(BYTE *Image, BYTE *Output, int W, int H, int Angle) {
	int tmpX, tmpY;
	double Radian = Angle * 3.141592 / 180.0;

	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			tmpX = (int)(cos(Radian) * j + sin(Radian) * i); //�������̶� -sin�� +sin��
			tmpY = (int)(-sin(Radian) * j + cos(Radian) * i); // +sin�� -sin��
			if ((tmpY < H && tmpY >= 0) && (tmpX < W && tmpX >= 0)) //������ ������ ��� ���
				Output[i * W + j] = Image[tmpY * W + tmpX];
		}
	}
}
// ������ C_X, C_Y�� �������� ȸ��
void RotationCenter(BYTE *Image, BYTE *Output, int W, int H, int Angle, int C_X, int C_Y) {
	int tmpX, tmpY;
	double Radian = Angle * 3.141592 / 180.0;

	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			tmpX = (int)(cos(Radian) * (j - C_X) + sin(Radian) * (i - C_Y) + C_X); //�������̶� -sin�� +sin��
			tmpY = (int)(-sin(Radian) * (j - C_X) + cos(Radian) * (i - C_Y) + C_Y); // +sin�� -sin��
			if ((tmpY < H && tmpY >= 0) && (tmpX < W && tmpX >= 0)) //������ ������ ��� ���
				Output[i * W + j] = Image[tmpY * W + tmpX];
		}
	}
}

// week5 HW03----------
void MedianFiltering(BYTE *Image, BYTE *Output, int W, int H, int size) {

	int Length = size;  // ����ũ�� �� ���� ����
	int Margin = Length / 2; // ���� ���� ���� ó������ �ʱ� ���� length�� �߾Ӱ��� margin���� ����
	int WSize = Length * Length; // median filtering�� �� ������ ũ��
	BYTE* temp = (BYTE*)malloc(sizeof(BYTE) * WSize); // median  filtering��  ���� �ӽ� ����

	int i, j, m, n; // for���� ����� ���� ����
	for (i = Margin; i < H - Margin; i++) { // ���������� ��(���ι����̵�)
		for (j = Margin; j < W - Margin; j++) { // ���������� ��(���ι����̵�)
			//Length X Length(������) ȭ�Ұ��� temp�迭�� �ֱ�
			for (m = -Margin; m <= Margin; m++) { // �������� ��(���ι����̵�)
				for (n = -Margin; n <= Margin; n++) { // �������� ��(���ι����̵�)
					temp[(m + Margin) * Length + (n + Margin)] = Image[(i + m)*W + j + n]; // ������ ��ġ���� ȭ�Ұ��� temp�� �Է� 
				}
			}
			Output[i * W + j] = Median(temp, WSize); // Median() = temp�迭�� ���� �� �߰� ���� return�Ͽ� i��j�� ��ġ�� Output�� �ִ´�
		}
	}
	free(temp);
}

// week6 HW04----------
// �����簢�� ���� �� �ȼ�ã��
void Obtain2DBoundingBox(BYTE *Img, int W, int H, int *LU_X, int *LU_Y, int *RD_X, int *RD_Y) {
	//LU_X, LU_Y, RD_X, RD_Y
	int flag = 0;

	for (int i = 0; i < H; i++) { // y1(LU_Y) ������ܺ��� ����� ���ο켱 Ž��
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
	for (int i = H - 1; i >= 0; i--) { // y2(RD_Y) �����ϴܺ��� �¹��� ���ο켱 Ž��
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
	for (int j = 0; j < W; j++) { // x1(LU_X) ������ܺ��� �Ϲ��� ���ο켱 Ž��
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
	for (int j = W - 1; j >= 0; j--) { // x2(RD_X) �����ϴܺ��� ����� ���ο켱 Ž��
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

// �����簢�� �ܰ����� �׸���
void DrawRectOutline(BYTE * Img, BYTE *Out, int W, int H, int LU_X, int LU_Y, int RD_X, int RD_Y) {
	// LU_X: �簢���� ������� X��ǥ, LU_Y: �簢���� ������� Y��ǥ,
	// RD_X: �簢���� �����ϴ� X��ǥ, LU_Y: �簢���� �����ϴ� Y��ǥ.
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			Out[i * W + j] = Img[i * W + j];
		}
	}

	for (int i = LU_X; i <= RD_X; i++) { // ����
		Out[(LU_Y)* W + i] = 255;
	}
	for (int i = LU_Y; i <= RD_Y; i++) { // ���ʸ�
		Out[i * W + LU_X] = 255;
	}
	for (int i = RD_Y; i >= LU_Y; i--) { // �����ʸ�
		Out[i * W + RD_X] = 255;
	}
	for (int i = RD_X; i >= LU_X; i--) { // �Ʒ���
		Out[(RD_Y)* W + i] = 255;
	}
}

// �����߽� ���ϱ�
void Obtain2DCenter(BYTE *Img, int W, int H, int *Cx, int *Cy) {
	int cnt = 0;
	int SumX = 0, SumY = 0;

	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			if (Img[i * W + j] == 0) { // ������ ����(���������̸�)
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

// �����߽��� ����ϴ� ������
void DrawCrossLine(BYTE * Img, BYTE *Out, int W, int H, int Cx, int Cy) {
	// Cx: ����/���� ������ �����Ǵ� ������ X��ǥ
	// Cy: ����/���� ������ �����Ǵ� ������ Y��ǥ
	for (int i = 0; i < H; i++) { // ������
		Out[i * W + Cx] = 255;
	}
	for (int i = 0; i < W; i++) { // ������
		Out[Cy * W + i] = 255;
	}
}

// week 10---------------
// Ư��Į��� �ٲ��ֱ�
void FillColor(BYTE *Image,int X, int Y, int W, int H, BYTE R, BYTE G, BYTE B) {
	Image[Y * W * 3 + X * 3] = B; // �� �ȼ��� rgb 3�ȼ��� ����� *3 Blue ����
	Image[Y * W * 3 + X * 3 + 1] = G; // Green ����
	Image[Y * W * 3 + X * 3 + 2] = R; // Red ����
	// BGR �����̴� (�Ųٷ�)
}

// week 11----------------

void RGB2YCbCr(BYTE *Image, BYTE *Y, BYTE *Cb, BYTE *Cr, int W, int H) {
	// Red ���� ū ȭ�Ҹ� masking (R, G, B, �� ����)
	for (int i = 0; i < H; i++) { // Y��ǥ
		for (int j = 0; j < W; j++) { // X��ǥ
			Y[i * W + j] = (BYTE)(0.299 * Image[i * W * 3 + j * 3 + 2] + 0.587 * Image[i * W * 3 + j * 3 + 1] + 0.114 * Image[i * W * 3 + j * 3]);
			Cb[i * W + j] = (BYTE)(-0.16874 * Image[i * W * 3 + j * 3 + 2] -0.3313 * Image[i * W * 3 + j * 3 + 1] + 0.5 * Image[i * W * 3 + j * 3] + 128.0);
			Cr[i * W + j] = (BYTE)(0.5 * Image[i * W * 3 + j * 3 + 2] - 0.4187 * Image[i * W * 3 + j * 3 + 1] - 0.0813 * Image[i * W * 3 + j * 3] + 128.0);
		}
	}
}

// week 12--------------------(10������ ��� ����)
// ħ��
void Erosion(BYTE *Image, BYTE *Output, int W, int H) {
	for (int i = 1; i < H - 1; i++) {
		for (int j = 1; j < W - 1; j++) {
			if (Image[i * W + j] == 255) { // ����ȭ�Ҷ��
				if (!(Image[(i - 1) * W + j] == 255 && Image[(i + 1) * W + j] == 255 &&
					Image[i * W + j - 1] == 255 && Image[i * W + j + 1] == 255)) { //4�ֺ�ȭ�Ұ� ��� ���� ȭ�Ұ� �ƴ϶��
					Output[i * W + j] = 0;
				}
				else Output[i * W + j] = 255;
			}
			else Output[i * W + j] = 0;
		}
	}
}

// ��â
void Dliation(BYTE *Image, BYTE *Output, int W, int H) {
	for (int i = 1; i < H - 1; i++) {
		for (int j = 1; j < W - 1; j++) {
			if (Image[i * W + j] == 0) { // ���ȭ�Ҷ��
				if (!(Image[(i - 1) * W + j] == 0 && Image[(i + 1) * W + j] == 0 &&
					Image[i * W + j - 1] == 0 && Image[i * W + j + 1] == 0)) { //4�ֺ�ȭ�Ұ� ��� ���ȭ�Ұ� �ƴ϶��
					Output[i * W + j] = 255;
				}
				else Output[i * W + j] = 0;
			}
			else Output[i * W + j] = 255;
		}
	}
}

// 2���� �迭 �����Ҵ� ����
unsigned char** imageMatrix;

// �������󿡼� 
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

// Tining �˰���
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
//�׶��̼� �����
void gradation(BYTE *Image, BYTE *Output, int W, int H) {
	// �ʱ�ȭ
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			Image[i * W * 3 + j * 3] = 0;
			Image[i * W * 3 + j * 3 + 1] = 0;
			Image[i * W * 3 + j * 3 + 2] = 0;
		}
	}

	//�׶��̼� �����
	double wt; // ����ġ
	// y��ǥ ���� 0 ~ 239 (��) Red -> Cyan
	for (int i = H / 3 * 2; i < H; i++) {
		for (int j = 0; j < W; j++) {
			wt = j / (double)(W - 1);
			Image[i * W * 3 + j * 3] = (BYTE)255 * wt; // Blue
			Image[i * W * 3 + j * 3 + 1] = (BYTE)255 * wt; // Green
			Image[i * W * 3 + j * 3 + 2] = (BYTE)255 * (1.0 - wt); // Red
		}
	}
	// y��ǥ ���� 120 ~ 359 Green -> Magenta
	for (int i = H / 3; i < H / 3 * 2; i++) {
		for (int j = 0; j < W; j++) {
			wt = j / (double)(W - 1);
			Image[i * W * 3 + j * 3] = (BYTE)255 * wt; // Blue
			Image[i * W * 3 + j * 3 + 1] = (BYTE)255 * (1.0 - wt); // Green
			Image[i * W * 3 + j * 3 + 2] = (BYTE)255 * wt; // Red
		}
	}
	// y��ǥ ���� 340 ~ 479 Blue -> Yellow
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
	// LU_X: �簢���� ������� X��ǥ, LU_Y: �簢���� ������� Y��ǥ,
	// RD_X: �簢���� �����ϴ� X��ǥ, LU_Y: �簢���� �����ϴ� Y��ǥ.

	for (int i = LU_X; i <= RD_X; i++) { // ����
		Img[(LU_Y)* W * 3 + i * 3] = B; // �� �ȼ��� rgb 3�ȼ��� ����� *3 Blue ����
		Img[(LU_Y)* W * 3 + i * 3 + 1] = G; // Green ����
		Img[(LU_Y)* W * 3 + i * 3 + 2] = R; // Red ����
	}
	for (int i = LU_Y; i <= RD_Y; i++) { // ���ʸ�
		Img[i * W * 3 + LU_X * 3] = B; // �� �ȼ��� rgb 3�ȼ��� ����� *3 Blue ����
		Img[i * W * 3 + LU_X * 3 + 1] = G; // Green ����
		Img[i * W * 3 + LU_X * 3 + 2] = R; // Red ����
	}
	for (int i = RD_Y; i >= LU_Y; i--) { // �����ʸ�
		Img[i * W * 3 + RD_X * 3] = B; // �� �ȼ��� rgb 3�ȼ��� ����� *3 Blue ����
		Img[i * W * 3 + RD_X * 3 + 1] = G; // Green ����
		Img[i * W * 3 + RD_X * 3 + 2] = R; // Red ����
	}
	for (int i = RD_X; i >= LU_X; i--) { // �Ʒ���
		Img[(RD_Y)* W * 3 + i * 3] = B; // �� �ȼ��� rgb 3�ȼ��� ����� *3 Blue ����
		Img[(RD_Y)* W * 3 + i * 3 + 1] = G; // Green ����
		Img[(RD_Y)* W * 3 + i * 3 + 2] = R; // Red ����
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
	// ���� �Է�
	BITMAPFILEHEADER hf; // 14 bytes
	BITMAPINFOHEADER hinfo; // 40 bytes
	RGBQUAD hRGB[256]; // 4 * 256 = 1024 bytes

	FILE *fp; // ���� ������ ���� ���� ������ ����
	fp = fopen("dilation.bmp", "rb"); // ���� ����, �б� ���� rb(read binary) , �̹��� ������ ���� binary
	if (fp == NULL) { // ���� �����Ͱ� ������ ����Ű�� �ʰ��ִٸ�
		printf("File not found!/n");
		return -1;
	}
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp); // fread�� ���� �б�
	fread(&hinfo, sizeof(BITMAPINFOHEADER), 1, fp); // fread(������ ����, ũ��, Ƚ��, ���� ���� ������)
	int ImgSize = hinfo.biWidth * hinfo.biHeight; // ������ ȭ��(����������� ����, ���� ���� ��������)
	int H = hinfo.biHeight, W = hinfo.biWidth;
	BYTE *Image;
	BYTE *Output;
	BYTE *Temp;
	if (hinfo.biBitCount == 24) { // Ʈ�� �÷�
		Image = (BYTE *)malloc(ImgSize * 3); // �����Ҵ�, BYTE(0 ~ 255)
		Temp = (BYTE *)malloc(ImgSize * 3); // �ӽ�
		Output = (BYTE *)malloc(ImgSize * 3); // ���� ó�� ��� ���
		fread(Image, sizeof(BYTE), ImgSize * 3, fp); // ������ ȭ�� ���� ���
	}
	else { // (hinfo.biBitCount == 8) �ε���(�׷���)
		fread(hRGB, sizeof(RGBQUAD), 256, fp); // �迭�̸� ��ü�� �ּҶ� &��� x
		Image = (BYTE *)malloc(ImgSize); // �����Ҵ�, BYTE(0 ~ 255)
		Temp = (BYTE *)malloc(ImgSize); // �ӽ�
		Output = (BYTE *)malloc(ImgSize); // ���� ó�� ��� ���
		fread(Image, sizeof(BYTE), ImgSize, fp); // ������ ȭ�� ���� ���
	}

	fclose(fp); //���� �����Ϳ� �������� ���� ���� ����


	//---------------------------------------
	// ���� ó��

	// week3 ----------------------------------------
	/*
	InverseImage(Image, Output, hinfo.biWidth, hinfo.biHeight); // ���� ������Ű��

	BrightnessAdj(Image, Output, hinfo.biWidth, hinfo.biHeight, 70); // ���� ��� �����ϱ�

	BrightnessAdj(Image, Output, hinfo.biWidth, hinfo.biHeight, 0.5); // ���� ��� �����ϱ�

	int Histo[256] = { 0 }; // ������׷�
	int AHisto[256] = { 0 }; // ���� ������׷�

	ObtainHistogram(Image, Histo, hinfo.biWidth, hinfo.biHeight); // ������׷�
	//	for (int i = 0; i < 256; i++) {		printf("%d", &Histo[i]);	} // ������׷� ���

	HistogramStretching(Image, Output, Histo, hinfo.biWidth, hinfo.biHeight); // ������׷� ��Ʈ��Ī

	ObtainAHistogram(Histo, AHisto); // ����������׷� ���ϱ�

	HistogramEqualization(Image, Output, AHisto, hinfo.biWidth, hinfo.biHeight); // ������׷� ��Ȱȭ
	*/

	// week4 ----------------------------------------
	/*
	AverageConv(Image, Output, hinfo.biWidth, hinfo.biHeight); // �ڽ���Ȱȭ

	GaussAvrConv(Image, Output, hinfo.biWidth, hinfo.biHeight); // ����þ� ��Ȱȭ

																// x, y ���� ��踦 ��� ����ϰ� �ʹٸ�
	Prewitt_X_Conv(Image, Temp, hinfo.biWidth, hinfo.biHeight); // Prewitt ����ũ X
	Prewitt_Y_Conv(Temp, Output, hinfo.biWidth, hinfo.biHeight); // Prewitt ����ũ Y
	for (int i = 0; i < ImgSize; i++) {
		if (Temp[i] > Output[i])
			Output[i] = Temp[i];
	}
	Binarization(Image, Output, hinfo.biWidth, hinfo.biHeight, 80); // ����ȭ�� ��踦 �ѷ��ϰ�

	Laplace_Conv(Image, Output, hinfo.biWidth, hinfo.biHeight); // ���ö�þ�

	GaussAvrConv(Image, Temp, hinfo.biWidth, hinfo.biHeight); // ����þ� ��Ȱȭ�� ������ ���ֱ�
	Laplace_Conv_DC(Temp, Output, hinfo.biWidth, hinfo.biHeight); // ���ö�þ�

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
			temp[4] = Image[i * W + j]; // 3 x 3 �� center
			temp[5] = Image[i * W + j + 1];
			temp[6] = Image[(i + 1) * W + j - 1];
			temp[7] = Image[(i + 1) * W + j];
			temp[8] = Image[(i + 1) * W + j + 1];
			Output[i * W + j] = Median(temp, 9); // temp�� ���� �� �߰��� �ִ� ���� return
			//Output[i * W + j] = MaxPooling(temp, 9); // papper ����� ���� ��������
			//Output[i * W + j] = MinPooling(temp, 9); // Papper ����� ���� ����
		}
	}
	*/

	//week2 HW01-------
	/*
	// ���� ó�� ��� ��� Output1
	SaveBMPFile(hf, hinfo, hRGB, Image, hinfo.biWidth, hinfo.biHeight, "output1.bmp");

	// Output2
	for (int i = 0; i < ImgSize; i++) { //���� ������ ��Ⱚ 50 ����
		Output[i] = Image[i] + 50; // (255���� ū ���� ������ overflow(���ϰ� ����))
	}
	SaveBMPFile(hf, hinfo, hRGB, Output, hinfo.biWidth, hinfo.biHeight, "output2.bmp");

	// output3
	for (int i = 0; i < ImgSize; i++) { //���� ������ ���� ��� ����
		Output[i] = 255 - Image[i];
	}
	SaveBMPFile(hf, hinfo, hRGB, Output, hinfo.biWidth, hinfo.biHeight, "output3.bmp");
	*/

	// week6----------
	/*
	// �󺧸�
	Binarization(Image, Temp, W, H, 50); // ����ȭ
	InverseImage(Temp, Temp, W, H); // ���� ����
	m_BlobColoring(Temp, H, W); // Output�� �󺧸��� ��� ���

	// ��谪 4�ֺ�ȭ��
	for (int i = 0; i < ImgSize; i++) {
		Output[i] = Image[i];
	}

	BinaryImageEdgeDetection(Temp, Output, W, H);
	*/

	// week3 HW02-----
	/*
	int Histo[256] = { 0 }; // ������׷�
	BYTE Th;

	ObtainHistogram(Image, Histo, hinfo.biWidth, hinfo.biHeight); // ������׷�
	Th = DetermThGonzales(Histo); // ��谪 T �ʱⰪ
	Binarization(Image, Output, hinfo.biWidth, hinfo.biHeight, Th); // ����ȭ
	*/

	// week5 HW03-----
	/*
	 Median filtering

	MedianFiltering(Image, Output, W, H, 11);

	*/

	// week7-----------
	/*
	// VerticalFlip(Image, W, H); // ���� ���� ������
	//  HorizontalFlip(Image, W, H); // ���� �¿� ������
	// Translation(Image, Output, W, H, 100, 40); // �̵�
	// Scaling(Image, Output, W, H, 2.0, 0.7); // Ȯ��
	//Rotation(Image, Output, W, H, 60); // ������ �߽����� ȸ��
	RotationCenter(Image, Output, W, H, 45, 256, 256); //������ ���͸� �߽����� ȸ���ϴ� ���� �ǽ�..
	*/

	// week6 HW04-------
	/*
	int Cx, Cy;
	int LU_X, LU_Y, RD_X, RD_Y;

	// �󺧸�
	Binarization(Image, Temp, W, H, 50); // ����ȭ
	InverseImage(Temp, Temp, W, H); // ���� ����
	m_BlobColoring(Temp, H, W); // Output�� �󺧸��� ��� ���

	Obtain2DBoundingBox(Temp, W, H, &LU_X, &LU_Y, &RD_X, &RD_Y); // �����簢�� ������ ��ǥ ã��
	DrawRectOutline(Image, Output, W, H, LU_X, LU_Y, RD_X, RD_Y); // �����簢��

	Obtain2DCenter(Temp, W, H, &Cx, &Cy); // �����߽� ���ϱ�
	DrawCrossLine(Output, Output, W, H, Cx, Cy); // �����߽� ����
	*/

	// week10 ----------
	/*
	// �� �� ��ĥ�ϱ�
	for (int i = 0; i < W; i++) {
		FillColor(Image, i, 200, W, H, 255, 255, 0); // Ư�� ��ġ �÷� �ٲٱ�
	}

	// (50, 100) - (300, 400) �ڽ�ä���
	for (int i = 100; i <= 400; i++) {
		for (int j = 50; j <= 300; j++) {
			FillColor(Image, j, i, W, H, 255, 255, 0); // Ư�� ��ġ �÷� �ٲٱ�
		}
	}
	// 4����� ���� RGB�� �� ��ġ��, ���� �� �����
	// �ʱ�ȭ
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			Image[i * W * 3 + j * 3] = 0;
			Image[i * W * 3 + j * 3 + 1] = 0;
			Image[i * W * 3 + j * 3 + 2] = 0;
		}
	}
	// y��ǥ ���� 0 ~ 239 (�Ʒ�)
	for (int i = 0; i < 240; i++) { // RED
		for (int j = 0; j < W; j++) {
			Image[i * W * 3 + j * 3 + 2] = 255;
		}
	}
	// y��ǥ ���� 120 ~ 359
	for (int i = 120; i < 360; i++) { // GREEN
		for (int j = 0; j < W; j++) {
			Image[i * W * 3 + j * 3 + 1] = 255;
		}
	}
	// y��ǥ ���� 340 ~ 479
	for (int i = 340; i < 480; i++) { // BLUE
		for (int j = 0; j < W; j++) {
			Image[i * W * 3 + j * 3] = 255;
		}
	}

	//�׶��̼� �����
	double wt; // ����ġ
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
	// Red ���� ū ȭ�Ҹ� masking (R, G, B, �� ����)
	for (int i = 0; i < H; i++) { // Y��ǥ
		for (int j = 0; j < W; j++) { // X��ǥ
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
	// ������ ���� ���� �� masking(Y, Cb, Cr, �� ����)
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
	zhangSuen(Image, Output, H, W); // �����νĿ� ���� ��� 
	*/

	// week 11 HW---------------
	/*
	// �󱼿�������
	BYTE*Y = (BYTE*)malloc(ImgSize);
	BYTE*Cb = (BYTE*)malloc(ImgSize);
	BYTE*Cr = (BYTE*)malloc(ImgSize);

	RGB2YCbCr(Image, Y, Cb, Cr, W, H);
	// �� ���� �� masking(Y, Cb, Cr, �� ����)
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
	m_BlobColoring(temp3, H, W); //�󺧸� ����� ���� ū ������ ����

	int Cx, Cy;
	int LU_X, LU_Y, RD_X, RD_Y;

	Obtain2DBoundingBox(temp3, W, H, &LU_X, &LU_Y, &RD_X, &RD_Y); // �����簢�� ������ ��ǥ ã��
	DrawColorRectOutline(Image, W, H, LU_X, LU_Y, RD_X, RD_Y, 255, 0, 0); // �����簢��
	*/
	
	// week 12 HW---------------
	Dliation(Image, Output, W, H);
	Dliation(Output, Image, W, H);
	Dliation(Image, Output, W, H);
	Erosion(Output, Image, W, H);
	Erosion(Image, Output, W, H);
	Erosion(Output, Image, W, H);
	InverseImage(Image, Image, W, H);
	zhangSuen(Image, Output, H, W); // �����νĿ� ���� ��� 
	FeatureExtractThinImage(Output, Image, W, H);
//---------------------------------------
	// ó�� ��� ���
	SaveBMPFile(hf, hinfo, hRGB, Image, hinfo.biWidth, hinfo.biHeight, "output4.bmp");

	free(Image); // �����Ҵ� ���� 
	free(Output); // ���������� memory leak ����߻�
	free(Temp);
	//free(Y);
	//free(Cb);
	//free(Cr);
	//free(temp3);

	return 0;
}