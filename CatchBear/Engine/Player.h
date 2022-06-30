#pragma once
#include "MonoBehaviour.h"
#include "Item.h"

class CameraScript;

class Player : public MonoBehaviour, public enable_shared_from_this<Player>
{
public:
	//enum STATE
	//{
	//	IDLE, WALK, DASH, JUMP, ATTACK, END
	//};

public:
	enum ITEM
	{
		SPEED_UP, TELEPORT, SHIELD, SPEED_DOWN, BLIND,
		DEBUFF_OFF, STUN, ITEM_END, NONE,
	};

	//��ֹ� �غ� ������ ���� ����
	enum DIR
	{
		DIR_LEFT, DIR_RIGHT, DIR_END,
	};

public:
	Player();
	virtual ~Player();

	virtual void Update() override;
	virtual void LateUpdate() override;

public:
	void AddPlayerItem(Item::ITEM_EFFECT itemEffect);
	void Reset();	// �ٽ��ϱ� ���, �÷��̾� ���� �ʱ�ȭ

	void SetCurItem(Player::ITEM curItem, bool value) { _curPlayerItem[curItem] = value; }	// �� �Լ� ����ؼ� �÷��̾�� ������ ȿ�� �ο�
	void SetPlayerSpeed(float speed) { _speed = speed; }
	void SetPlayerStunned(bool value) { _bStunned = value; }
	void AddPlayerScore(int score) { _iScore += score; }
	void SetPlayerScore(int score) { _iScore = score; }
	void SetSheildTime(float time) { _fShieldTime = time; }

	bool GetCurItem(Player::ITEM curItem) { return _curPlayerItem[curItem]; }
	const float GetPlayerSpeed() { return _speed; }
	const float GetPlayerOriginalSpeed() { return _originalSpeed; }
	const float GetPlayerDashSpeed() { return _dashSpeed; }
	const float GetPlayerSlowSpeed() { return _slowSpeed; }
	const bool GetPlayerStunned() { return _bStunned; }
	const int GetPlayerScore() { return _iScore; }
	const int GetItemCount() { return _iItemCnt; }
	
private:
	void KeyCheck();
	void Move();
	void KeyCheck_Item();
	void UseItem(int itemNum);
	void ApplyItemEffect();	
	void DeletePlayerItem(int itemIndex);
	void ClearDebuff();
	bool CheckShield();
	bool CheckDebuff(Item::ITEM_EFFECT itemEffect);
	void KeyCheck_Cheat();

private:
	// ������ ����ϴ�(���� ���濡��) �Լ�
	void Item_SpeedUp();
	void Item_Teleport();
	void Item_Shield();
	void Item_SpeedDown();
	void Item_Blind();
	void Item_DebuffOff();
	void Item_Stun();

	// ����� ������ ȿ�� �޴� �Լ�
	void SlowDown();
	void Blinded();
	void Stunned();

private:
	float	_speed = 10.f;
	float	_originalSpeed = 10.f;
	float	_dashSpeed = 15.f;
	float	_slowSpeed = 5.f;
	float	_rotSpeed = 2.f;

	float	_fTeleportDis = 20.f;	// �����̵� �ϴ� �Ÿ�
	bool	_bStunned = false;

	float	_fShieldTime = 0.f;
	float	_fBlindTime = 0.f;

	int		_iScore = 0;
	int		_iItemCnt = 0;

	shared_ptr<GameObject>		_player = make_shared<GameObject>();
	shared_ptr<GameObject>		_camera = make_shared<GameObject>();
	shared_ptr<CameraScript>	_cameraScript = make_shared<CameraScript>();

	array<bool, Player::ITEM::ITEM_END> _curPlayerItem;	// �÷��̾ �������or���ϴ����� ������ Ȯ���ϱ� ���� �迭
	array<Item::ITEM_EFFECT, 3> _playerItemArr;	// �÷��̾ ������ �ִ� �������� ȿ���� �־�δ� ���(�ִ� 3��)

private:
	STATE	_curStatePlayer = STATE::END;
	STATE	_preState = STATE::END;

public:
	class PlayerState* _state;

	float	_testTime = 0.f;
	bool	_isFont = false;
	wstring _wstrText = L"";

	DIR		_dir = DIR::DIR_END;
};

