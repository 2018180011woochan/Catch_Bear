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

	void SetCurItem(Player::ITEM curItem, bool value) { _curPlayerItem[curItem] = value; }	// 이 함수 사용해서 플레이어에게 아이템 효과 부여
	void SetPlayerSpeed(float speed) { _speed = speed; }
	void SetPlayerStunned(bool value) { _bStunned = value; }
	void SetTagger(bool value) { _bTagger = value; }
	void AddPlayerScore(int score) { _iScore += score; }

	bool GetCurItem(Player::ITEM curItem) { return _curPlayerItem[curItem]; }
	const float GetPlayerOriginalSpeed() { return _originalSpeed; }
	const bool GetPlayerStunned() { return _bStunned; }
	const bool GetTagger() { return _bTagger; }
	const int GetPlayerScore() { return _iScore; }
	
private:
	void KeyCheck();
	void Move();
	void KeyCheck_Item();
	void UseItem(int itemNum);
	void ApplyItemEffect();	
	void CreateParticle();
	void RemoveParticle();

private:
	// 아이템 사용하는 함수
	void Item_SpeedUp();
	void Item_Teleport();
	void Item_Shield();
	void Item_SpeedDown();
	void Item_Blind();
	void Item_DebuffOff();
	void Item_Stun();

	// 디버프 아이템 효과 겪는 함수
	void SlowDown();
	void Blinded();
	void Stunned();

private:
	float	_speed = 10.f;
	float	_originalSpeed = 10.f;
	float	_rotSpeed = 2.f;

	float	_fTeleportDis = 20.f;	// 순간이동 하는 거리
	bool	_bUseShield = false;
	bool	_bStunned = false;

	float	_fShieldTime = 0.f;
	float	_fBlindTime = 0.f;
	float	_fStunTime = 0.f;

	bool	_bTagger = false;	// 술래인지 아닌지 판단
	int		_iScore = 0;

	shared_ptr<GameObject>		_player = make_shared<GameObject>();
	shared_ptr<GameObject>		_camera = make_shared<GameObject>();
	shared_ptr<CameraScript>	_cameraScript = make_shared<CameraScript>();

	array<bool, Player::ITEM::ITEM_END> _curPlayerItem;	// 플레이어가 사용중인or당하는중인 아이템 확인하기 위한 배열
	vector<shared_ptr<GameObject>>	_playerItemVec;	// 플레이어가 가지고 있는 아이템 넣어두는 벡터(최대 3개)

private:
	STATE	_curState = STATE::END;
	STATE	_preState = STATE::END;

public:
	class PlayerState* _state;
};

