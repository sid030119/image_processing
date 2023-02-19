#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<Windows.h>

int main()
{
    unsigned char raw_image[512][512] = { 0 };

    for (int i = 0; i < 512; i++) 
    {
        for (int j = 0; j < 100; j++) {
            raw_image[i][j] = (char)120;
        }

        for (int j = 100; j < 200; j++) {
            raw_image[i][j] = (char)((0.15) * (j - 100) + 120);
        }
        for (int j = 200; j < 280; j++) {
            raw_image[i][j] = (char)((9 / 8) * (j - 200) + 135);
        }
        for (int j = 280; j < 300; j++) {
            raw_image[i][j] = (char)((0.75) * (j - 280) + 225);
        }
        for (int j = 300; j < 512; j++) {
            raw_image[i][j] = (char)240;
        }
    }


    FILE* outfile = fopen("raw_image.raw", "wb");

    fwrite(raw_image, sizeof(char), 512 * 512, outfile);
    fclose(outfile);

    return 0;

}



