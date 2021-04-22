/*
�̸� : �̿���
�а� : �޸������������а�
�й� : 201810793
*/
/*
[����] �������� �����簢�� �� �����߽�����ϴ� ũ�ν����� �׸���

pupil1,bmp, pupil2.bmp ���Ͽ��� ���� ������ ������ ��,

1) ���� ������ �����ϴ� ���簢���� �׸���(���� �β� 1�ȼ�, ���� ��Ⱚ 255)

2) ���� ������ �����߽��� ����ϴ� ����/�������� �׸���(���� �β� 1�ȼ�, ���� ��Ⱚ 255)

���α׷��� �����Ͻÿ�.
*/

#pragma warning(disable:4996) // ����� ���� ����
#include <stdio.h>
#include <stdlib.h> // �����Ҵ�
#include <Windows.h> // ��Ʈ�� ��� ���� ����ü(BITMAPFILEHEADER, BITMAPINFOHEADER, RGBQUAD)

// �̹��� ����(����) (�̹���, ���, ����, ����)
void InverseImage(BYTE *Img, BYTE *Out, int W, int H) {
	int ImgSize = W * H;

	for (int i = 0; i < ImgSize; i++) {
		Out[i] = 255 - Img[i];
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

// ó����� ����ϱ�
void SaveBMPFile(BITMAPFILEHEADER hf, BITMAPINFOHEADER hinfo, RGBQUAD *hRGB, BYTE *Output, int W, int H, const char *FileName) {
	FILE *fp = fopen(FileName, "wb"); //bmp���� ����, wb = write binary
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp); // fwrite(������ ����, ũ��, Ƚ��, ���� ���� ������)
	fwrite(&hinfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp); // ����� �Ҷ��� 1byte ������ �ؾ���
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(Output, sizeof(BYTE), W * H, fp); // ���� ó�� ��� ���
	fclose(fp); // ���������Ϳ� �������� ���� ����

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

// �����簢�� ������ ã��
void QuadVertex(BYTE *Img, int H, int W, int *LRxy){
	//LU_X, LU_Y, RD_X, RD_Y

	for (int i = 0; i < H; i++) { // y1(LU_Y) ������ܺ��� ����� ���ο켱 Ž��
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
	for (int i = H - 1; i >= 0; i--) { // y2(RD_Y) �����ϴܺ��� �¹��� ���ο켱 Ž��
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
	for (int i = 0; i < W; i++) { // x1(LU_X) ������ܺ��� �Ϲ��� ���ο켱 Ž��
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
	for (int i = W - 1; i >= 0; i--) { // x2(RD_X) �����ϴܺ��� ����� ���ο켱 Ž��
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
		Out[(LU_Y ) * W + i] = 255;
	}
	for (int i = LU_Y; i <= RD_Y; i++) { // ���ʸ�
		Out[i * W + LU_X ] = 255;
	}
	for (int i = RD_Y; i >= LU_Y; i--) { // �����ʸ�
		Out[i * W + RD_X] = 255;
	}
	for (int i = RD_X; i >= LU_X; i--) { // �Ʒ���
		Out[(RD_Y) * W + i] = 255;
	}
}

// �����߽� ���ϱ�
void CenterGravity(BYTE *Img, int H, int W, int *Cxy) {
	int cnt = 0;

	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			if (Img[i * W + j] == 0) { // ������ ����
				Cxy[0] += j;
				Cxy[1] += i;
				cnt++;
			}
		}
	}
	Cxy[0] /= cnt;
	Cxy[1] /= cnt;
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

int main() {
	// -----------------------------------
	// ���� �Է�
	BITMAPFILEHEADER hf; // 14 bytes
	BITMAPINFOHEADER hinfo; // 40 bytes
	RGBQUAD hRGB[256]; // 4 * 256 = 1024 bytes

	FILE *fp; // ���� ������ ���� ���� ������ ����
	fp = fopen("pupil1.bmp", "rb"); // ���� ����, �б� ���� rb(read binary) , �̹��� ������ ���� binary
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
	BYTE *Temp = (BYTE *)malloc(ImgSize); // �ӽ�
	BYTE *Output = (BYTE *)malloc(ImgSize); // ���� ó�� ��� ���
	fread(Image, sizeof(BYTE), ImgSize, fp); // ������ ȭ�� ���� ���
	fclose(fp); //���� �����Ϳ� �������� ���� ���� ����
	int H = hinfo.biHeight, W = hinfo.biWidth;

	//---------------------------------------
	/*
	����
	���Ͽ��� ���� ������ ������ ��,
	1) ���� ������ �����ϴ� ���簢���� �׸���(���� �β� 1�ȼ�, ���� ��Ⱚ 255)
	2) ���� ������ �����߽��� ����ϴ� ����/�������� �׸���(���� �β� 1�ȼ�, ���� ��Ⱚ 255)
	*/

	// ���� ó��
	int Cxy[2] = {};
	int LRxy[4] = {};

	// �󺧸�
	Binarization(Image, Temp, W, H, 50); // ����ȭ
	InverseImage(Temp, Temp, W, H); // ���� ����
	m_BlobColoring(Temp, H, W); // Output�� �󺧸��� ��� ���

	QuadVertex(Temp, H, W, LRxy); // �����簢�� ������ ��ǥ ã��
	DrawRectOutline(Image, Output, W, H, LRxy[0], LRxy[1], LRxy[2], LRxy[3]); // �����簢��

	CenterGravity(Temp, H, W, Cxy); // �����߽� ���ϱ�
	DrawCrossLine(Output, Output, W, H, Cxy[0], Cxy[1]); // �����߽� ����
	//---------------------------------------
	// ó�� ��� ���
	SaveBMPFile(hf, hinfo, hRGB, Output, hinfo.biWidth, hinfo.biHeight, "output_pupil1.bmp");

	free(Image); // �����Ҵ� ���� 
	free(Output); // ���������� memory leak ����߻�
	free(Temp);

	return 0;
}