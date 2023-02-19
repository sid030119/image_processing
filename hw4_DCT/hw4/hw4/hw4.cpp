// hw4.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
#define _CRT_SECURE_NO_WARNINGS

#include "framework.h"
#include "hw4.h"
#include <iostream>
#include <string>


#define B_size 8                
#define  nint(x)      ( (x)<0. ? (int)((x)-0.5) : (int)((x)+0.5) )


#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

BYTE input_image[512][512] = { 0, };
BYTE spectrum_image[512][512] = { 0, };
BYTE out_image[512][512] = { 0, };
float MSE;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void dct8x8(int[][B_size]);
void idct8x8(int[][B_size]);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_HW4, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_HW4));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_HW4));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_HW4);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
        char InFileName[30] = "BOAT512.bmp";
        char SpectrumFileName[30] = "spectrum_boat.bmp"; //스펙트럼 이미지가 저장될 파일
        char OutFileName[30] = "boat_DCT.bmp";

    

    switch (message)
    {
    case WM_CREATE:
    {
        InvalidateRgn(hWnd, NULL, true);

        

        FILE* infile;
        FILE* outfile;
        FILE* spectrumfile;
        infile = fopen(InFileName, "rb");
        spectrumfile = fopen(SpectrumFileName, "wb");
        outfile = fopen(OutFileName, "wb");

        BITMAPFILEHEADER HF;
        BITMAPINFOHEADER IF;
        RGBQUAD hRGB[256];

        fread(&HF, sizeof(BITMAPFILEHEADER), 1, infile);
        fread(&IF, sizeof(BITMAPINFOHEADER), 1, infile);
        fread(hRGB, sizeof(RGBQUAD), 256, infile);

        BYTE* lpImg = new BYTE[IF.biSizeImage];
        fread(lpImg, sizeof(char), IF.biSizeImage, infile);




        for (int i = 0; i < 512; i++)
        {
            for (int j = 0; j < 512; j++)
            {
                input_image[i][j] = lpImg[(i * 512) + j];
            }
        }

        for (int i = 0; i < 64; i++) {
            for (int j = 0; j < 64; j++) {
                int temp[B_size][B_size] = { 0, };


                for (int k = 0; k < 8; k++) {
                    for (int q = 0; q < 8; q++) {
                        temp[k][q] = input_image[8 * i + k][8 * j + q];
                    }
                }
                dct8x8(temp);


                for (int k = 0; k < 8; k++) {
                    for (int q = 0; q < 8; q++) {
                        int s = temp[k][q];
                        spectrum_image[8 * i + k][8 * j + q] = s >= 255 ? 255 : s < 0 ? 0 : s;
                    }
                }

                idct8x8(temp);

                for (int k = 0; k < 8; k++) {
                    for (int q = 0; q < 8; q++) {
                        int s = temp[k][q];
                        out_image[8 * i + k][8 * j + q] = s >= 255 ? 255 : s < 0 ? 0 : s;
                    }
                }

            }
        }

        fwrite(&HF, sizeof(char), sizeof(BITMAPFILEHEADER), spectrumfile);
        fwrite(&IF, sizeof(char), sizeof(BITMAPINFOHEADER), spectrumfile);
        fwrite(hRGB, sizeof(RGBQUAD), 256, spectrumfile);
        fwrite(spectrum_image, sizeof(char), IF.biSizeImage, spectrumfile);
        fclose(spectrumfile);

        

        fwrite(&HF, sizeof(char), sizeof(BITMAPFILEHEADER), outfile);
        fwrite(&IF, sizeof(char), sizeof(BITMAPINFOHEADER), outfile);
        fwrite(hRGB, sizeof(RGBQUAD), 256, outfile);
        fwrite(out_image, sizeof(char), IF.biSizeImage, outfile);
        fclose(outfile);

        double temp_mse = 0;
        for (int i = 0; i < 512; i++) {
            for (int j = 0; j < 512; j++) {
                temp_mse += pow(input_image[i][j] - out_image[i][j], 2);
            }
        }
        MSE = temp_mse/262144;

    }
    break;


    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            HDC hdc, memdc;
            static HBITMAP hBitmap, hBase;
            PAINTSTRUCT ps;

            hdc = BeginPaint(hWnd, &ps);
            memdc = CreateCompatibleDC(hdc);
            hBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_spec_boat));
            SelectObject(memdc, hBitmap);
            StretchBlt(hdc, 0, 0, 512, 512, memdc, 0, 0, 512, 512, SRCCOPY);

            hBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_out_boat));
            SelectObject(memdc, hBitmap);
            StretchBlt(hdc, 600, 0, 512, 512, memdc, 0, 0, 512, 512, SRCCOPY);
            char str[20];
            sprintf(str, "MSE : %f", MSE);
            TextOut(hdc, 300 , 512, str, strlen(str));





            DeleteDC(memdc);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


