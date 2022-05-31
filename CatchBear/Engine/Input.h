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

	NUM1 = '1',
	NUM2 = '2',
	NUM3 = '3',
	NUM4 = '4',		// ������ â ���� ġƮŰ
	NUM5 = '5',		// ��� ����� ȿ�� �����ϴ� ġƮŰ
	NUM9 = '9',		// �ٽ��ϱ� �׽�Ʈ ��ư

	NUM0 = '0',

	// ������ ġƮŰ
	Z = 'Z',	// SpeedUp
	X = 'X',	// Teleport
	C = 'C',	// Shield
	V = 'V',	// SpeedDown
	B = 'B',	// Blind
	N = 'N',	// DebuffOff
	M = 'M',	// Stun

	F = 'F',	// ���ǵ� �������

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


	// ������ ���� ��
	bool GetButton(KEY_TYPE key) { return GetState(key) == KEY_STATE::PRESS; }

	// �� ó�� ������ ��
	bool GetButtonDown(KEY_TYPE key) { return GetState(key) == KEY_STATE::DOWN; }

	// �� ó�� ������ ���� ��
	bool GetButtonUp(KEY_TYPE key) { return GetState(key) == KEY_STATE::UP; }


	const POINT& GetMousePos() { return _mousePos; }

private:
	inline KEY_STATE GetState(KEY_TYPE key) { return _states[static_cast<uint8>(key)]; }

private:
	HWND _hwnd;

	// �� Ű�� �ش��ϴ� ���� ����
	vector<KEY_STATE> _states;

	POINT _mousePos = {};
};

