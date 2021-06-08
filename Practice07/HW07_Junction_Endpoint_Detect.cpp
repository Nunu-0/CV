/*
�̸� : �̿���
�а� : �޸������������а�
�й� : 201810793
*/
/*
[����] ����ȭ �� �б��� �� ���� ���� �� ǥ���ϱ�
�ֱ� �ҽ��ڵ�� �Բ� ������ dilation.bmp���Ͽ� ����
����ȭ���� �Ϸ� ��,
�б����� ������ �����Ͽ� ǥ�����ִ� �Լ� �����ϱ�.
�� �Լ��� main�Լ����� ȣ���ؼ� ���� ����̹��� ��±��� �����ϴ� ���α׷� �ۼ�.

�Լ�����
void FeatureExtractThinImage(BYTE * Image, BYTE * Output, int W, int H);
parameter ����
- Image: ����ȭ�� ���� ����
- Output: �б��� �� ������ ǥ�õ� ����
*/

#pragma warning(disable:4996) // ����� ���� ����
#include <stdio.h>
#include <stdlib.h> // �����Ҵ�
#include <Windows.h> // ��Ʈ�� ��� ���� ����ü(BITMAPFILEHEADER, BITMAPINFOHEADER, RGBQUAD)
#include <math.h> // sin, cos

// �̹��� ����(����) (�̹���, ���, ����, ����)
void InverseImage(BYTE *Img, BYTE *Out, int W, int H) {
	int ImgSize = W * H;

	for (int i = 0; i < ImgSize; i++) {
		Out[i] = 255 - Img[i];
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

			// �б����� ���� ǥ��
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

	// week12 HW-----------
	Dliation(Image, Output, W, H);
	Dliation(Output, Image, W, H);
	Dliation(Image, Output, W, H);
	Erosion(Output, Image, W, H);
	Erosion(Image, Output, W, H);
	Erosion(Output, Image, W, H);
	InverseImage(Image, Image, W, H);
	zhangSuen(Image, Output, H, W); // �����νĿ� ���� ��� 
	InverseImage(Output, Image, W, H);
	FeatureExtractThinImage(Image, Output, H, W);

	//---------------------------------------
	// ó�� ��� ���
	SaveBMPFile(hf, hinfo, hRGB, Output, hinfo.biWidth, hinfo.biHeight, "output.bmp");

	free(Image); // �����Ҵ� ���� 
	free(Output); // ���������� memory leak ����߻�
	free(Temp);

	return 0;
}