#include "pch.h"
#include "Player.h"
#include "Transform.h"
#include "Input.h"
#include "Timer.h"
#include "GameObject.h"
#include "Component.h"
#include "Scene.h"
#include "SceneManager.h"
#include "CameraScript.h"
#include "AnimationController.h"
#include "AnimationTrack.h"
#include "PlayerState.h"
#include "IdleState.h"
#include "MoveState.h"
#include "AttackState.h"
#include "DashState.h"
#include "DashRestState.h"
#include "ServerPacketHandler.h"
#include "ServerSession.h"
#include "SlowRestState.h"
#include "SlowState.h"
#include "StunState.h"

#include "ItemManager.h"
#include "Item.h"
#include "Light.h"
#include "ItemSlotManager.h"
#include "TagMark.h"
#include "MeshRenderer.h"
#include "Resources.h"
#include "Engine.h"
#include "SoundManager.h"
#include "ShieldParticleManager.h"

Protocol::C_MOVE pkt;
Protocol::C_STATE StatePkt;

Player::Player()
{
	// 서버에서 컨트롤하는 플레이어는 _state 갖고있으면 안됨
	_state = new IdleState();
	_curPlayerItem = { false, };
	_playerItemArr = { Item::ITEM_EFFECT::NONE, Item::ITEM_EFFECT::NONE, Item::ITEM_EFFECT::NONE };
}

Player::~Player()
{
}

void Player::Update()
{
	//if (!_isInit)
	//{
	//	_isInit = true;
	//	GET_SINGLE(ItemSlotManager)->ResetItemSlot(1);
	//	GET_SINGLE(ItemSlotManager)->ResetItemSlot(2);
	//	GET_SINGLE(ItemSlotManager)->ResetItemSlot(3);
	//}
	//cout << "플레이어 " << _player->GetPlayerID() << ": " << /*static_pointer_cast<Player>(_player->GetScript(0))->*/_iScore << endl;
	ApplyItemEffect();
}

void Player::LateUpdate()
{
	// 서버에서 컨트롤하는 플레이어는 서버에서 위치값도 받아오니까 필요없을듯
	KeyCheck();

	////////////////////////////////////////////////////////////////////
	// 이 부분은 직접 플레이하고 있는 플레이어에만 적용되야 함!!
	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
	_player = scene->GetPlayer(mysession->GetPlayerID());

	PlayerState* state = _state->Update(*_player, _curStatePlayer);
	_player->_curState = _curStatePlayer;

	//cout << _player->GetPlayerID() << " : " << _curStatePlayer << endl;

	if (state != NULL)
	{
		_state->End(*_player);
		delete _state;
		_state = state;
		_state->Enter(*_player);
	}

	// 애니메이션 재생하는 부분 -> 모두 적용되야 함
	GetAnimationController()->AdvanceTime(DELTA_TIME);
	GetTransform()->UpdateTransform(NULL);
	GetAnimationController()->SetWorldMatrix();

	Vec3 trans = GetTransform()->GetLocalPosition();
	//printf("%f, %f, %f\n", trans.x, trans.y, trans.z);

	if (_player->_curState == WALK && GetAnimationController()->GetCurrentFrame() >= 2)
		GET_SINGLE(SoundManager)->PlaySound(L"click.mp3", SoundManager::MOVE);

}

void Player::AddPlayerItem(Item::ITEM_EFFECT itemEffect)
{
	// 플레이어가 갖고있을 수 있는 최대 아이템 수는 3개
	// 아이템을 3개 지니고 있으면 아이템 획득해도 무시
	if (_iItemCnt < 3)
	{
		for (int i = 0; i < 3; ++i)
		{
			if (_playerItemArr[i] == Item::ITEM_EFFECT::NONE)
			{
				_playerItemArr[i] = itemEffect;
				printf("%d 획득\n", itemEffect);

				// 해당 아이템을 확인해서 slot에 맞는 텍스처를 설정해야 한다.
				GET_SINGLE(ItemSlotManager)->AddItem(itemEffect);

				_iItemCnt++;	// 아이템 얻으면 올라가고 사용하면 내려감

				break;
			}
		}
	}
}

