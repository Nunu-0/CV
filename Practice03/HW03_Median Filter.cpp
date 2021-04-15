/*
�̸� : �̿���
�а� : �޸������������а�
�й� : 201810793
*/
/*
[����] Median filter ����� ���� �����
���Ǹ� ���� 3*3 ������ �̵�����͸��� ������ҽ��ϴ�.

����, ���� �ڵ��� �ǹ̸� �����ϰ�, �ະ�� �ǹ��ϴ� �ٸ� �ּ�(//)ǥ�÷� �����Ͻÿ�.

5*5, 7*7, 9*9������ �̵�����͸��� ���� �����
median_5.bmp, median_7.bmp, median_9.bmp ���Ϸ� �����ϼ���.
*/

#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

// ó����� ��� �Լ�
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

// a, b ���� �ٲٱ�
void swap(BYTE* a, BYTE* b)
{
	BYTE temp = *a;
	*a = *b;
	*b = temp;
}

BYTE Median(BYTE* arr, int size)
{
	// bubble sorting ���
	// �������� ����
	const int S = size;
	for (int i = 0; i < size - 1; i++) // pivot index
	{
		for (int j = i + 1; j < size; j++) // �񱳴�� index
		{
			if (arr[i] > arr[j]) 	swap(&arr[i], &arr[j]);  // ��ġ �ٲٱ�
		}
	}
	return arr[S / 2]; // �߰��� return
}


int main()
{
	BITMAPFILEHEADER hf; // 14����Ʈ
	BITMAPINFOHEADER hInfo; // 40����Ʈ
	RGBQUAD hRGB[256]; // 1024����Ʈ
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
	BYTE * Temp = (BYTE*)malloc(ImgSize); // �ӽù迭
	BYTE* Output = (BYTE*)malloc(ImgSize);
	fread(Image, sizeof(BYTE), ImgSize, fp);
	fclose(fp);

	/* Median filtering */

	int Length = 3;  // ����ũ�� �� ���� ����
	int Margin = Length / 2; // ���� ���� ���� ó������ �ʱ� ���� length�� �߾Ӱ��� margin���� ����
	int WSize = Length * Length; // median filtering�� �� ������ ũ��
	BYTE* temp = (BYTE*)malloc(sizeof(BYTE) * WSize); // median  filtering��  ���� �ӽ� ����
	int W = hInfo.biWidth, H = hInfo.biHeight; // ���� �̹����� ����(W), ����(H) ������

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

	/* Median filtering */

	SaveBMPFile(hf, hInfo, hRGB, Output, hInfo.biWidth, hInfo.biHeight, "median_3.bmp");

	free(Image);
	free(Output);
	free(Temp);
	return 0;
}