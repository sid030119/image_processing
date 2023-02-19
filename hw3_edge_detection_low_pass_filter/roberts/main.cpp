#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include<Windows.h>
#define N 512
#define THRESH 150


void main() {
    char InFileName[30] = "lena_bmp_512x512_new.bmp"; // .bmp 흑백이미지 받기
    char OutFileName[30] = "out_original.bmp"; // 노이즈 추가된 이미지
    FILE* infile;
    FILE* outfile;
    infile = fopen(InFileName, "rb");
    outfile = fopen(OutFileName, "wb");

    BITMAPFILEHEADER HF;
    BITMAPINFOHEADER IF;
    RGBQUAD hRGB[256];

    fread(&HF, sizeof(BITMAPFILEHEADER), 1, infile);
    fread(&IF, sizeof(BITMAPINFOHEADER), 1, infile);
    fread(hRGB, sizeof(RGBQUAD), 256, infile);

    BYTE* lpImg = new BYTE[IF.biSizeImage];
    fread(lpImg, sizeof(char), IF.biSizeImage, infile);

    BYTE input_image[N][N] = {0,};
    int Gx, Gy;
    BYTE output_image[N][N] = {0,};


    for (int i = 0; i < 512; i++)
    {
        for (int j = 0; j < 512; j++)
        {
            input_image[i][j] = lpImg[(i * 512) + j];
        }
    }
    int s;
    int cnt = 0;
    for (int i = 1; i < N; i++) {
        for (int j = 1; j < N; j++) {
            Gx = input_image[i - 1][j - 1] - input_image[i][j];
            Gy = input_image[i - 1][j] - input_image[i][j - 1];
            s = (unsigned int)sqrt(pow(Gx,2) + pow(Gy,2));
            if (s >= THRESH) cnt++;
            output_image[i][j] = s >= 255 ? 255 : s <0 ? 0 : s;
            
        }
    }


    printf("roberts: %d", cnt);

    fwrite(&HF, sizeof(char), sizeof(BITMAPFILEHEADER), outfile);
    fwrite(&IF, sizeof(char), sizeof(BITMAPINFOHEADER), outfile);
    fwrite(hRGB, sizeof(RGBQUAD), 256, outfile);
    fwrite(output_image, sizeof(char), IF.biSizeImage, outfile);


}


