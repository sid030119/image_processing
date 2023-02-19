#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include<Windows.h>
#define N 512
#define THRESH 150

void bubble_sort(int* arr);


void main() {
    char InFileName[30] = "noise_9.bmp";
    char OutFileName[30] = "out_median.bmp";
    char RawFileName[30] = "BOAT512.raw";
    FILE* infile;
    FILE* outfile;
    FILE* rawfile;
    infile = fopen(InFileName, "rb");
    outfile = fopen(OutFileName, "wb");
    rawfile = fopen(RawFileName, "rb");
    BITMAPFILEHEADER HF;
    BITMAPINFOHEADER IF;
    RGBQUAD hRGB[256];

    fread(&HF, sizeof(BITMAPFILEHEADER), 1, infile);
    fread(&IF, sizeof(BITMAPINFOHEADER), 1, infile);
    fread(hRGB, sizeof(RGBQUAD), 256, infile);

    BYTE* lpImg = new BYTE[IF.biSizeImage];
    fread(lpImg, sizeof(char), IF.biSizeImage, infile);

    BYTE* lpImg2 = new BYTE[IF.biSizeImage];
    fread(lpImg2, sizeof(char), IF.biSizeImage, rawfile);

    BYTE input_image[N][N] = { 0, };
    BYTE output_image[N][N] = { 0, };
    BYTE raw_image[N][N] = { 0, };

    for (int i = 0; i < 512; i++)
    {
        for (int j = 0; j < 512; j++)
        {
            input_image[i][j] = lpImg[(i * 512) + j];
        }
    }

    for (int i = 0; i < 512; i++)
    {
        for (int j = 0; j < 512; j++)
        {
            raw_image[511 - i][j] = lpImg2[(i * 512) + j];
        }
    }

    int arr[9];
    int s;
    for (int i = 1; i < N - 1; i++) {
        for (int j = 1; j < N - 1; j++) {
            for (int k = 0; k < 9; k++) {
                arr[k] = input_image[i - 1 + (int)k / 3][j - 1 + k % 3];
            }
            bubble_sort(arr);
            s = arr[4];
            output_image[i][j] = s >= 255 ? 255 : s < 0 ? 0 : s;

        }
    }

    int MSE = 0;
    for (int i = 1; i < N-1; i++) {
        for (int j = 1; j < N-1; j++) {
            MSE = MSE + pow(output_image[i][j] - raw_image[i][j], 2);
        }
    }
    MSE = MSE / (N * N);

    printf("%d", MSE);

    fwrite(&HF, sizeof(char), sizeof(BITMAPFILEHEADER), outfile);
    fwrite(&IF, sizeof(char), sizeof(BITMAPINFOHEADER), outfile);
    fwrite(hRGB, sizeof(RGBQUAD), 256, outfile);
    fwrite(output_image, sizeof(char), IF.biSizeImage, outfile);


}


void bubble_sort(int* arr) {
    int temp;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8 - i; j++) {
            if (arr[j] > arr[j + 1]) {
                temp = arr[j + 1];
                arr[j + 1] = arr[j];
                arr[j] = temp;
            }
        }
    }

}