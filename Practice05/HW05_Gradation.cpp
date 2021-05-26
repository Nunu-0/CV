/*
�̸� : �̿���
�а� : �޸������������а�
�й� : 201810793
*/
/*
[����] 3�� �׶��̼� ��� ���α׷�
���� ���� ����ϴ� ���α׷� �ۼ� �� cpp �ҽ����ϸ� ����
Red ~~~~Cyan
Green ~~~~~Magenta
Blue ~~~~Yellow
*/

#pragma warning(disable:4996) // ����� ���� ����
#include <stdio.h>
#include <stdlib.h> // �����Ҵ�
#include <Windows.h> // ��Ʈ�� ��� ���� ����ü(BITMAPFILEHEADER, BITMAPINFOHEADER, RGBQUAD)
#include <math.h> // sin, cos

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

int main() {
	// -----------------------------------
	// ���� �Է�
	BITMAPFILEHEADER hf; // 14 bytes
	BITMAPINFOHEADER hinfo; // 40 bytes
	RGBQUAD hRGB[256]; // 4 * 256 = 1024 bytes

	FILE *fp; // ���� ������ ���� ���� ������ ����
	fp = fopen("TcSample.bmp", "rb"); // ���� ����, �б� ���� rb(read binary) , �̹��� ������ ���� binary
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

	// week10 HW ----------

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
	//---------------------------------------
	// ó�� ��� ���
	SaveBMPFile(hf, hinfo, hRGB, Image, hinfo.biWidth, hinfo.biHeight, "Gradation.bmp");

	free(Image); // �����Ҵ� ���� 
	free(Output); // ���������� memory leak ����߻�
	free(Temp);

	return 0;
}