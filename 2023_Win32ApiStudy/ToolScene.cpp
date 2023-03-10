#include "pch.h"
#include "ToolScene.h"

#include "resource.h"

#include "Core.h"

#include "InputManager.h"
#include "SceneManager.h"
#include "CollisionManager.h"
#include "EventManager.h"
#include "Camera.h"

#include "AssetManager.h"

#include "Tile.h"
#include "PanelUI.h"
#include "ButtonUI.h"

CToolScene::CToolScene() :
	m_tileXCount(),
	m_tileYCount()
{
}

CToolScene::~CToolScene()
{
}

void CToolScene::ArrangeTiles(int tileXCount, int tileYCount)
{
	// 타일 생선 전, 기존에 배치된 타일들을 제거한다.
	DeleteGroupObject(GROUP_TYPE::TILE);

	m_tileXCount = tileXCount;
	m_tileYCount = tileYCount;

	// 타일 생성
	CTexture* texture = CAssetManager::GetInstance()->LoadTexture(L"TileSet.bmp", L"TileSet");

	for (int i = 0; i < m_tileYCount; ++i)
	{
		for (int j = 0; j < m_tileXCount; ++j)
		{
			CTile* tile = new CTile();

			tile->SetPosition(Vec2(TILE_SIZE * j, TILE_SIZE * i));
			tile->SetScale(Vec2(TILE_SIZE, TILE_SIZE));
			tile->SetTexture(texture);

			AddObject(GROUP_TYPE::TILE, tile);
		}
	}
}

void CToolScene::SaveData()
{
	wstring assetPath = CAssetManager::GetInstance()->GetAssetPath();
	OPENFILENAME ofn = {};
	wchar_t filePath[256] = {};

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = CCore::GetInstance()->GetHwnd();
	ofn.lpstrFile = filePath;
	ofn.nMaxFile = sizeof(filePath);
	ofn.lpstrFilter = L"all\0*.*\0tile\0*.tile\0";
	ofn.nFilterIndex = 0;
	ofn.lpstrFileTitle = nullptr;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = wstring(assetPath + L"Tile\\").c_str();
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// 저장 버튼을 누른 경우 true를 반환한다.
	if (GetSaveFileName(&ofn))
	{
		setlocale(LC_ALL, "");

		ofstream out(filePath, ios::binary);

		assert(out.is_open());

		// 데이터 저장
		const vector<CObject*>& tiles = GetGroupObject(GROUP_TYPE::TILE);

		out.write((const char*)&m_tileXCount, sizeof(int));
		out.write((const char*)&m_tileYCount, sizeof(int));

		for (int i = 0; i < tiles.size(); ++i)
		{
			CTile* tile = (CTile*)tiles[i];
			int tileIndex = tile->GetIndex();

			out.write((const char*)&tileIndex, sizeof(int));
		}
	}
}

void CToolScene::LoadData()
{
	wstring assetPath = CAssetManager::GetInstance()->GetAssetPath();
	OPENFILENAME ofn = {};
	wchar_t filePath[256] = {};

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = CCore::GetInstance()->GetHwnd();
	ofn.lpstrFile = filePath;
	ofn.nMaxFile = sizeof(filePath);
	ofn.lpstrFilter = L"all\0*.*\0tile\0*.tile\0";
	ofn.nFilterIndex = 0;
	ofn.lpstrFileTitle = nullptr;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = wstring(assetPath + L"Tile\\").c_str();
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// 열기 버튼을 누른 경우 true를 반환한다.
	if (GetOpenFileName(&ofn))
	{
		ifstream in(filePath, ios::binary);

		assert(in.is_open());

		// 데이터 로드
		in.read((char*)&m_tileXCount, sizeof(int));
		in.read((char*)&m_tileYCount, sizeof(int));

		// 읽어온 수만큼 타일을 배치한다.
		ArrangeTiles(m_tileXCount, m_tileYCount);

		// 배치한 타일의 인덱스를 읽어와 설정한다.
		const vector<CObject*>& tiles = GetGroupObject(GROUP_TYPE::TILE);

		for (int i = 0; i < tiles.size(); ++i)
		{
			CTile* tile = (CTile*)tiles[i];
			int tileIndex = 0;

			in.read((char*)&tileIndex, sizeof(int));
			tile->SetIndex(tileIndex);
		}
	}
}

