// Day23.cpp: 응용 프로그램의 진입점을 정의합니다.
//

/*
	Manager 관리만을 하는 녀석이라고 보면 됨
	ex-ImageManager 사운드만 관리하는 녀석
*/

#include "stdafx.h"
#include "Day23.h"
#include "MainGame6.h"		// 추가

#define MAX_LOADSTRING 100

// g global 약자
// 전역 변수:
HINSTANCE g_hInstance;		// 이름변경         // 현재 인스턴스입니다.
HWND g_hWnd;				// 추가
POINT g_ptMouse = { 0,0 };	// 추가

MainGame6* g_mg;			// 추가

WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

												// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 여기에 코드를 입력합니다.
	g_mg = new MainGame6;			// 추가

	g_hInstance = hInstance;		// 추가

	// 전역 문자열을 초기화합니다.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_DAY23, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 응용 프로그램 초기화를 수행합니다.
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	// 추가
	if (FAILED(g_mg->Init())) {
		return false;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DAY23));

	MSG msg;

	// 기본 메시지 루프입니다.
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	g_mg->Release();
	delete g_mg;

	return (int)msg.wParam;	// 이 때 창 종료됨
}



//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	// WinClass(기본) 가 있고 WinClassEX가 있음 확장임 
	// W는 멀티바이트 확장인거

	WNDCLASSEX wcex;		// W 제거함

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DAY23));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;		// NULL 설정
	wcex.lpszClassName = WINNAME;	// WINNAME으로 변경
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);		// W 제거
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	// h_Inst 지움 위에서 받아서

	// g_hWnd로 변경
	// CreateWindow로 변경 후 파라미터 변경 해줌
	g_hWnd = CreateWindow(
		WINNAME, 
		WINNAME, 
		WS_OVERLAPPEDWINDOW,
		WINSTARTX, 
		WINSTARTY, 
		WINSIZEX, 
		WINSIZEY, 
		nullptr, 
		nullptr, 
		hInstance, 
		nullptr);

	if (!g_hWnd)
	{
		return FALSE;
	}

	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//

// 박스가 선택되어있는 녀석에 따라 사각형 움직이고 그 안에 사각형도 집어넣으려함

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 메뉴 선택을 구문 분석합니다.
		switch (wmId)
		{
		case IDM_ABOUT:
			// g_hInstance로 변경
			DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	default:
		// MainProc에서 돌리려고 하는데 
		// wParam 키보드 관련 파라미터, lParam 마우스 관련 파라미터
		return g_mg->MainProc(hWnd, message, wParam, lParam);
		break;
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

// 오늘 한건 한번 쫙 다시 해보자 새로 프로젝트 만들어서
// 익숙해질 필요도 있고 이해할 필요도 있음
// keyManager 사용법은 내일 해볼 꺼

/*
	과제1
	똥피하기 게임
	rect 떨어지는데
	좌우로 움직여서 피하는 게임
	점수 설정은 마음대로 ex) 회피 시간, 회피 횟수 점수를 얻게 
	rect와 부딪치면 마음대로 작업하기 (게임 끝나거나 창을 끄거나)

	과제2
	두더지 잡기
	API RECT 색상 or FILL RECT 검색하면 그안에 rect 바꾸는 기능 있음
	4 x 4 
	[] [] [] []
	[] [0] [] []	-> 단 중복 클릭 방지 
	[] [] [] []		(색상 나왓을때 클릭 할때마다 점수 올라가면 안됨)
	[] [] [] []		(한번만 오르고 다른 녀석 바뀔때까지 점수 올라가지 않게)
	랜덤한 녀석 색상 바뀌는데 그때 클릭하면 점수 올라가는거
	일정시간 지나면 다른 녀석 색상 바뀜

	과제3
	악어 이빨
	[] [] [] [] []
	[X] [] [] [] []
	내가 클릭하면 색상이 바뀜, 랜덤한 녀석은 섞은 이빨 그넘 클릭하면
	게임 종료하거나 다시시작하게

	과제4
	카드 짝맞추기, 같은 그림찾기 같은
	[] [] [] []
	[] [] [] []
	[] [] [] []
	[] [] [] []
	숫자, 색깔, 영어 마음대로
	클릭하면 카드 뒤집히고 다음 클릭시 다르면 다시 뒤집고
	같으면 유지 (유지는 계속 진행하더라도 보이게)

	과제5
	런게임 (장애물 화면 밖 오른쪽 -> 왼쪽방향으로 옴)
	점프 버튼 눌러서 피하는 식 (스페이스 바 누르면)
	점프 눌르면 올라갔다가 내려오는 식으로
*/ 

// 넥슨 코딩 문제
/*
	1번
	숫자 X보다 크거나 같은 수 중 가장 작은 2이 승수(2^n) NPOT 라고 할때
	주어진 숫자 N개의 NPOT를 후여 모두 XOR 한 값을 구하시오

	standard library 쓰지 않고 비트연산으로 해야함

	조건
	1 < X < 2^63
	1 <= N < 2^18

	입력 
	3
	5
	7
	출력
	4
	4 = 4 ^ 8 ^ 8

	2번
	계산 문제
	감염자 A
	배수 B
	시간 T
	A * B ^ T 구하기

	3번
	탱크 -> 목표지점까지 최단거리 찾기
	앞으로 이동할때 2초		F
	왼쪽 오른쪽 회전 1초	L, R 
	시간 얼마만큼 걸리는지

	4번 관통샷 문제
	원과 선끼리 어떻게 출동 계산하는지 문제
	총을 몇번에 쐈는데 선과 원이 충돌하는지 계산하는 문제

	// 보통 라이브러리 함수 사용하면 안됨
	// 수행시간 넘어가버림
	4번은 텍스트 만들어왔으니 읽어서 푸는문제
	인덱스 x좌표, y좌표 반지름 한줄 이렇게 되어 있음

	이거 시험 전에는 인턴시험이었는데 
	다 영어로 나왔었다고 함

	코드업
*/