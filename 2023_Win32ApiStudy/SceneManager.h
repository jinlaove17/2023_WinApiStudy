#pragma once

class CScene;

class CSceneManager : public CSingleton<CSceneManager>
{
	friend class CSingleton;
	friend class CEventManager;

private:
	CScene* m_scenes[(int)SCENE_TYPE::COUNT];
	CScene* m_currentScene;

private:
	CSceneManager();
	~CSceneManager();

	// 실제 씬을 교체하는 ChangeScene() 함수는 오로지 CEventManager만 호출할 수 있도록 만든다.
	void ChangeScene(SCENE_TYPE scene);

public:
	void Init();

	CScene* GetCurrentScene();

	void Update();

	void Render(HDC hDC);
};