void CToolScene::Enter()
{
	// 메뉴 바 표시
	CCore::GetInstance()->ShowMenu();

	// 카메라 포커싱 설정
	Vec2 resolution = CCore::GetInstance()->GetResolution();

	// 초기 위치는 해상도의 정중앙이다.
	CCamera::GetInstance()->SetFinalLookAt(resolution / 2.0f);
	CCamera::GetInstance()->SetTarget(nullptr);

	// 오브젝트 생성
	CUI* panelUI = new CPanelUI();

	panelUI->SetScale(Vec2(500.0f, 300.0f));
	panelUI->SetPosition(Vec2(resolution.m_x - panelUI->GetScale().m_x, 0.0f));

	AddObject(GROUP_TYPE::UI, panelUI);

	CButtonUI* buttonUI = new CButtonUI();

	buttonUI->SetScale(Vec2(100.0f, 40.0f));
	buttonUI->SetLocalPosition(Vec2(0.0f, 0.0f));
	buttonUI->SetEvent([]() { CEventManager::GetInstance()->ChangeScene(SCENE_TYPE::TITLE); });
	panelUI->AddChild(buttonUI);

	panelUI = panelUI->Clone();
	panelUI->SetScale(Vec2(500.0f, 300.0f));
	panelUI->SetPosition(Vec2(resolution.m_x - panelUI->GetScale().m_x, 400.0f));

	AddObject(GROUP_TYPE::UI, panelUI);
}

void CToolScene::Exit()
{
	// 메뉴 바 제거
	CCore::GetInstance()->HideMenu();

	for (int i = 0; i < (int)GROUP_TYPE::COUNT; ++i)
	{
		DeleteGroupObject((GROUP_TYPE)i);
	}

	CCollisionManager::GetInstance()->ResetCollisionGroup();
}

void CToolScene::Update()
{
	CScene::Update();

	if (KEY_TAP(KEY::LBUTTON))
	{
		Vec2 cursor = CCamera::GetInstance()->ScreenToWorld(CURSOR);
		int row = (int)cursor.m_y / TILE_SIZE;
		int col = (int)cursor.m_x / TILE_SIZE;

		// 타일이 아닌 곳을 클릭할 경우 예외처리한다.
		if ((cursor.m_x < 0.0f) || (col >= m_tileXCount) || (cursor.m_y < 0.0f) || (row >= m_tileYCount))
		{
			return;
		}

		const vector<CObject*>& tiles = GetGroupObject(GROUP_TYPE::TILE);
		CTile* tile = (CTile*)tiles[m_tileXCount * row + col];

		tile->SetIndex(tile->GetIndex() + 1);
	}

	if (KEY_TAP(KEY::O))
	{
		SaveData();
	}

	if (KEY_TAP(KEY::P))
	{
		LoadData();
	}
}



// 타일 관련 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK TileCountProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			// Dialog 안에 있는 int 값을 가져 올 수 있다.
			int xCount = GetDlgItemInt(hDlg, IDC_EDIT1, nullptr, false);
			int yCount = GetDlgItemInt(hDlg, IDC_EDIT2, nullptr, false);

			if ((xCount > 0) && (yCount > 0))
			{
				CToolScene* currentScene = dynamic_cast<CToolScene*>(CSceneManager::GetInstance()->GetCurrentScene());

				assert(currentScene != nullptr);

				currentScene->ArrangeTiles(xCount, yCount);
			}

			EndDialog(hDlg, LOWORD(wParam));

			return (INT_PTR)TRUE;
		}
		else if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));

			return (INT_PTR)TRUE;
		}
		break;
	}

	return (INT_PTR)FALSE;
}
