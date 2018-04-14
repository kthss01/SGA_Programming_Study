#include "stdafx.h"
#include "GameNode.h"

GameNode::GameNode() {

}

GameNode::~GameNode() {}

Image* GameNode::m_backBuffer = NULL;

Image* GameNode::SetBackBuffer() {
	// 이 문장이 싱글톤
	if (m_backBuffer == NULL) {
		m_backBuffer = new Image;
		m_backBuffer->Init(WINSIZEX, WINSIZEY);
	}

	return m_backBuffer;
}

HRESULT GameNode::Init() {
	SetBackBuffer();
	_hdc = GetDC(g_hWnd);
	//SetTimer(g_hWnd, 1, 10, NULL);

	// strlen은 거기 안에 들어있는 문자 길이만큼 나옴
	ZeroMemory(str, sizeof(str));

	FRAME->Init();
	SUBWIN;

	return S_OK;
}

void GameNode::Release() {
	//KillTimer(g_hWnd, 1);
	m_backBuffer->Release();
	SAFE_DELETE(m_backBuffer);

	INPUT->ReleaseSingleton();

	SCENE->Release();
	SCENE->ReleaseSingleton();

	IMAGE->Release();
	IMAGE->ReleaseSingleton();

	FRAME->Release();
	FRAME->ReleaseSingleton();

	SOUND->Release();
	SOUND->ReleaseSingleton();

	TEXTDATA->ReleaseSingleton();

	INIDATA->ReleaseSingleton();

	SUBWIN->Release();
	SUBWIN->ReleaseSingleton();
}

void GameNode::Update() {
	InvalidateRect(g_hWnd, NULL, FALSE);

	//SUBWIN->Update();
}

void GameNode::Render() {}

void GameNode::Render(HDC hdc)
{
}

LRESULT GameNode::MainProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	PAINTSTRUCT ps;

	switch (msg) {
	case WM_TIMER:
		this->Update();
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		this->Render();
		EndPaint(hWnd, &ps);
		break;
	case WM_MOUSEMOVE:
		SUBWIN->SetIsActive(false);
		g_ptMouse.x = LOWORD(lParam);
		g_ptMouse.y = HIWORD(lParam);
		break;
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		}
		break;
	case WM_CHAR:
	{
		int len = strlen(str);
		str[len] = (char)wParam;
		str[len + 1] = 0;
	}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return (DefWindowProc(hWnd, msg, wParam, lParam));
}