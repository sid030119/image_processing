/*WinMain은 별다른 점 없으므로 안보셔두 됩니다.
  WinProc 부분만 보시면 됩니다.

  기본적인 출력은 WM_PAINT 부분에서 모두 처리하고 있습니다.
  WinProc에서 메세지를 처리하기전 부분에서 파일입력및 선언을 처리하고
  WM_PAINT 부분에서 각각의 연산, 히스토그램 출력, 파일 출력을 처리하고 있습니다.

  WM_COMMAND 부분에서는 윈도우 창의 메뉴에서 이미지 변환을 클릭할 경우를 처리하고 있고
  각각 bool 타입의 변수를 지정해서 어떤 연산을 해야하는지 정해주고 있습니다.

  WM_PAINT 부분에서는 위의 bool 변수들의 값을 분기로 해당 연산을 처리하고 히스토그램과 파일을 출력합니다.

  맨처음 실행의 경우 연산없이 처음 이미지 그대로 히스토그램과 파일이 출력됩니다.

  #ECS(End-in Contrast Stretching)의 경우 ECS_MAX 와 ECS_MIN을 #define 으로 선언했습니다.
*/

#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <TCHAR.H>
#include "Resource.h"
#include <string.h>
#include <stdio.h>
#include <cmath>
#include <climits>
#define ECS_MAX 190
#define ECS_MIN 50

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg,
	WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpszCmdLine, int nCmdShow)
	// UNICODE 사용시 wWinMain() 형태 	
	// hPrevInstance 이전 인스턴스 항상 0값
	// lpszCmdLine > 외부에서 (내부로) 입력받는 변수
	// nCmdShow 윈도우 출력 형태에 관련한 값
{
	HWND     hwnd;
	MSG		 msg;
	WNDCLASS WndClass;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;	//height, vertical redraw
	WndClass.lpfnWndProc = WndProc;		// Proc 설정
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	// 형변환
	WndClass.lpszMenuName = MAKEINTRESOURCE(IDC_MENU1);
	
	WndClass.lpszClassName = _T("Window Class Name");
	RegisterClass(&WndClass);		// WndClass 등록
	hwnd = CreateWindow(_T("Window Class Name"),
		_T("20112233_이름"),		// 타이틀바, 학번이름 체크
		WS_OVERLAPPEDWINDOW,		// 윈도우 스타일
		600, 400,					// 창출력좌표 x, y 
		600, 400,					// 창크기 x, y축
		NULL,						// 부모 윈도우
		NULL,						// 메뉴바 핸들
		hInstance,					// 인스턴스
		NULL						// 여분, NULL
	);
	ShowWindow(hwnd, nCmdShow);		// 윈도우 출력, WM_PAINT 출력내용 가져옴
	UpdateWindow(hwnd);				// WM_PAINT 출력내용 발생해서 출력하도록
									// hwnd 핸들을 통해 보여주고 갱신

	//ShowWindow(hwnd, SW_SHOW);	// 위와 같음
	//UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0))	// 메시지 큐의 메시지를 가져옴
	{
		TranslateMessage(&msg);		// 키입력에 반응하는 메시지 변환, WM_KEYDOWN (키가 눌릴때) WM_CHAR 메시지 발생
		DispatchMessage(&msg);		// WndProc() 함수 호출과 WndProc()으로 메세지 전달
	}								// 종료는 WM_QUIT 발생할때 FALSE 리턴하면서 종료
	return (int)msg.wParam;			// wParam, lParam 윈도우 크기가 어떻게 변했는지, 변경된 클라이언트, 키보드, 마우스 값

}

//WinProc() 


LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg,
	WPARAM wParam, LPARAM lParam)
	//	WinDef.h 에서 정의
	//	wPram > unsigned ptr, lParam > long ptr 
{
	HDC hdc;
	PAINTSTRUCT ps;


	char InFileName[30] = "lena_bmp_512x512_new.bmp";	// .bmp 흑백이미지 받기
	char OutFileName[30] = "out.bmp";					// output bmp file 이미지
	const int width = 512;
	int i;

	FILE* infile; //file pointer
	FILE* outfile;
						//레나 파일

	float cnt[256] = { 0.0, };				//연산과 히스토그램 출력에 필요한 배열들
	int histo[256] = { 0, };
	float cdf[256] = { 0.0, };
	float cnt_h[256] = { 0.0, };
	

	BITMAPFILEHEADER HF;						//비트맵 헤더
	BITMAPINFOHEADER IF;
	RGBQUAD hRGB[256];
	
	infile = fopen(InFileName, "rb");			
	fread(&HF, sizeof(BITMAPFILEHEADER), 1, infile);		//레나 파일에서 헤더들 읽기
	fread(&IF, sizeof(BITMAPINFOHEADER), 1, infile);
	fread(hRGB, sizeof(RGBQUAD), 256, infile);

	int ImgSize = IF.biWidth * IF.biHeight;
	BYTE* lpImg = (BYTE*)malloc(sizeof(BYTE) * ImgSize);		//원본 영상의 밝기 값 정보가 저장될 배열
	BYTE* p = (BYTE*)malloc(sizeof(BYTE) * ImgSize);			//변환된 영상의 밝기 값 정보가  저장될 배열

	
	fread(lpImg, sizeof(char), ImgSize, infile);		//레나 파일에서 영상 밝기 값 정보 읽기
	fclose(infile);

	float max_histo = 0;

	static bool eql, bcs, ecs;		//어떤 연산을 수행할지 토큰


	switch (iMsg)
	{
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);	


		if (eql == TRUE) {			//평활화를 수행

			for (i = 0;i < ImgSize;i++) {		//원본 영상에 대한 밝기 값의 분포를 배열에 저장
				cnt[lpImg[i]]++;
			}

			cdf[0] = cnt[0];					//누적분포를 저장
			for (i = 1;i < 256; i++) {
				cdf[i] = cdf[i - 1] + cnt[i];
			}

			for (i = 0; i < ImgSize;i++) {		// 평활화 영상 변환 연산
				p[i] = static_cast<BYTE>((cdf[lpImg[i]] * 255 / cdf[255]));
			}
		}

		else if (bcs == TRUE) {		//Basic Contrast Stretching 연산
			BYTE max, min;
			max = min = lpImg[0];
			for (i = 1;i < ImgSize;i++) {	//최대, 최소값을 구하는 연산
				if (max < lpImg[i])max = lpImg[i];
				if (min > lpImg[i])min = lpImg[i];
			}

			for (i = 0; i < ImgSize;i++) {	//Stretcing
				p[i] = (lpImg[i] - min) * 255 / (max - min);
			}
		}

		else if (ecs == TRUE) {		//End-in Contrast Stretching 연산

			for (i = 0; i < ImgSize;i++) {		
				if (lpImg[i] < ECS_MIN)p[i] = 0;	//클리핑
				else if (lpImg[i] > ECS_MAX)p[i] = 255;
				else
				p[i] = (lpImg[i] - ECS_MIN) * 255 / (ECS_MAX - ECS_MIN);	//연산
			}
		}
		else(p = lpImg);	//맨 처음의 경우 영상처리 연산 없이 적용

		//여기부터 히스토그램 출력

		for (i = 0;i < ImgSize;i++) {	//변환된 영상이 저장된 배열 p 에 대해서 히스토그램을 만듬
			cnt_h[p[i]]++;
		}

		max_histo = cnt_h[0];			//히스토그램의 최대값을 찾음
		for (int i = 0; i < 256;i++) {
			if (cnt_h[i] > max_histo)max_histo = cnt_h[i];
		}

		for (int i = 0; i < 256;i++) {		//해당 최대값이 100이 되도록 크기 조정
			histo[i] = static_cast<int>(cnt_h[i] * 100 / max_histo);
		}


		MoveToEx(hdc, 20, 20, NULL);		//기본 히스토그램 틀
		LineTo(hdc, 20, 120);
		LineTo(hdc, 275, 120);
		LineTo(hdc, 275, 20);

		for (i = 0;i < 256;i++) {			//(20,120, 에서 시작해서 한 픽셀씩 움직이며 선을 그림
			MoveToEx(hdc, 20 + i, 120, NULL);
			LineTo(hdc, 20 + i, 120 - histo[i]);
		}

		EndPaint(hwnd, &ps);	//그림 종료

		outfile = fopen(OutFileName, "wb");			//파일 출력

		fwrite(&HF, sizeof(char), sizeof(BITMAPFILEHEADER), outfile);
		fwrite(&IF, sizeof(char), sizeof(BITMAPINFOHEADER), outfile);
		fwrite(hRGB, sizeof(RGBQUAD), 256, outfile);
		fwrite(p, sizeof(char), IF.biSizeImage, outfile);

		fclose(outfile);

		// 출력받을 파일은 output.bmp로 하나로 통일했고 연산을 처리할때마다 사진 파일의 내용이 바뀜


		break;

	case WM_COMMAND:		//메뉴 커맨드 처리
		switch (LOWORD(wParam)) {
			

		case ID_IMG_EQL:	//평활화 선택
			eql = TRUE; bcs = FALSE; ecs = FALSE;
			break;

		case ID_IMG_BCS:	//BCS 선택
			bcs = TRUE; eql = FALSE; ecs = FALSE;
			break;

		case ID_IMG_ECS:	//ECS 선택
			ecs = TRUE;eql = FALSE; bcs = FALSE;
			break;
		}

		InvalidateRgn(hwnd, NULL, TRUE);
		break;
		

	case WM_CREATE:
		
		break;	

	case WM_DESTROY:

		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hwnd, iMsg, wParam, lParam);

}