void Player::Reset()
{
	// 플레이어 보유 아이템
	_playerItemArr[0] = Item::ITEM_EFFECT::NONE;
	_playerItemArr[1] = Item::ITEM_EFFECT::NONE;
	_playerItemArr[2] = Item::ITEM_EFFECT::NONE;

	GET_SINGLE(ItemSlotManager)->ResetItemSlot(1);
	GET_SINGLE(ItemSlotManager)->ResetItemSlot(2);
	GET_SINGLE(ItemSlotManager)->ResetItemSlot(3);

	// 플레이어 적용중인 버프,디버프 아이템 초기화
	_curPlayerItem = { false, };

	// 플레이어 점수 초기화, 술래 꼴등이 하는거면 그거 먼저 정하고 해줘야할듯
	_iScore = 0;

	// 플레이어 아이템 개수 초기화
	_iItemCnt = 0;

	// 플레이어 멤버변수들 초기화 (혹시 모르니 해둠)
	_speed = 10.f;
	//_bStunned = false;	// 이동 오류땜에 일단 꺼둠
	_fShieldTime = 0.f;
	_fBlindTime = 0.f;
}

static bool isFirstEnter = true;

void Player::KeyCheck()
{
	// 게임종료
	if (INPUT->GetButtonDown(KEY_TYPE::ESC))
		::PostQuitMessage(0);

	if (mysession == NULL)
		return;

	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
	const vector<shared_ptr<GameObject>>& gameObjects = scene->GetGameObjects();

	_player = scene->GetPlayer(mysession->GetPlayerID());
	Vec3 pos = _player->GetTransform()->GetLocalPosition();
	//////////////////////////////////////////////////////////////////////////
	// 이 부분은 직접 플레이하고 있는 플레이어에만 적용되야 함!!
	// State Check
	PlayerState* state = _state->KeyCheck(*_player, _curStatePlayer);

	// Item KeyCheck /////////////////////////////////////////////////////////
	// stunned면 키입력을 안받아서 스턴일때 알약이 안됨, 코드 위로 옮김
	KeyCheck_Item();
	KeyCheck_Cheat();
	//////////////////////////////////////////////////////////////////////////

	//cout << _bStunned << endl;

	if (_bStunned)	// 멀티플레이 환경에서 stun 상태일때 WALK애니메이션 하지 않게 함
	{
		_curStatePlayer = STATE::STUN;
		//_bStunned = false;
		return;
	}


	_player->_curState = _curStatePlayer;

	if (state != NULL)
	{
		_state->End(*_player);
		delete _state;
		_state = state;
		_state->Enter(*_player);
	}

#pragma region 애니메이션동기화
	switch (_player->_curState)
	{
	case STATE::IDLE:
	{
		StatePkt.set_playerid(mysession->GetPlayerID());
		StatePkt.set_state(Protocol::IDLE);
		if (gPacketControl % 10 == 1)
		{
			auto sendBuffer = ServerPacketHandler::MakeSendBuffer(StatePkt);
			mysession->Send(sendBuffer);
		}
		break;
	}
	case STATE::WALK:
		StatePkt.set_playerid(mysession->GetPlayerID());
		StatePkt.set_state(Protocol::WALK);
		if (gPacketControl % 2 == 1)
		{
			auto sendBuffer = ServerPacketHandler::MakeSendBuffer(StatePkt);
			mysession->Send(sendBuffer);
		}
		break;
	case STATE::JUMP:
	{
		StatePkt.set_playerid(mysession->GetPlayerID());
		StatePkt.set_state(Protocol::JUMP);
		if (gPacketControl % 10 == 1)
		{
			auto sendBuffer = ServerPacketHandler::MakeSendBuffer(StatePkt);
			mysession->Send(sendBuffer);
		}
		break;
	}
	case STATE::ATTACK:
	{
		StatePkt.set_playerid(mysession->GetPlayerID());
		StatePkt.set_state(Protocol::ATTACK);
		if (gPacketControl % 60 == 1)
		{
			auto sendBuffer = ServerPacketHandler::MakeSendBuffer(StatePkt);
			mysession->Send(sendBuffer);
		}
		break;
	}
	case STATE::DASH:
	{
		StatePkt.set_playerid(mysession->GetPlayerID());
		StatePkt.set_state(Protocol::DASH);
		if (gPacketControl % 60 == 1)
		{
			auto sendBuffer = ServerPacketHandler::MakeSendBuffer(StatePkt);
			mysession->Send(sendBuffer);
		}
		break;
	}
	case STATE::SLOW:
	{
		StatePkt.set_playerid(mysession->GetPlayerID());
		StatePkt.set_state(Protocol::SLOW);
		if (gPacketControl % 60 == 1)
		{
			auto sendBuffer = ServerPacketHandler::MakeSendBuffer(StatePkt);
			mysession->Send(sendBuffer);
		}
		break;
	}
	}
#pragma endregion 애니메이션동기화
	//////////////////////////////////////////////////////////////////////////

	Move();
}



