#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<Windows.h>



int main()
{
    char RawFileName[30] = "raw_image.raw"; // bmp file�� ���� raw �̹���
    char InFileName[30] = "lena_bmp_512x512_new.bmp"; // �־��� ���� �̹��� �ޱ�
    char OutFileName[30] = "out.bmp"; // output bmp file �̹���
    const int width = 512;


    FILE* infile; //file pointer
    FILE* rawfile;
    FILE* outfile;
    infile = fopen(InFileName, "rb");
    rawfile = fopen(RawFileName, "rb");
    outfile = fopen(OutFileName, "wb");

    BITMAPFILEHEADER HF;
    BITMAPINFOHEADER IF;
    RGBQUAD hRGB[256];

    fread(&HF, sizeof(BITMAPFILEHEADER), 1, infile);
    fread(&IF, sizeof(BITMAPINFOHEADER), 1, infile);
    fread(hRGB, sizeof(RGBQUAD), 256, infile);

    BYTE* lpImg = new BYTE[IF.biSizeImage];
    fread(lpImg, sizeof(char), IF.biSizeImage, rawfile);

    unsigned char row[512][512] = { 0 };

    for (int i = 0; i < 512; i++)
    {
        for (int j = 0; j < 512; j++)
        {
            row[511 - j][i] = lpImg[(i * 512) + j];
        }
    }

    unsigned char temp = 0;
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 512; j++) {
            temp = row[i][j];
            row[i][j] = row[511 - i][j];
            row[511 - i][j] = temp;
        }

    }



    fwrite(&HF, sizeof(char), sizeof(BITMAPFILEHEADER), outfile);
    fwrite(&IF, sizeof(char), sizeof(BITMAPINFOHEADER), outfile);
    fwrite(hRGB, sizeof(RGBQUAD), 256, outfile);
    fwrite(row, sizeof(char), IF.biSizeImage, outfile);

}