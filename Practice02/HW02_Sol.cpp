/*
�̸� : �̿���
�а� : �޸������������а�
�й� : 201810793
*/
/*
[����] Gonzalez �ڵ� ����ȭ �Ӱ�ġ ���� ���α׷� ����
Gonzalez �˰��� �� �������ǿ� �ش��ϴ� �ԽǷ� ���� 3���Ϸ� �ϼ���.

���� § �ڵ嵵 ��谪�� 104�� ���°� ���� �ǽ��� ����
*/
#pragma warning(disable:4996) // ����� ���� ����
#include <stdio.h>
#include <stdlib.h> // �����Ҵ�
#include <Windows.h> // ��Ʈ�� ��� ���� ����ü(BITMAPFILEHEADER, BITMAPINFOHEADER, RGBQUAD)

// ������׷� ����� (����׷���, ȭ�Ұ� �� �����)
void ObtainHistogram(BYTE *Img, int *Histo, int W, int H) {
	int ImgSize = W * H;

	for (int i = 0; i < ImgSize; i++) {
		Histo[Img[i]]++;
	}
}

// ����ȭ
void Binarization(BYTE *Img, BYTE *Out, int W, int H, BYTE Threshold) {
	int ImgSize = W * H;

	for (int i = 0; i < ImgSize; i++) {
		if (Img[i] < Threshold) { // �Ӱ�ġ���� ������ 0
			Out[i] = 0;
		}
		else { // ũ�� 255
			Out[i] = 255;
		}
	}
}

// �ʱ� �Ӱ�ġ
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

int main() {
	// -----------------------------------
	// ���� �Է�
	BITMAPFILEHEADER hf; // 14 bytes
	BITMAPINFOHEADER hinfo; // 40 bytes
	RGBQUAD hRGB[256]; // 4 * 256 = 1024 bytes

	FILE *fp; // ���� ������ ���� ���� ������ ����
	fp = fopen("lenna.bmp", "rb"); // ���� ����, �б� ���� rb(read binary) , �̹��� ������ ���� binary
	if (fp == NULL) { // ���� �����Ͱ� ������ ����Ű�� �ʰ��ִٸ�
		printf("File not found!/n");
		return -1;
	}
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp); // fread�� ���� �б�
	fread(&hinfo, sizeof(BITMAPINFOHEADER), 1, fp); // fread(������ ����, ũ��, Ƚ��, ���� ���� ������)
	fread(hRGB, sizeof(RGBQUAD), 256, fp); // �迭�̸� ��ü�� �ּҶ� &��� x
	int ImgSize = hinfo.biWidth * hinfo.biHeight; // ������ ȭ��(����������� ����, ���� ���� ��������)
	BYTE *Image = (BYTE *)malloc(ImgSize); // �����Ҵ�, BYTE(0 ~ 255)
	// = BYTE Image[ImgSize], �������� ���
	BYTE *Output = (BYTE *)malloc(ImgSize); // ���� ó�� ��� ���
	fread(Image, sizeof(BYTE), ImgSize, fp); // ������ ȭ�� ���� ���
	fclose(fp); //���� �����Ϳ� �������� ���� ���� ����


	//---------------------------------------
	// ���� ó��

	int Histo[256] = { 0 }; // ������׷�
	BYTE Th;

	ObtainHistogram(Image, Histo, hinfo.biWidth, hinfo.biHeight); // ������׷�
	Th = DetermThGonzales(Histo); // ��谪 T �ʱⰪ
	Binarization(Image, Output, hinfo.biWidth, hinfo.biHeight, Th); // ����ȭ

	//---------------------------------------
	// ó�� ��� ���
	fp = fopen("output.bmp", "wb"); //bmp���� ����, wb = write binary
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp); // fwrite(������ ����, ũ��, Ƚ��, ���� ���� ������)
	fwrite(&hinfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp); // ����� �Ҷ��� 1byte ������ �ؾ���
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(Output, sizeof(BYTE), ImgSize, fp); // ���� ó�� ��� ���
	fclose(fp); // ���������Ϳ� �������� ���� ����

	free(Image); // �����Ҵ� ���� 
	free(Output); // ���������� memory leak ����߻�

	return 0;
}