void Player::Move()
{
	if (_bStunned)	return;

	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
	const vector<shared_ptr<GameObject>>& gameObjects = scene->GetGameObjects();

	shared_ptr<GameObject> tagObject;
	if (mysession->GetPlayerID() == 0)
		tagObject = scene->GetGameObject(L"PlayerTag1");
	if (mysession->GetPlayerID() == 1)
		tagObject = scene->GetGameObject(L"PlayerTag2");
	if (mysession->GetPlayerID() == 2)
		tagObject = scene->GetGameObject(L"PlayerTag3");

	Vec3 pos = _player->GetTransform()->GetLocalPosition();
	Vec3 rot = _player->GetTransform()->GetLocalRotation();

	
	if (isFirstEnter) {
		Item_Stun();
		isFirstEnter = false;

		// 실드 버그 대문에 시작할때 실드 패킷 한번 보내고 시작
		Protocol::C_USE_SHIELD pkt;
		pkt.set_playerid(mysession->GetPlayerID());
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
		mysession->Send(sendBuffer);

		if (mysession->GetPlayerID() == g_EnterPlayerCnt - 1)
			scene->_FinalPlayerEnter = true;		
	}

	for (auto& gameObject : gameObjects)
	{
		if (gameObject->GetName() == L"Main_Camera")
		{
			_camera = gameObject;
			break;
		}
	}

	_cameraScript = static_pointer_cast<CameraScript>(_camera->GetScript(0));

	// 이동
	if (INPUT->GetButton(KEY_TYPE::UP))
	{
		if (_player->GetIsAllowPlayerMove())
			pos += _player->GetTransform()->GetLook() * _speed * DELTA_TIME;
		else
		{
			if (_dir == DIR::DIR_LEFT)
				pos -= _player->GetTransform()->GetRight() * (_speed / 5.0f) * DELTA_TIME;
			else
				pos += _player->GetTransform()->GetRight() * (_speed / 5.0f) * DELTA_TIME;
		}

		pkt.set_xpos(pos.x);
		pkt.set_ypos(pos.y);
		pkt.set_zpos(pos.z);
		pkt.set_yrot(rot.y);
		pkt.set_playerid(mysession->GetPlayerID());

		//if (gPacketControl % 2 == 1)
		//{
			if (_player->GetIsAllowPlayerMove()) {
				auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
				mysession->Send(sendBuffer);
			}
		//}
		
	}
	else if (INPUT->GetButton(KEY_TYPE::DOWN))
	{
		pos -= _player->GetTransform()->GetLook() * _speed * DELTA_TIME;

		pkt.set_xpos(pos.x);
		pkt.set_ypos(pos.y);
		pkt.set_zpos(pos.z);
		pkt.set_yrot(rot.y);
		pkt.set_playerid(mysession->GetPlayerID());

		//if (gPacketControl % 2 == 1)
		//{
			auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
			mysession->Send(sendBuffer);
		//}
	}

	// 회전
	float delta = 0.f;
	if (INPUT->GetButton(KEY_TYPE::RIGHT))
	{
		_dir = DIR::DIR_RIGHT;

		rot.y += DELTA_TIME * _rotSpeed;
		delta = DELTA_TIME * _rotSpeed;

		pkt.set_xpos(pos.x);
		pkt.set_ypos(pos.y);
		pkt.set_zpos(pos.z);
		pkt.set_yrot(rot.y);
		pkt.set_playerid(mysession->GetPlayerID());

		if (gPacketControl % 5 == 1)
		{
			auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
			mysession->Send(sendBuffer);
		}
		_player->GetTransform()->SetLocalRotation(rot);
	}

	if (INPUT->GetButton(KEY_TYPE::LEFT))
	{
		_dir = DIR::DIR_LEFT;

		rot.y -= DELTA_TIME * _rotSpeed;
		delta = -DELTA_TIME * _rotSpeed;

		pkt.set_xpos(pos.x);
		pkt.set_ypos(pos.y);
		pkt.set_zpos(pos.z);
		pkt.set_yrot(rot.y);
		pkt.set_playerid(mysession->GetPlayerID());

		if (gPacketControl % 5 == 1)
		{
			auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
			mysession->Send(sendBuffer);
		}

		_player->GetTransform()->SetLocalRotation(rot);
	}


	_player->GetTransform()->SetLocalPosition(pos);
	_cameraScript->Revolve(delta, _player->GetTransform()->GetLocalPosition());
	static_pointer_cast<TagMark>(tagObject->GetScript(0))->SetPosition(pos);
	static_pointer_cast<TagMark>(tagObject->GetScript(0))->SetRotation();
}