void dct8x8(int ix[][B_size]) {
    static float pi = 3.141592653589793238;
    float x[B_size][B_size], z[B_size][B_size], y[B_size], c[40], s[40],
        ft[4], fxy[4], yy[B_size], zz;
    int i, ii, jj;

    for (i = 0; i < 40; i++) {
        zz = pi * (float)(i + 1) / 64.0;
        c[i] = cos(zz);
        s[i] = sin(zz);
    }

    for (ii = 0; ii < B_size; ii++)
        for (jj = 0; jj < B_size; jj++)
            x[ii][jj] = (float)ix[ii][jj];

    for (ii = 0; ii < B_size; ii++) {
        for (jj = 0; jj < B_size; jj++)
            y[jj] = x[ii][jj];

        for (jj = 0; jj < 4; jj++)
            ft[jj] = y[jj] + y[7 - jj];

        fxy[0] = ft[0] + ft[3];
        fxy[1] = ft[1] + ft[2];
        fxy[2] = ft[1] - ft[2];
        fxy[3] = ft[0] - ft[3];

        ft[0] = c[15] * (fxy[0] + fxy[1]);
        ft[2] = c[15] * (fxy[0] - fxy[1]);
        ft[1] = s[7] * fxy[2] + c[7] * fxy[3];
        ft[3] = -s[23] * fxy[2] + c[23] * fxy[3];

        for (jj = 4; jj < 8; jj++)
            yy[jj] = y[7 - jj] - y[jj];

        y[4] = yy[4];
        y[7] = yy[7];
        y[5] = c[15] * (-yy[5] + yy[6]);
        y[6] = c[15] * (yy[5] + yy[6]);

        yy[4] = y[4] + y[5];
        yy[5] = y[4] - y[5];
        yy[6] = -y[6] + y[7];
        yy[7] = y[6] + y[7];

        y[0] = ft[0];
        y[4] = ft[2];
        y[2] = ft[1];
        y[6] = ft[3];
        y[1] = s[3] * yy[4] + c[3] * yy[7];
        y[5] = s[19] * yy[5] + c[19] * yy[6];
        y[3] = -s[11] * yy[5] + c[11] * yy[6];
        y[7] = -s[27] * yy[4] + c[27] * yy[7];

        for (jj = 0; jj < B_size; jj++)
            z[ii][jj] = y[jj];

    }

    for (ii = 0; ii < B_size; ii++) {
        for (jj = 0; jj < B_size; jj++)
            y[jj] = z[jj][ii];

        for (jj = 0; jj < 4; jj++)
            ft[jj] = y[jj] + y[7 - jj];

        fxy[0] = ft[0] + ft[3];
        fxy[1] = ft[1] + ft[2];
        fxy[2] = ft[1] - ft[2];
        fxy[3] = ft[0] - ft[3];

        ft[0] = c[15] * (fxy[0] + fxy[1]);
        ft[2] = c[15] * (fxy[0] - fxy[1]);
        ft[1] = s[7] * fxy[2] + c[7] * fxy[3];
        ft[3] = -s[23] * fxy[2] + c[23] * fxy[3];

        for (jj = 4; jj < 8; jj++)
            yy[jj] = y[7 - jj] - y[jj];

        y[4] = yy[4];
        y[7] = yy[7];
        y[5] = c[15] * (-yy[5] + yy[6]);
        y[6] = c[15] * (yy[5] + yy[6]);

        yy[4] = y[4] + y[5];
        yy[5] = y[4] - y[5];
        yy[6] = -y[6] + y[7];
        yy[7] = y[6] + y[7];

        y[0] = ft[0];
        y[4] = ft[2];
        y[2] = ft[1];
        y[6] = ft[3];
        y[1] = s[3] * yy[4] + c[3] * yy[7];
        y[5] = s[19] * yy[5] + c[19] * yy[6];
        y[3] = -s[11] * yy[5] + c[11] * yy[6];
        y[7] = -s[27] * yy[4] + c[27] * yy[7];

        for (jj = 0; jj < B_size; jj++)
            y[jj] = y[jj] / 4.0;

        for (jj = 0; jj < B_size; jj++)
            z[jj][ii] = y[jj];
    }

    for (ii = 0; ii < B_size; ii++)
        for (jj = 0; jj < B_size; jj++)
            ix[ii][jj] = nint(z[ii][jj]);
}

