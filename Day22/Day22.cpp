// Day22.cpp: 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Day22.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
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

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DAY22, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 응용 프로그램 초기화를 수행합니다.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DAY22));

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

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DAY22));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_DAY22);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
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
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      0, 0, 1024, 768, nullptr, nullptr, hInstance, nullptr);

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
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//

// 박스가 선택되어있는 녀석에 따라 사각형 움직이고 그 안에 사각형도 집어넣으려함
// enum 자주 사용하자 이거 활용할 수록 코드가 더 짧아짐
enum BOX {
	BOX_FIRST,
	BOX_SECOND,
	BOX_END
};

BOX currentBox;
RECT m_rc[BOX_END], m_rcIn;
bool isCollision;
POINT ptMouse = { 0, 0 };
bool isTouched;

enum DIRECTION {
	DIRECTION_NULL,
	DIRECTION_LT,
	DIRECTION_RT,
	DIRECTION_LB,
	DIRECTION_RB
};

bool isCheck;
DIRECTION direction;

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
	case WM_CREATE:
		// 여기서 초기화
		currentBox = BOX_SECOND;		// 조그만한 사각형 누가 갖고 있는지 
		m_rc[BOX_FIRST] = RectMakeCenter(
			1024 / 2 - 100, 768 / 2, 100, 100);	// 화면에 중심에 가도록

		m_rc[BOX_SECOND] = RectMakeCenter(
			1024 / 2 + 100, 768 / 2, 100, 100);

		m_rcIn = RectMakeCenter(
			1024 / 2 + 100, 768 / 2, 50, 50);
		isCollision = false;
		isTouched = false;
		isCheck = false;
		direction = DIRECTION_NULL;
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다.

		for (int i = 0; i < BOX_END; i++) {
			Rectangle(hdc,
				m_rc[i].left,
				m_rc[i].top,
				m_rc[i].right,
				m_rc[i].bottom);
		}

		Rectangle(hdc,
			m_rcIn.left,
			m_rcIn.top,
			m_rcIn.right,
			m_rcIn.bottom);

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_KEYDOWN:
		//// 충돌 계산 방법 1 저번 방법
		//if (m_rc[0].right >= m_rc[1].left &&
		//	m_rc[0].left <= m_rc[1].right &&
		//	m_rc[0].bottom >= m_rc[1].top &&
		//	m_rc[0].top <= m_rc[1].bottom) {

		//}

		// 충돌 계산 방법 2
		// 함수 있음 IntersectRect()
		// -> 겹치면 충돌 판정

		// 커다란 사각형 충돌
		RECT temp;	// 충돌된 구역
		if (IntersectRect(&temp, &m_rc[0], &m_rc[1])) {
			// 충돌하면 true, 아니면 false 반환

			// 계속 충돌했는지 처음 충돌인지
			if (!isCollision) {
				// 처음 충돌인 경우
				isCollision = true;
				switch (currentBox)
				{
				case BOX_FIRST:
					currentBox = BOX_SECOND;
					break;
				case BOX_SECOND:
					currentBox = BOX_FIRST;
					break;
				}
				m_rcIn = RectMakeCenter
				(m_rc[currentBox].left
					+ (m_rc[currentBox].right - m_rc[currentBox].left) / 2,
					m_rc[currentBox].top
					+ (m_rc[currentBox].bottom - m_rc[currentBox].top) / 2,
					50, 50);
			}
			else
				isCollision = false;
		}

		// 큰 사각형과 작은 사각형
		switch (wParam) {
		case VK_LEFT:
			m_rc[currentBox].left -= 3.0f;
			m_rc[currentBox].right -= 3.0f;
			if (m_rc[currentBox].right < m_rcIn.right) {
				m_rcIn.left -= 3.0f;
				m_rcIn.right -= 3.0f;
			}
			break;
		case VK_RIGHT:
			m_rc[currentBox].left += 3.0f;
			m_rc[currentBox].right += 3.0f;
			if (m_rc[currentBox].left > m_rcIn.left) {
				m_rcIn.left += 3.0f;
				m_rcIn.right += 3.0f;
			}
			break;
		}

		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_MOUSEMOVE:
		ptMouse.x = LOWORD(lParam);
		ptMouse.y = HIWORD(lParam);
		// 점과 사각형 충돌 계산 함수 있음
		// PtInRect(rect, point)
		// 여기서 계산해도 됨 WM_LBUTTONDOWN 에서 해도 되고 순서 차이
		if (PtInRect(&m_rc[currentBox], ptMouse)) {
			if (isTouched == true) {
				m_rc[currentBox] =
					RectMakeCenter(ptMouse.x, ptMouse.y, 
						m_rc[currentBox].right - m_rc[currentBox].left, 
						m_rc[currentBox].bottom - m_rc[currentBox].top);
			}
		}
		// 실질적으로 움직이는 건 여기서
		if (isCheck == true) {
			switch (direction)
			{
			case DIRECTION_LT:
				m_rc[currentBox].left = ptMouse.x;
				m_rc[currentBox].top = ptMouse.y;
				break;
			case DIRECTION_RT:
				m_rc[currentBox].right = ptMouse.x;
				m_rc[currentBox].top = ptMouse.y;
				break;
			case DIRECTION_LB:
				m_rc[currentBox].left = ptMouse.x;
				m_rc[currentBox].bottom = ptMouse.y;
				break;
			case DIRECTION_RB:
				m_rc[currentBox].right = ptMouse.x;
				m_rc[currentBox].bottom = ptMouse.y;
				break;
			}
		}

		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_LBUTTONDOWN:
		// 점과 사각형 충돌 계산
		/*if (m_rc[currentBox].left <= ptMouse.x &&
			ptMouse.x <= m_rc[currentBox].right &&
			m_rc[currentBox].top <= ptMouse.y &&
			ptMouse.y <= m_rc[currentBox].bottom) {

		}*/

		isTouched = true;
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_LBUTTONUP:
		isTouched = false;
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_RBUTTONDOWN:
		if (PtInRect(&m_rc[currentBox], ptMouse)) {
			isCheck = true;
			// 중심점 좌표 구해야함
			int centerX = m_rc[currentBox].left +
				(m_rc[currentBox].right - m_rc[currentBox].left) / 2;
			int centerY = m_rc[currentBox].top +
				(m_rc[currentBox].bottom - m_rc[currentBox].top) / 2;
			
			// 마우스 위치가 왼쪽 위
			if (ptMouse.x < centerX && ptMouse.y < centerY) {
				direction = DIRECTION_LT;
			}
			// 마우스 위치가 오른쪽 위
			if (ptMouse.x > centerX && ptMouse.y < centerY) {
				direction = DIRECTION_RT;
			}
			// 마우스 위치가 왼쪽 아래
			if (ptMouse.x < centerX && ptMouse.y > centerY) {
				direction = DIRECTION_LB;
			}
			// 마우스 위치가 오른쪽 아래
			if (ptMouse.x > centerX && ptMouse.y > centerY) {
				direction = DIRECTION_RB;
			}
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_RBUTTONUP:
		isCheck = false;

		// 뒤집엇을때 충돌 처리 안되는 이유
		// 기본적 충돌 공식으로 계산할때 너비값이 -가 되버림
		// 높이값도 마찬가지 
		// 여기서 처리해줘야함

		// left 는 right보다 작아야되는데 뒤집힌 경우니 swap
		if (m_rc[currentBox].left > m_rc[currentBox].right) {
			int temp = m_rc[currentBox].left;
			m_rc[currentBox].left = m_rc[currentBox].right;
			m_rc[currentBox].right = temp;
		}
		// top bottom도 마찬가지로
		if (m_rc[currentBox].top > m_rc[currentBox].bottom) {
			int temp = m_rc[currentBox].top;
			m_rc[currentBox].top = m_rc[currentBox].bottom;
			m_rc[currentBox].bottom = temp;
		}
		

		InvalidateRect(hWnd, NULL, TRUE);
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

/*
	과제
	한줄 조사
	디자인 패턴 (싱글톤 패턴)
	게임에서 사용하는 디자인 패턴 24가지 정도임 그 중 자주 사용하는게
	싱글톤 패턴
*/