void Player::KeyCheck_Item()
{
	// 침묵상태이면 아이템 사용 X
	if (_curPlayerItem[Player::ITEM::SILENCE])
		return;

	// 아이템 사용 키입력 - 1, 2, 3
	if (INPUT->GetButtonDown(KEY_TYPE::NUM1))
	{
		if (_playerItemArr[0] == Item::ITEM_EFFECT::NONE) return;
		if (CheckDebuff(_playerItemArr[0])) return;

		UseItem(0);
		GET_SINGLE(ItemSlotManager)->UseItem(1);
		GET_SINGLE(SoundManager)->PlaySound(L"item1.MP3", SoundManager::CHANNELID::ITEM);
		DeletePlayerItem(0);
	}
	if (INPUT->GetButtonDown(KEY_TYPE::NUM2))
	{
		if (_playerItemArr[1] == Item::ITEM_EFFECT::NONE) return;
		if (CheckDebuff(_playerItemArr[1])) return;

		UseItem(1);
		GET_SINGLE(ItemSlotManager)->UseItem(2);
		GET_SINGLE(SoundManager)->PlaySound(L"item1.MP3", SoundManager::CHANNELID::ITEM);
		DeletePlayerItem(1);
	}
	if (INPUT->GetButtonDown(KEY_TYPE::NUM3))
	{
		if (_playerItemArr[2] == Item::ITEM_EFFECT::NONE) return;
		if (CheckDebuff(_playerItemArr[2])) return;

		UseItem(2);
		GET_SINGLE(ItemSlotManager)->UseItem(3);
		GET_SINGLE(SoundManager)->PlaySound(L"item1.MP3", SoundManager::CHANNELID::ITEM);
		DeletePlayerItem(2);
	}
}

void Player::UseItem(int itemNum)
{
	// 침묵상태이면 아이템 사용 X
	if (_curPlayerItem[Player::ITEM::SILENCE])
		return;

	// 키입력을 받은 후 _curPlayerItem의 bool값을 true로 만들어주는 함수
	Item::ITEM_EFFECT itemEffect = _playerItemArr[itemNum];

	printf("사용한 아이템: %d\n", itemEffect);

	switch (itemEffect)
	{
	case Item::ITEM_EFFECT::SPEED_UP:
		_curPlayerItem[Player::ITEM::SPEED_UP] = true;
		break;
	case Item::ITEM_EFFECT::TELEPORT:
		_curPlayerItem[Player::ITEM::TELEPORT] = true;
		break;
	case Item::ITEM_EFFECT::SHIELD:
	{
		_curPlayerItem[Player::ITEM::SHIELD] = true;
		_shieldPlayerIdx = mysession->GetPlayerID();

		// Server
		Protocol::C_USE_SHIELD pkt;
		pkt.set_playerid(mysession->GetPlayerID());
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
		mysession->Send(sendBuffer);
		break;
	}
	case Item::ITEM_EFFECT::SPEED_DOWN:
		//_curPlayerItem[Player::ITEM::SPEED_DOWN] = true;	// test
		Item_SpeedDown();
		break;
	case Item::ITEM_EFFECT::BLIND:
		//_curPlayerItem[Player::ITEM::BLIND] = true;	// test
		Item_Blind();
		break;
	case Item::ITEM_EFFECT::SILENCE:
		//_curPlayerItem[Player::ITEM::SILENCE] = true;	// test
		Item_Silence();
		break;
	case Item::ITEM_EFFECT::STUN:
		//_curPlayerItem[Player::ITEM::STUN] = true;	// test
		Item_Stun();
		break;
	}
}

