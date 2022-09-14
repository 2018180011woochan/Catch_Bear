#pragma once
#include "MonoBehaviour.h"
#include "Item.h"

class CameraScript;
class ShieldParticle;

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

public:
	Player();
	virtual ~Player();

	virtual void Update() override;
	virtual void LateUpdate() override;

public:
	void AddPlayerItem(Item::ITEM_EFFECT itemEffect);
	void Reset();	// 다시하기 기능, 플레이어 정보 초기화

	void SetCurItem(Player::ITEM curItem, bool value) { _curPlayerItem[curItem] = value; }	// 이 함수 사용해서 플레이어에게 아이템 효과 부여
	void SetPlayerSpeed(float speed) { _speed = speed; }
	void SetPlayerStunned(bool value) { _bStunned = value; }
	void AddPlayerScore(int score) { _iScore += score; }
	void SetPlayerScore(int score) { _iScore = score; }
	void SetSheildTime(float time) { _fShieldTime = time; }
	void SetTextureKey(const wstring& textureKey) { _textureKey = textureKey; }

	bool GetCurItem(Player::ITEM curItem) { return _curPlayerItem[curItem]; }
	const float GetPlayerSpeed() { return _speed; }
	const float GetPlayerOriginalSpeed() { return _originalSpeed; }
	const float GetPlayerDashSpeed() { return _dashSpeed; }
	const float GetPlayerSlowSpeed() { return _slowSpeed; }
	const bool GetPlayerStunned() { return _bStunned; }
	const int GetPlayerScore() { return _iScore; }
	const int GetItemCount() { return _iItemCnt; }
	const wstring& GetTextureKey() { return _textureKey; }
	
	void SetPlayerLight(shared_ptr<GameObject>& light) { _playerLight = light; }

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
	// 아이템 사용하는(내가 상대방에게) 함수
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

	// 아이템 이펙트
	void ShieldEffect();

private:
	float	_speed = 10.f;
	float	_originalSpeed = 10.f;
	float	_dashSpeed = 15.f;
	float	_slowSpeed = 5.f;
	float	_rotSpeed = 2.f;

	float	_fTeleportDis = 20.f;	// 순간이동 하는 거리
	bool	_bStunned = false;

	float	_fShieldTime = 0.f;
	float	_fBlindTime = 0.f;

	int		_iScore = 0;
	int		_iItemCnt = 0;

	wstring _textureKey;

	shared_ptr<GameObject>		_player = make_shared<GameObject>();
	shared_ptr<GameObject>		_camera = make_shared<GameObject>();
	shared_ptr<CameraScript>	_cameraScript = make_shared<CameraScript>();
	//shared_ptr<ShieldParticle>	_shieldParticle = make_shared<ShieldParticle>();
	shared_ptr<GameObject>		_shieldParticle = make_shared<GameObject>();

	array<bool, Player::ITEM::ITEM_END> _curPlayerItem;	// 플레이어가 사용중인or당하는중인 아이템 확인하기 위한 배열
	array<Item::ITEM_EFFECT, 3> _playerItemArr;	// 플레이어가 가지고 있는 아이템의 효과를 넣어두는 어레이(최대 3개)

private:
	STATE	_curStatePlayer = STATE::END;
	STATE	_preState = STATE::END;

	shared_ptr<GameObject>	_playerLight = make_shared<GameObject>();

public:
	class PlayerState* _state;

	float	_testTime = 0.f;
	bool	_isFont = false;
	wstring _wstrText = L"";
};

