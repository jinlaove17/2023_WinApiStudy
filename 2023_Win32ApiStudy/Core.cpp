#include "pch.h"
#include "Core.h"

#include "resource.h"

#include "TimeManager.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "AssetManager.h"
#include "CollisionManager.h"
#include "EventManager.h"
#include "UIManager.h"
#include "SoundManager.h"
#include "Camera.h"

#include "Object.h"

#include "Sound.h"

CCore::CCore() :
	m_hWnd(),
	m_resolution(),
	m_hDC(),
	m_hBitmap(),
	m_hMemDC(),
	m_hMenu()
{
}

CCore::~CCore()
{
	// GetDC 함수로 얻어온 DC는 ReleaseDC 함수로 해제한다.
	ReleaseDC(m_hWnd, m_hDC);

	// CreateCompatibleDC 함수로 얻어온 DC는 DeleteDC 함수로 해제한다.
	DeleteDC(m_hMemDC);
	DeleteObject(m_hBitmap);

	// 메뉴바 제거
	DestroyMenu(m_hMenu);
}

int CCore::Init(HWND hWnd, const SIZE& resolution)
{
	m_hWnd = hWnd;
	m_resolution = resolution;
	m_hDC = ::GetDC(m_hWnd);
	
	// 해상도에 맞게 윈도우 크기 조정
	RECT rect = { 0, 0, m_resolution.cx, m_resolution.cy };

	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
	SetWindowPos(m_hWnd, nullptr, 100, 100, rect.right - rect.left, rect.bottom - rect.top, 0);

	// 더블 버퍼링 용도의 비트맵과 DC를 만든다.
	m_hBitmap = CreateCompatibleBitmap(m_hDC, m_resolution.cx, m_resolution.cy);
	m_hMemDC = CreateCompatibleDC(m_hDC);

	// DC가 처음 만들어질 때, 1 픽셀짜리의 비트맵이 만들어진다.
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(m_hMemDC, m_hBitmap);

	// 1 픽셀짜리 비트맵은 사용하지 않을 것이므로, 곧바로 삭제한다.
	DeleteObject(hOldBitmap);

	// 툴 씬에서 사용할 메뉴바를 생성한다.
	m_hMenu = LoadMenu(nullptr, MAKEINTRESOURCEW(IDC_MY2023WIN32APISTUDY));

	// 매니저 초기화
	CTimeManager::GetInstance()->Init();
	CInputManager::GetInstance()->Init();
	CAssetManager::GetInstance()->Init();
	CSoundManager::GetInstance()->Init();
	CCamera::GetInstance()->Init();
	CSceneManager::GetInstance()->Init();

	// 배경음 설정
	CAssetManager::GetInstance()->LoadSound(L"Gateway.wav", L"BGM_1")->PlayToBGM(true);

	return S_OK;
}

HWND CCore::GetHwnd()
{
	return m_hWnd;
}

const SIZE& CCore::GetResolution()
{
	return m_resolution;
}

HDC CCore::GetDC()
{
	return m_hDC;
}

void CCore::ShowMenu()
{
	SetMenu(m_hWnd, m_hMenu);

	// 해상도에 맞게 윈도우 크기 조정
	RECT rect = { 0, 0, m_resolution.cx, m_resolution.cy };

	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, true);
}

void CCore::HideMenu()
{
	SetMenu(m_hWnd, nullptr);

	// 해상도에 맞게 윈도우 크기 조정
	RECT rect = { 0, 0, m_resolution.cx, m_resolution.cy };

	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
}

void CCore::PrepareRender()
{
	// 화면 클리어(검정)
	//GDIObject gdiObject(m_hMemDC, BRUSH_TYPE::BLACK);

	Rectangle(m_hMemDC, -1, -1, m_resolution.cx + 1, m_resolution.cy + 1);
}

void CCore::AdvanceFrame()
{
	// 매니저 업데이트
	CTimeManager::GetInstance()->Update();
	CInputManager::GetInstance()->Update();
	CCamera::GetInstance()->Update();
	CSceneManager::GetInstance()->Update();
	CCollisionManager::GetInstance()->Update();
	CUIManager::GetInstance()->Update();

	PrepareRender();

	CSceneManager::GetInstance()->Render(m_hMemDC);
	CCamera::GetInstance()->Render(m_hMemDC);

	// m_hMemDC에 그려진 내용을 m_hDC로 복사한다.
	BitBlt(m_hDC, 0, 0, m_resolution.cx, m_resolution.cy, m_hMemDC, 0, 0, SRCCOPY);

	// 이번 프레임에 추가된 이벤트를 처리한다.
	CEventManager::GetInstance()->Update();
}