void Player::ApplyItemEffect()
{
	// _curPlayerItem에서 true인 아이템 효과를 플레이어에 적용시켜주는 함수, Update에서 매번 호출

	if (_curPlayerItem[Player::ITEM::SPEED_UP])
		Item_SpeedUp();

	if (_curPlayerItem[Player::ITEM::TELEPORT])
		Item_Teleport();

	if (_curPlayerItem[Player::ITEM::SHIELD])
		Item_Shield();

	if (_curPlayerItem[Player::ITEM::SPEED_DOWN])
		SlowDown();

	if (_curPlayerItem[Player::ITEM::BLIND])
		Blinded();

	if (_curPlayerItem[Player::ITEM::SILENCE])
		Silence();

	if (_curPlayerItem[Player::ITEM::STUN])
		Stunned();
}

void Player::DeletePlayerItem(int itemIndex)
{
	_playerItemArr[itemIndex] = Item::ITEM_EFFECT::NONE;
	_iItemCnt--;
}

void Player::ClearDebuff()
{
	// 디버프 해제와 쉴드에 사용됨

	// SPEED_DOWN 해제
	if (_curPlayerItem[Player::ITEM::SPEED_DOWN])
	{
		_state->End(*_player);
		delete _state;
		_state = new IdleState;
		_state->Enter(*_player);

		_speed = 10.f;
		_curPlayerItem[Player::ITEM::SPEED_DOWN] = false;
		cout << "SpeedDown 해제" << endl;
	}

	// BLIND 해제
	if (_curPlayerItem[Player::ITEM::BLIND])
	{
		shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
		auto& lights = scene->GetLights();

		for (auto& light : lights)
		{
			static_pointer_cast<Light>(light)->SetDiffuse(Vec3(1.f, 1.f, 1.f));
			static_pointer_cast<Light>(light)->SetAmbient(Vec3(0.3f, 0.3f, 0.3f));
			static_pointer_cast<Light>(light)->SetSpecular(Vec3(0.1f, 0.1f, 0.1f));
		}

		_fBlindTime = 0.f;
		_curPlayerItem[Player::ITEM::BLIND] = false;
		cout << "Blind 해제" << endl;
	}

	// STUN 해제
	if (_curPlayerItem[Player::ITEM::STUN])
	{
		_state->End(*_player);
		delete _state;
		_state = new IdleState;
		_state->Enter(*_player);

		_curPlayerItem[Player::ITEM::STUN] = false;
		_bStunned = false;
		cout << "Stun 해제" << endl;
	}
}

bool Player::CheckShield()
{
	if (_shieldPlayerIdx != mysession->GetPlayerID())	// 실드 쓴 플레이어가 아니면 체크하지않음
		return false;

	if (_curPlayerItem[Player::ITEM::SHIELD])
	{
		_curPlayerItem[Player::ITEM::SHIELD] = false;
		_fShieldTime = 0.f;
		GET_SINGLE(ItemSlotManager)->UseShieldItem();
		GET_SINGLE(ShieldParticleManager)->SetShieldParticleOff();	// 쉴드 파티클 효과 해제
		return true;
	}

	else
		return false;
}

bool Player::CheckDebuff(Item::ITEM_EFFECT itemEffect)
{
	// 아이템 사용 키체크 전에 호출
	// 디버프 효과를 받는 중이라면, 버프 아이템 사용 불가능 -> true 리턴
	if (_curPlayerItem[ITEM::SPEED_DOWN] || _curPlayerItem[ITEM::BLIND] || _curPlayerItem[ITEM::STUN])
	{
		if (itemEffect == Item::ITEM_EFFECT::SPEED_UP ||
			itemEffect == Item::ITEM_EFFECT::TELEPORT ||
			itemEffect == Item::ITEM_EFFECT::SHIELD ||
			itemEffect == Item::ITEM_EFFECT::SILENCE ||
			itemEffect == Item::ITEM_EFFECT::STUN ||
			itemEffect == Item::ITEM_EFFECT::SPEED_DOWN ||
			itemEffect == Item::ITEM_EFFECT::BLIND)
		{
			cout << "디버프중 - 버프아이템 사용 못함" << endl;
			return true;
		}
	}

	return false;
}

