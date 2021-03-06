#pragma once

enum class KEY_TYPE
{
	UP = VK_UP,
	DOWN = VK_DOWN,
	LEFT = VK_LEFT,
	RIGHT = VK_RIGHT,

	W = 'W',
	A = 'A',
	S = 'S',
	D = 'D',

	Q = 'Q',
	E = 'E',
	//Z = 'Z',
	//C = 'C',

	TEST_KEY = 'T',
	P = 'P',
	I = 'I',
	J = 'J',	// reset testkey
	O = 'O',

	NUM1 = '1',
	NUM2 = '2',
	NUM3 = '3',
	NUM4 = '4',		// 아이템 창 비우는 치트키
	NUM5 = '5',		// 모든 디버프 효과 해제하는 치트키
	NUM9 = '9',		// 다시하기 테스트 버튼

	NUM0 = '0',

	// 아이템 치트키
	Z = 'Z',	// SpeedUp
	X = 'X',	// Teleport
	C = 'C',	// Shield
	V = 'V',	// SpeedDown
	B = 'B',	// Blind
	N = 'N',	// DebuffOff
	M = 'M',	// Stun

	F = 'F',	// 스피드 원래대로

	LBUTTON = VK_LBUTTON,
	RBUTTON = VK_RBUTTON,

	ENTER = VK_RETURN,

	ESC = VK_ESCAPE,
	SPACE = VK_SPACE,
	CTRL = VK_CONTROL
};

enum class KEY_STATE
{
	NONE,
	PRESS,
	DOWN,
	UP,
	END
};

enum
{
	// [0]  ~ [255] Index
	KEY_TYPE_COUNT = static_cast<int32>(UINT8_MAX + 1),	

	KEY_STATE_COUNT = static_cast<int32>(KEY_STATE::END),
};

class Input
{
	DECLARE_SINGLE(Input);

public:
	void Init(HWND hwnd);
	void Update();


	// 누르고 있을 때
	bool GetButton(KEY_TYPE key) { return GetState(key) == KEY_STATE::PRESS; }

	// 맨 처음 눌렀을 때
	bool GetButtonDown(KEY_TYPE key) { return GetState(key) == KEY_STATE::DOWN; }

	// 맨 처음 눌렀다 뗐을 때
	bool GetButtonUp(KEY_TYPE key) { return GetState(key) == KEY_STATE::UP; }


	const POINT& GetMousePos() { return _mousePos; }

private:
	inline KEY_STATE GetState(KEY_TYPE key) { return _states[static_cast<uint8>(key)]; }

private:
	HWND _hwnd;

	// 각 키에 해당하는 상태 관리
	vector<KEY_STATE> _states;

	POINT _mousePos = {};
};