void idct8x8(int ix[][B_size]) {
    static float pi = 3.141592653589793238;
    float x[B_size][B_size], z[B_size][B_size], y[B_size], c[40], s[40],
        ait[4], aixy[4], yy[B_size], zz;
    int i, ii, jj;

    for (i = 0; i < 40; i++) {
        zz = pi * (float)(i + 1) / 64.0;
        c[i] = cos(zz);
        s[i] = sin(zz);
    }

    for (ii = 0; ii < B_size; ii++)
        for (jj = 0; jj < B_size; jj++)
            x[ii][jj] = (float)ix[ii][jj];

    for (ii = 0; ii < B_size; ii++) {
        for (jj = 0; jj < B_size; jj++)
            y[jj] = x[jj][ii];

        ait[0] = y[0];
        ait[1] = y[2];
        ait[2] = y[4];
        ait[3] = y[6];

        aixy[0] = c[15] * (ait[0] + ait[2]);
        aixy[1] = c[15] * (ait[0] - ait[2]);
        aixy[2] = s[7] * ait[1] - s[23] * ait[3];
        aixy[3] = c[7] * ait[1] + c[23] * ait[3];

        ait[0] = aixy[0] + aixy[3];
        ait[1] = aixy[1] + aixy[2];
        ait[2] = aixy[1] - aixy[2];
        ait[3] = aixy[0] - aixy[3];

        yy[4] = s[3] * y[1] - s[27] * y[7];
        yy[5] = s[19] * y[5] - s[11] * y[3];
        yy[6] = c[19] * y[5] + c[11] * y[3];
        yy[7] = c[3] * y[1] + c[27] * y[7];

        y[4] = yy[4] + yy[5];
        y[5] = yy[4] - yy[5];
        y[6] = -yy[6] + yy[7];
        y[7] = yy[6] + yy[7];

        yy[4] = y[4];
        yy[7] = y[7];
        yy[5] = c[15] * (-y[5] + y[6]);
        yy[6] = c[15] * (y[5] + y[6]);

        for (jj = 0; jj < 4; jj++)
            y[jj] = ait[jj] + yy[7 - jj];

        for (jj = 4; jj < 8; jj++)
            y[jj] = ait[7 - jj] - yy[jj];

        for (jj = 0; jj < B_size; jj++)
            z[jj][ii] = y[jj];

    }

    for (ii = 0; ii < B_size; ii++) {
        for (jj = 0; jj < B_size; jj++)
            y[jj] = z[ii][jj];

        ait[0] = y[0];
        ait[1] = y[2];
        ait[2] = y[4];
        ait[3] = y[6];

        aixy[0] = c[15] * (ait[0] + ait[2]);
        aixy[1] = c[15] * (ait[0] - ait[2]);
        aixy[2] = s[7] * ait[1] - s[23] * ait[3];
        aixy[3] = c[7] * ait[1] + c[23] * ait[3];

        ait[0] = aixy[0] + aixy[3];
        ait[1] = aixy[1] + aixy[2];
        ait[2] = aixy[1] - aixy[2];
        ait[3] = aixy[0] - aixy[3];

        yy[4] = s[3] * y[1] - s[27] * y[7];
        yy[5] = s[19] * y[5] - s[11] * y[3];
        yy[6] = c[19] * y[5] + c[11] * y[3];
        yy[7] = c[3] * y[1] + c[27] * y[7];

        y[4] = yy[4] + yy[5];
        y[5] = yy[4] - yy[5];
        y[6] = -yy[6] + yy[7];
        y[7] = yy[6] + yy[7];

        yy[4] = y[4];
        yy[7] = y[7];
        yy[5] = c[15] * (-y[5] + y[6]);
        yy[6] = c[15] * (y[5] + y[6]);

        for (jj = 0; jj < 4; jj++)
            y[jj] = ait[jj] + yy[7 - jj];

        for (jj = 4; jj < 8; jj++)
            y[jj] = ait[7 - jj] - yy[jj];

        for (jj = 0; jj < B_size; jj++)
            z[ii][jj] = y[jj] / 4.0;

    }

    for (ii = 0; ii < B_size; ii++)
        for (jj = 0; jj < B_size; jj++)
            ix[ii][jj] = nint(z[ii][jj]);
}