void Player::KeyCheck_Cheat()
{
	// 보유 아이템 확인
	if (INPUT->GetButtonDown(KEY_TYPE::TEST_KEY))	// T
	{
		printf("1번 아이템: %d\n", _playerItemArr[0]);
		printf("2번 아이템: %d\n", _playerItemArr[1]);
		printf("3번 아이템: %d\n", _playerItemArr[2]);
	}

	// 치트키
	if (INPUT->GetButtonDown(KEY_TYPE::NUM4))	// 아이템 창 비움
	{
		_playerItemArr[0] = Item::ITEM_EFFECT::NONE;
		_playerItemArr[1] = Item::ITEM_EFFECT::NONE;
		_playerItemArr[2] = Item::ITEM_EFFECT::NONE;

		GET_SINGLE(ItemSlotManager)->ResetItemSlot(1);
		GET_SINGLE(ItemSlotManager)->ResetItemSlot(2);
		GET_SINGLE(ItemSlotManager)->ResetItemSlot(3);

		_iItemCnt = 0;
	}

	// 속도 원래대로 & speedUp speedDown 해제
	if (INPUT->GetButtonDown(KEY_TYPE::F))
	{
		_speed = 10.f;
		_curPlayerItem[ITEM::SPEED_UP] = false;
		_curPlayerItem[ITEM::SPEED_DOWN] = false;

		_state->End(*GetGameObject());
		delete _state;
		_state = new IdleState;
		_state->Enter(*GetGameObject());
	}

	// 아이템 생성 치트키
	if (INPUT->GetButtonDown(KEY_TYPE::Z))
	{
		_playerItemArr[0] = Item::ITEM_EFFECT::SPEED_UP;
		GET_SINGLE(ItemSlotManager)->AddItem_Cheat(Item::ITEM_EFFECT::SPEED_UP);
		if (_playerItemArr[0] == Item::ITEM_EFFECT::NONE)
			_iItemCnt++;
	}
	if (INPUT->GetButtonDown(KEY_TYPE::X))
	{
		_playerItemArr[0] = Item::ITEM_EFFECT::TELEPORT;
		GET_SINGLE(ItemSlotManager)->AddItem_Cheat(Item::ITEM_EFFECT::TELEPORT);
		if (_playerItemArr[0] == Item::ITEM_EFFECT::NONE)
			_iItemCnt++;
	}
	if (INPUT->GetButtonDown(KEY_TYPE::C))
	{
		_playerItemArr[0] = Item::ITEM_EFFECT::SHIELD;
		GET_SINGLE(ItemSlotManager)->AddItem_Cheat(Item::ITEM_EFFECT::SHIELD);
		if (_playerItemArr[0] == Item::ITEM_EFFECT::NONE)
			_iItemCnt++;
	}
	if (INPUT->GetButtonDown(KEY_TYPE::V))
	{
		_playerItemArr[0] = Item::ITEM_EFFECT::SILENCE;
		GET_SINGLE(ItemSlotManager)->AddItem_Cheat(Item::ITEM_EFFECT::SILENCE);
		if (_playerItemArr[0] == Item::ITEM_EFFECT::NONE)
			_iItemCnt++;
	}
	if (INPUT->GetButtonDown(KEY_TYPE::B))
	{
		_playerItemArr[0] = Item::ITEM_EFFECT::STUN;
		GET_SINGLE(ItemSlotManager)->AddItem_Cheat(Item::ITEM_EFFECT::STUN);
		if (_playerItemArr[0] == Item::ITEM_EFFECT::NONE)
			_iItemCnt++;
	}
	if (INPUT->GetButtonDown(KEY_TYPE::N))
	{
		_playerItemArr[0] = Item::ITEM_EFFECT::SPEED_DOWN;
		GET_SINGLE(ItemSlotManager)->AddItem_Cheat(Item::ITEM_EFFECT::SPEED_DOWN);
		if (_playerItemArr[0] == Item::ITEM_EFFECT::NONE)
			_iItemCnt++;
	}
	if (INPUT->GetButtonDown(KEY_TYPE::M))
	{
		_playerItemArr[0] = Item::ITEM_EFFECT::BLIND;
		GET_SINGLE(ItemSlotManager)->AddItem_Cheat(Item::ITEM_EFFECT::BLIND);
		if (_playerItemArr[0] == Item::ITEM_EFFECT::NONE)
			_iItemCnt++;
	}

	// 사용중인 아이템 확인 치트키
	if (INPUT->GetButtonDown(KEY_TYPE::I))
	{
		cout << "SpeedUp: " << _curPlayerItem[ITEM::SPEED_UP] << endl;
		cout << "Teleport: " << _curPlayerItem[ITEM::TELEPORT] << endl;
		cout << "Shield: " << _curPlayerItem[ITEM::SHIELD] << endl;
		cout << "SpeedDown: " << _curPlayerItem[ITEM::SPEED_DOWN] << endl;
		cout << "Blind: " << _curPlayerItem[ITEM::BLIND] << endl;
		cout << "Silence: " << _curPlayerItem[ITEM::SILENCE] << endl;
		cout << "Stun: " << _curPlayerItem[ITEM::STUN] << endl;
		cout << endl;
	}

	// 디버프 해제 치트키 (스턴 제외)
	if (INPUT->GetButtonDown(KEY_TYPE::O))
	{
		_curPlayerItem[ITEM::SPEED_DOWN] = false;
		_curPlayerItem[ITEM::BLIND] = false;
		_curPlayerItem[ITEM::SILENCE] = false;

		_speed = _originalSpeed;
		_fBlindTime = 0.f;
		_fSilenceTime = 0.f;
		GET_SINGLE(ItemSlotManager)->IsSilenced(false);

		shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
		auto& lights = scene->GetLights();
		for (auto& light : lights)
		{
			static_pointer_cast<Light>(light)->SetDiffuse(Vec3(1.f, 1.f, 1.f));
			static_pointer_cast<Light>(light)->SetAmbient(Vec3(0.3f, 0.3f, 0.3f));
			static_pointer_cast<Light>(light)->SetSpecular(Vec3(0.1f, 0.1f, 0.1f));
		}


		_state->End(*GetGameObject());
		delete _state;
		_state = new IdleState;
		_state->Enter(*GetGameObject());

		//_curPlayerItem[ITEM::STUN] = false;
	}
}

