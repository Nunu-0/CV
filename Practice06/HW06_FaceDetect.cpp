/*
�̸� : �̿���
�а� : �޸������������а�
�й� : 201810793
*/
/*
[����] �󱼿��� ���� �� ���� �簢�� �׸���
÷���� BMP ���Ͽ��� �Ǻ� ������ �����ϰ�, �Ǻ� ������ �����ϴ� �ٿ�� �ڽ��� ���������� ǥ���ϴ� ���α׷� �ۼ�.

�������: ���������� �ľ��ϰ� �ִ� Cb, Cr �����ο����� �Ǻλ� range��

95 < Cb < 125
155 < Cr < 185
�Դϴ�. �����Ͻð�, �� �ȸ��� ��� ���������� �����ؼ� ������.

�����: HSI, RGB, YCbCr�� �پ��� �÷��𵨿����� �Ǻλ� range�� ���� ���Դϴ�.
*/

#pragma warning(disable:4996) // ����� ���� ����
#include <stdio.h>
#include <stdlib.h> // �����Ҵ�
#include <Windows.h> // ��Ʈ�� ��� ���� ����ü(BITMAPFILEHEADER, BITMAPINFOHEADER, RGBQUAD)

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

// �����簢�� ���� �� �ȼ�ã��
void Obtain2DBoundingBox(BYTE *Img, int W, int H, int *LU_X, int *LU_Y, int *RD_X, int *RD_Y) {
	//LU_X, LU_Y, RD_X, RD_Y
	int flag = 0;

	for (int i = 0; i < H; i++) { // y1(LU_Y) ������ܺ��� ����� ���ο켱 Ž��
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
	for (int i = H - 1; i >= 0; i--) { // y2(RD_Y) �����ϴܺ��� �¹��� ���ο켱 Ž��
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
	for (int j = 0; j < W; j++) { // x1(LU_X) ������ܺ��� �Ϲ��� ���ο켱 Ž��
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
	for (int j = W - 1; j >= 0; j--) { // x2(RD_X) �����ϴܺ��� ����� ���ο켱 Ž��
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

// �����簢�� �ܰ����� �׸���
void DrawRectOutline(BYTE * Img, BYTE *Out, int W, int H, int LU_X, int LU_Y, int RD_X, int RD_Y) {
	// LU_X: �簢���� ������� X��ǥ, LU_Y: �簢���� ������� Y��ǥ,
	// RD_X: �簢���� �����ϴ� X��ǥ, LU_Y: �簢���� �����ϴ� Y��ǥ.
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			Out[i * W * 3 + j * 3] = Img[i * W * 3 + j * 3]; // �� �ȼ��� rgb 3�ȼ��� ����� *3 Blue ����
			Out[i * W * 3 + j * 3 + 1] = Img[i * W * 3 + j * 3 + 1]; // Green ����
			Out[i * W * 3 + j * 3 + 2] = Img[i * W * 3 + j * 3 + 2]; // Red ����
		}
	}

	for (int i = LU_X; i <= RD_X; i++) { // ����
		Out[(LU_Y)* W * 3 + i * 3] = 0; // �� �ȼ��� rgb 3�ȼ��� ����� *3 Blue ����
		Out[(LU_Y)* W * 3 + i * 3 + 1] = 0; // Green ����
		Out[(LU_Y)* W * 3 + i * 3 + 2] = 255; // Red ����
	}
	for (int i = LU_Y; i <= RD_Y; i++) { // ���ʸ�
		Out[i * W * 3 + LU_X * 3] = 0; // �� �ȼ��� rgb 3�ȼ��� ����� *3 Blue ����
		Out[i * W * 3 + LU_X * 3 + 1] = 0; // Green ����
		Out[i * W * 3 + LU_X * 3 + 2] = 255; // Red ����
	}
	for (int i = RD_Y; i >= LU_Y; i--) { // �����ʸ�
		Out[i * W * 3 + RD_X * 3] = 0; // �� �ȼ��� rgb 3�ȼ��� ����� *3 Blue ����
		Out[i * W * 3 + RD_X * 3 + 1] = 0; // Green ����
		Out[i * W * 3 + RD_X * 3 + 2] = 255; // Red ����
	}
	for (int i = RD_X; i >= LU_X; i--) { // �Ʒ���
		Out[(RD_Y)* W * 3 + i * 3] = 0; // �� �ȼ��� rgb 3�ȼ��� ����� *3 Blue ����
		Out[(RD_Y)* W * 3 + i * 3 + 1] = 0; // Green ����
		Out[(RD_Y)* W * 3 + i * 3 + 2] = 255; // Red ����
	}
}

void RGB2YCbCr(BYTE *Image, BYTE *Y, BYTE *Cb, BYTE *Cr, int W, int H) {
	// Red ���� ū ȭ�Ҹ� masking (R, G, B, �� ����)
	for (int i = 0; i < H; i++) { // Y��ǥ
		for (int j = 0; j < W; j++) { // X��ǥ
			Y[i * W + j] = (BYTE)(0.299 * Image[i * W * 3 + j * 3 + 2] + 0.587 * Image[i * W * 3 + j * 3 + 1] + 0.114 * Image[i * W * 3 + j * 3]);
			Cb[i * W + j] = (BYTE)(-0.16874 * Image[i * W * 3 + j * 3 + 2] - 0.3313 * Image[i * W * 3 + j * 3 + 1] + 0.5 * Image[i * W * 3 + j * 3] + 128.0);
			Cr[i * W + j] = (BYTE)(0.5 * Image[i * W * 3 + j * 3 + 2] - 0.4187 * Image[i * W * 3 + j * 3 + 1] - 0.0813 * Image[i * W * 3 + j * 3] + 128.0);
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
	fp = fopen("face.bmp", "rb"); // ���� ����, �б� ���� rb(read binary) , �̹��� ������ ���� binary
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

	BYTE*Y = (BYTE*)malloc(ImgSize);
	BYTE*Cb = (BYTE*)malloc(ImgSize);
	BYTE*Cr = (BYTE*)malloc(ImgSize);

	RGB2YCbCr(Image, Y, Cb, Cr, W, H);
	// �� ���� �� masking(Y, Cb, Cr, �� ����)
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

	Obtain2DBoundingBox(Temp, W, H, &LU_X, &LU_Y, &RD_X, &RD_Y); // �����簢�� ������ ��ǥ ã��
	DrawRectOutline(Image, Output, W, H, LU_X, LU_Y, RD_X, RD_Y); // �����簢��

	//---------------------------------------
	// ó�� ��� ���
	SaveBMPFile(hf, hinfo, hRGB, Output, hinfo.biWidth, hinfo.biHeight, "output.bmp");

	free(Image); // �����Ҵ� ���� 
	free(Output); // ���������� memory leak ����߻�
	free(Temp);
	free(Y);
	free(Cb);
	free(Cr);

	return 0;
}