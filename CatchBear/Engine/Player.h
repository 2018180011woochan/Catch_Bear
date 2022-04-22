#pragma once
#include "MonoBehaviour.h"

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
		DEBUFF_OFF, STUN, ITEM_END
	};

public:
	Player();
	virtual ~Player();

	virtual void Update() override;
	virtual void LateUpdate() override;

public:
	void AddPlayerItem(shared_ptr<GameObject> item);
	void AddPlayerItem(Player::ITEM itemType);

	void SetCurItem(Player::ITEM curItem, bool value) { _curPlayerItem[curItem] = value; }	// �� �Լ� ����ؼ� �÷��̾�� ������ ȿ�� �ο�
	void SetPlayerSpeed(float speed) { _speed = speed; }
	void SetPlayerStunned(bool value) { _bStunned = value; }
	void AddPlayerScore(int score) { _iScore += score; }

	bool GetCurItem(Player::ITEM curItem) { return _curPlayerItem[curItem]; }
	const float GetPlayerOriginalSpeed() { return _originalSpeed; }
	const bool GetPlayerStunned() { return _bStunned; }
	const int GetPlayerScore() { return _iScore; }
	
private:
	void KeyCheck();
	void Move();
	void KeyCheck_Item();
	void UseItem(int itemNum);
	void ApplyItemEffect();	
	void DeletePlayerItem(int itemIndex);

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
	float	_rotSpeed = 2.f;

	float	_fTeleportDis = 20.f;	// �����̵� �ϴ� �Ÿ�
	bool	_bUseShield = false;
	bool	_bStunned = false;

	float	_fShieldTime = 0.f;
	float	_fBlindTime = 0.f;
	float	_fStunTime = 0.f;
	float	_fScoreTime = 0.f;

	int		_iScore = 0;

	shared_ptr<GameObject>		_player = make_shared<GameObject>();
	shared_ptr<GameObject>		_camera = make_shared<GameObject>();
	shared_ptr<CameraScript>	_cameraScript = make_shared<CameraScript>();

	array<bool, Player::ITEM::ITEM_END> _curPlayerItem;	// �÷��̾ �������or���ϴ����� ������ Ȯ���ϱ� ���� �迭
	vector<shared_ptr<GameObject>>	_playerItemVec;	// �÷��̾ ������ �ִ� ������ �־�δ� ����(�ִ� 3��)
	array<bool, 3>	_playerItemArr;
	

private:
	STATE	_curState = STATE::END;
	STATE	_preState = STATE::END;

public:
	class PlayerState* _state;
};