void Player::Item_SpeedUp()
{
	// 스피드 변경 전 한번만 하도록
	if (_speed != _dashSpeed)
	{
		_speed = _dashSpeed;

		_state->End(*_player);
		delete _state;
		_state = new DashState;
		_state->Enter(*_player);

		StatePkt.set_playerid(mysession->GetPlayerID());
		StatePkt.set_state(Protocol::DASH);
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(StatePkt);
		mysession->Send(sendBuffer);
	}
}

void Player::Item_Teleport()
{
	// 카메라랑 플레이어 지금 방향의 앞으로 순간이동
	Vec3 pos = GetTransform()->GetLocalPosition();
	pos += GetTransform()->GetLook() * _fTeleportDis;
	GetTransform()->SetLocalPosition(pos);

	_curPlayerItem[Player::ITEM::TELEPORT] = false;
}

void Player::Item_Shield()
{
	// 쉴드 상태라면 디버프 방어, 쉴드 상태때 디버프 1회 방해했으면 쉴드 해제

	_fShieldTime += DELTA_TIME;

	// Shield Item
	if (_fShieldTime <= 5.f)
	{
		// Shield Effect
		ShieldEffect();
	}

	else if (_fShieldTime > 5.f)
	{
		_curPlayerItem[Player::ITEM::SHIELD] = false;

		//if (mysession->GetPlayerID() == _shieldPlayerIdx)
		//	GET_SINGLE(ItemSlotManager)->UseShieldItem();
		GET_SINGLE(ShieldParticleManager)->SetShieldParticleOff();
		_fShieldTime = 0.f;
		cout << "5초 지나고 쉴드 끝" << endl;
	}
}

void Player::Item_SpeedDown()
{
	/////////////// prod by. wc ///////////////
	Protocol::C_USE_DEBUFITEM pkt;
	pkt.set_fromplayerid(mysession->GetPlayerID());
	pkt.set_itemtype(Protocol::DEBUF_SPEEDDOWN);
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
	mysession->Send(sendBuffer);
	///////////////////////////////////////////
}

void Player::Item_Blind()
{
	/////////////// prod by. wc ///////////////
	Protocol::C_USE_DEBUFITEM pkt;
	pkt.set_fromplayerid(mysession->GetPlayerID());
	pkt.set_itemtype(Protocol::DEBUF_BLIND);
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
	mysession->Send(sendBuffer);
	///////////////////////////////////////////
}

