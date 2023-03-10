#pragma once

enum class KEY
{
	UP, DOWN, LEFT, RIGHT, ALT, CTRL, SHIFT, SPACE, ENTER, ESC,
	A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
	LBUTTON, RBUTTON,

	COUNT
};

enum class KEY_STATE
{
	NONE, // 이전에도 눌리지 않았고, 현재도 눌리지 않은 상태
	TAP,  // 이전에 눌리지 않았고, 현재 눌린 상태
	HOLD, // 이전에 눌렸고, 현재도 눌린 상태
	AWAY, // 이전에 눌렸고, 현재 떼진 상태
};

struct KeyInfo
{
	bool	  m_isPressed; // 이전 프레임에 눌렸는지에 대한 여부
	KEY_STATE m_state;     // 현재 키의 상태
};

// 1. 프레임 동기화
//   - 동일 프레임 내에서 같은 키에 대해서, 동일한 이벤트를 가져가야 한다.
// 2. 키 입력 상태 처리
//    - Tap, Hold, Away
class CInputManager : public CSingleton<CInputManager>
{
	friend class CSingleton;

private:
	int     m_virtualKey[(int)KEY::COUNT]; // 가상 키 값은 KEY의 순서와 일치해야 한다.
	KeyInfo m_keyInfo[(int)KEY::COUNT];
	
	Vec2    m_cursor;

private:
	CInputManager();
	~CInputManager();

public:
	void Init();

	KEY_STATE GetKeyState(KEY key);
	const Vec2& GetCursor();

	void Update();
};
