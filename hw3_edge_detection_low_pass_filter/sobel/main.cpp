#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include<Windows.h>
#define N 512
#define THRESH 150


void main() {
    char InFileName[30] = "noise_8.bmp"; 
    char OutFileName[30] = "out_noise_sobel.bmp"; 
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

    BYTE input_image[N][N] = { 0, };
    int Gx, Gy;
    BYTE output_image[N][N] = { 0, };


    for (int i = 0; i < 512; i++)
    {
        for (int j = 0; j < 512; j++)
        {
            input_image[i][j] = lpImg[(i * 512) + j];
        }
    }
    int s;
    int cnt = 0;
    for (int i = 1; i < N-1; i++) {
        for (int j = 1; j < N-1; j++) {
            Gx = (input_image[i - 1][j - 1] + 2 * input_image[i][j - 1] + input_image[i + 1][j - 1] - input_image[i - 1][j + 1] - 2 * input_image[i][j + 1] - input_image[i + 1][j + 1]);
            Gy = (input_image[i - 1][j - 1] + 2 * input_image[i - 1][j] + input_image[i - 1][j + 1] - input_image[i + 1][j - 1] - 2 * input_image[i + 1][j] - input_image[i + 1][j + 1]);
            s = (unsigned int)sqrt(pow(Gx, 2) + pow(Gy, 2));
            if (s >= THRESH) cnt++;
            output_image[i][j] = s >= 255 ? 255 : s < 0 ? 0 : s;

        }
    }

    printf("sobel : %d", cnt);


    fwrite(&HF, sizeof(char), sizeof(BITMAPFILEHEADER), outfile);
    fwrite(&IF, sizeof(char), sizeof(BITMAPINFOHEADER), outfile);
    fwrite(hRGB, sizeof(RGBQUAD), 256, outfile);
    fwrite(output_image, sizeof(char), IF.biSizeImage, outfile);


}