void Player::Item_Stun()
{
	cout << mysession->GetPlayerID() << " : 스턴 보냄\n";
	/////////////// prod by. wc ///////////////
	Protocol::C_USE_STUN pkt;
	pkt.set_fromplayerid(mysession->GetPlayerID());
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
	mysession->Send(sendBuffer);
	///////////////////////////////////////////
}

void Player::Item_Silence()
{
	// 서버 부분
	Protocol::C_USE_SILENCE pkt;
	pkt.set_fromplayerid(mysession->GetPlayerID());
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
	mysession->Send(sendBuffer);
}

void Player::SlowDown()
{
	if (CheckShield())
	{
		_curPlayerItem[Player::ITEM::SPEED_DOWN] = false;
		cout << "쉴드 방어: SPEED_DOWN" << endl;
		GET_SINGLE(ItemSlotManager)->UseShieldItem();
		return;
	}

	// 자신 제외 모든 플레이어 5초동안 속도 감소
	if (_speed != _slowSpeed)
	{
		_state->End(*GetGameObject());
		delete _state;
		_state = new SlowState;
		_state->Enter(*GetGameObject());
	}
}

void Player::Blinded()
{
	if (CheckShield())
	{
		_curPlayerItem[Player::ITEM::BLIND] = false;
		cout << "쉴드 방어: BLIND" << endl;
		GET_SINGLE(ItemSlotManager)->UseShieldItem();
		return;
	}

	_fBlindTime += DELTA_TIME;

	// 연막, 씬 안의 조명을 가져와서 어둡게
	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
	auto& lights = scene->GetLights();

	for (auto& light : lights)
	{
		static_pointer_cast<Light>(light)->SetDiffuse(Vec3(0.f, 0.f, 0.f));
		static_pointer_cast<Light>(light)->SetAmbient(Vec3(0.01f, 0.01f, 0.01f));
		static_pointer_cast<Light>(light)->SetSpecular(Vec3(0.01f, 0.01f, 0.01f));
	}

	if (_fBlindTime > 5.f)
	{
		for (auto& light : lights)
		{
			static_pointer_cast<Light>(light)->SetDiffuse(Vec3(1.f, 1.f, 1.f));
			static_pointer_cast<Light>(light)->SetAmbient(Vec3(0.3f, 0.3f, 0.3f));
			static_pointer_cast<Light>(light)->SetSpecular(Vec3(0.1f, 0.1f, 0.1f));
		}

		_fBlindTime = 0.f;
		_curPlayerItem[Player::ITEM::BLIND] = false;
	}
}

void Player::Stunned()
{
	if (CheckShield())
	{
		_curPlayerItem[Player::ITEM::STUN] = false;

		cout << "쉴드 방어: STUN" << endl;
		GET_SINGLE(ItemSlotManager)->UseShieldItem();

		return;
	}

	// 유니크 아이템 - 3초간 스턴 & 텍스처 매핑
	if (!_bStunned && !_curPlayerItem[ITEM::SHIELD])
	{
		if (!isFirstEnter) {
			_state->End(*_player);
			delete _state;
			_state = new StunState;
			_state->Enter(*_player);
		}

		_bStunned = true;
	}
}

void Player::Silence() 
{
	if (CheckShield())
	{
		cout << "쉴드 방어: SILENCE" << endl;
		GET_SINGLE(ItemSlotManager)->UseShieldItem();
		return;
	}

	_fSilenceTime += DELTA_TIME;

	// UI 변경
	GET_SINGLE(ItemSlotManager)->IsSilenced(true);

	if (_fSilenceTime >= 5.f)
	{
		_fSilenceTime = 0.f;
		_curPlayerItem[Player::ITEM::SILENCE] = false;

		// UI 변경
		GET_SINGLE(ItemSlotManager)->IsSilenced(false);
	}
}

void Player::ShieldEffect()
{
	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();

	_Shield_Effect_player = scene->GetPlayer(_shieldPlayerIdx);
	GET_SINGLE(ShieldParticleManager)->SetShieldParticleOn();

	GET_SINGLE(ShieldParticleManager)->UpdatePlayerPos(_Shield_Effect_player->GetTransform()->GetLocalPosition());
}
