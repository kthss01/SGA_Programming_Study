// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include "CommonMacroFunction.h"
#include "KeyManager.h"
#include "RandomFunction.h"

///////////////////////////////////////////////////////
// define 문 (윈도우 초기화 셋팅)
///////////////////////////////////////////////////////
#define WINNAME (LPTSTR)(TEXT("1801APIFrameWork"))
#define WINSTARTX 100
#define WINSTARTY 100
#define WINSIZEX 1024
#define WINSIZEY 768
#define WINSTYLE WS_CAPTION | WS_SYSMENU

///////////////////////////////////////////////////////
// macro 함수 (메인게임내의 릴리스 부분에서 사용)
///////////////////////////////////////////////////////
// 밑에 3개는 c++로 작업할 때 무조건 만들어야 하는 macro 함수라고 보면 됨
// 메모리 관리를 직접적으로 해줘야 하기 때문에
#define SAFE_DELETE(p) {if(p) {delete p; (p) = NULL;}}
#define SAFE_RELEASE(p) {if(p) {(p)->Release(); (p) = NULL;}}
#define SAFE_DELETE_ARRAY(p) {if(p) {delete[] (p); (p) = NULL;}}

// extern 원래 있던 전역변수 변수명 똑같이 써서 다른 cpp 에서 쓸수 있게 하는거
extern HINSTANCE g_hInstance;	
extern HWND g_hWnd;
extern POINT g_ptMouse;