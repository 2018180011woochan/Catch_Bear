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

Protocol::C_MOVE pkt;

Player::Player()
{
	// �������� ��Ʈ���ϴ� �÷��̾�� _state ���������� �ȵ�
	_state = new IdleState();
	_curPlayerItem = { false, };
}

Player::~Player()
{
}

void Player::Update()
{
	ApplyItemEffect();
}

void Player::LateUpdate()
{
	//if (_bStunned) return;
	// �������� ��Ʈ���ϴ� �÷��̾�� �������� ��ġ���� �޾ƿ��ϱ� �ʿ������
	KeyCheck();

	////////////////////////////////////////////////////////////////////
	// �� �κ��� ���� �÷����ϰ� �ִ� �÷��̾�� ����Ǿ� ��!!
	PlayerState* state = _state->Update(*_player, _curState);
	_player->_curState = _curState;

	if (state != NULL)
	{
		_state->End(*_player);
		delete _state;
		_state = state;
		_state->Enter(*_player);
	}

#pragma region �ִϸ��̼ǵ���ȭ
	switch (_player->_curState)
	{
	case STATE::WALK:
		pkt.set_playerid(mysession->GetPlayerID());
		pkt.set_state(Protocol::WALK);
		break;
	case STATE::JUMP:
	{
		pkt.set_playerid(mysession->GetPlayerID());
		pkt.set_state(Protocol::JUMP);
		if (gPacketControl % 60 == 1)
		{
			auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
			mysession->Send(sendBuffer);
		}
		break;
	}
	case STATE::ATTACK:
	{
		pkt.set_playerid(mysession->GetPlayerID());
		pkt.set_state(Protocol::ATTACK);
		if (gPacketControl % 60 == 1)
		{
			auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
			mysession->Send(sendBuffer);
		}
		break;
	}
	case STATE::STUN:
	{
		pkt.set_playerid(mysession->GetPlayerID());
		pkt.set_state(Protocol::STUN);
		if (gPacketControl % 60 == 1)
		{
			auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
			mysession->Send(sendBuffer);
		}
		break;
	}
	case STATE::DASH:
	{
		pkt.set_playerid(mysession->GetPlayerID());
		pkt.set_state(Protocol::DASH);
		if (gPacketControl % 60 == 1)
		{
			auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
			mysession->Send(sendBuffer);
		}
		break;
	}
	}
#pragma endregion �ִϸ��̼ǵ���ȭ
	////////////////////////////////////////////////////////////////////

	/*Vec3 pos = GetTransform()->GetLocalPosition();
	printf("%f, %f, %f\n", pos.x, pos.y, pos.z);*/

	// �ִϸ��̼� ����ϴ� �κ� -> ��� ����Ǿ� ��
	GetAnimationController()->AdvanceTime(DELTA_TIME);
	GetTransform()->UpdateTransform(NULL);
	GetAnimationController()->SetWorldMatrix();
	
	Vec3 trans = GetTransform()->GetLocalPosition();
	//printf("%f, %f, %f\n", trans.x, trans.y, trans.z);
	
}

void Player::AddPlayerItem(shared_ptr<GameObject> item)
{
	// �÷��̾ �������� �� �ִ� �ִ� ������ ���� 3��
	// �������� 3�� ���ϰ� ������ ������ ȹ���ص� ����
	if (_playerItemVec.size() < 3)
	{
		_playerItemVec.push_back(item);

		// �ش� �������� Ȯ���ؼ� slot�� �´� �ؽ�ó�� �����ؾ� �Ѵ�.
		ITEM_EFFECT itemType = static_pointer_cast<Item>(item->GetScript(0))->GetItemEffect();
		GET_SINGLE(ItemSlotManager)->AddItem(itemType);
	}
}

void Player::KeyCheck()
{
	// ��������
	if (INPUT->GetButtonDown(KEY_TYPE::ESC))
		::PostQuitMessage(0);

	

	if (mysession == NULL)
		return;

	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
	const vector<shared_ptr<GameObject>>& gameObjects = scene->GetGameObjects();

	_player = scene->GetPlayer(mysession->GetPlayerID());
	Vec3 pos = _player->GetTransform()->GetLocalPosition();
	//////////////////////////////////////////////////////////////////////////
	// �� �κ��� ���� �÷����ϰ� �ִ� �÷��̾�� ����Ǿ� ��!!
	// State Check
	PlayerState* state = _state->KeyCheck(*_player, _curState);

	if (_bStunned) return;		// ��Ƽ�÷��� ȯ�濡�� stun �����϶� WALK�ִϸ��̼� ���� �ʰ� ��

	_player->_curState = _curState;

	if (state != NULL)
	{
		_state->End(*_player);
		delete _state;
		_state = state;
		_state->Enter(*_player);
	}

#pragma region �ִϸ��̼ǵ���ȭ
	if (_player->GetPlayerID() != mysession->GetPlayerID())	//������
		int a = 10;

	switch (_player->_curState)
	{
	case STATE::IDLE:
	{
		pkt.set_playerid(mysession->GetPlayerID());
		pkt.set_state(Protocol::IDLE);
		pkt.set_xpos(pos.x);
		pkt.set_ypos(pos.y);
		pkt.set_zpos(pos.z);
		if (gPacketControl % 80 == 1)
		{
			auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
			mysession->Send(sendBuffer);
		}
		break;
	}
	case STATE::JUMP:
	{
		pkt.set_playerid(mysession->GetPlayerID());
		pkt.set_state(Protocol::JUMP);
		if (gPacketControl % 10 == 1)
		{
			auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
			mysession->Send(sendBuffer);
		}
		break;
	}
	case STATE::ATTACK:
	{
		pkt.set_playerid(mysession->GetPlayerID());
		pkt.set_state(Protocol::ATTACK);
		if (gPacketControl % 60 == 1)
		{
			auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
			mysession->Send(sendBuffer);
		}
		break;
	}
	case STATE::STUN:
	{
		pkt.set_playerid(mysession->GetPlayerID());
		pkt.set_state(Protocol::STUN);
		if (gPacketControl % 60 == 1)
		{
			auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
			mysession->Send(sendBuffer);
		}
		break;
	}
	case STATE::DASH:
	{
		pkt.set_playerid(mysession->GetPlayerID());
		pkt.set_state(Protocol::DASH);
		if (gPacketControl % 60 == 1)
		{
			auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
			mysession->Send(sendBuffer);
		}
		break;
	}
	}
#pragma endregion �ִϸ��̼ǵ���ȭ
	//////////////////////////////////////////////////////////////////////////

	Move();
	KeyCheck_Item();
}

static bool isFirstEnter = true;

void Player::Move()
{
	if (_bStunned) return;

	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
	const vector<shared_ptr<GameObject>>& gameObjects = scene->GetGameObjects();
	shared_ptr<GameObject> tagObject = scene->GetGameObject(L"PlayerTag1");

	Vec3 pos = _player->GetTransform()->GetLocalPosition();
	Vec3 rot = _player->GetTransform()->GetLocalRotation();

	// �̵� ���������� �����Ҷ� stun ��Ŷ �ѹ� ������ ����
	if (isFirstEnter) {
		Item_Stun();
		isFirstEnter = false;
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

	// �̵�
	if (INPUT->GetButton(KEY_TYPE::UP))
	{
		if (_player->GetIsAllowPlayerMove())
			pos += _player->GetTransform()->GetLook() * _speed * DELTA_TIME;
		
		pkt.set_xpos(pos.x);
		pkt.set_ypos(pos.y);
		pkt.set_zpos(pos.z);
		pkt.set_yrot(rot.y);
		pkt.set_playerid(mysession->GetPlayerID());
		pkt.set_movedir(0);

		if (gPacketControl % 3 == 1)
		{
			if (_player->GetIsAllowPlayerMove()) {
				auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
				mysession->Send(sendBuffer);
			}
		}
		
	}
	else if (INPUT->GetButton(KEY_TYPE::DOWN))
	{
		pos -= _player->GetTransform()->GetLook() * _speed * DELTA_TIME;

		pkt.set_xpos(pos.x);
		pkt.set_ypos(pos.y);
		pkt.set_zpos(pos.z);
		pkt.set_yrot(rot.y);
		pkt.set_playerid(mysession->GetPlayerID());
		pkt.set_movedir(1);

		if (gPacketControl % 3 == 1)
		{
			auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
			mysession->Send(sendBuffer);
		}
	}

	// ȸ��
	float delta = 0.f;
	if (INPUT->GetButton(KEY_TYPE::RIGHT))
	{
		rot.y += DELTA_TIME * _rotSpeed;
		delta = DELTA_TIME * _rotSpeed;

		Vec3 pl = _player->GetTransform()->GetLook();
		cout << "�÷��̾� " << _player->GetPlayerID() << " look: " <<
			pl.x << ", " << pl.y << ", " << pl.z << endl;

		pkt.set_xpos(pos.x);
		pkt.set_ypos(pos.y);
		pkt.set_zpos(pos.z);
		pkt.set_yrot(rot.y);
		pkt.set_playerid(mysession->GetPlayerID());

		if (gPacketControl % 3 == 1)
		{
			auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
			mysession->Send(sendBuffer);
		}
		_player->GetTransform()->SetLocalRotation(rot);
	}

	if (INPUT->GetButton(KEY_TYPE::LEFT))
	{
		rot.y -= DELTA_TIME * _rotSpeed;
		delta = -DELTA_TIME * _rotSpeed;

		Vec3 pl = _player->GetTransform()->GetLook();
		cout << "�÷��̾� " << _player->GetPlayerID() << " look: " <<
			pl.x << ", " << pl.y << ", " << pl.z << endl;

		pkt.set_xpos(pos.x);
		pkt.set_ypos(pos.y);
		pkt.set_zpos(pos.z);
		pkt.set_yrot(rot.y);
		pkt.set_playerid(mysession->GetPlayerID());

		if (gPacketControl % 3 == 1)
		{
			auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
			mysession->Send(sendBuffer);
		}

		_player->GetTransform()->SetLocalRotation(rot);
	}

	_player->GetTransform()->SetLocalPosition(pos);
	_cameraScript->Revolve(delta, _player->GetTransform()->GetLocalPosition());
	static_pointer_cast<TagMark>(tagObject->GetScript(0))->SetPosition(pos);
}

void Player::KeyCheck_Item()
{
	// ������ ��� Ű�Է� - 1, 2, 3
	if (INPUT->GetButtonDown(KEY_TYPE::NUM1))
	{
		if (_playerItemVec.size() < 1)
			return;

		UseItem(0);
		GET_SINGLE(ItemSlotManager)->UseItem(1);
		DeletePlayerItem(0);
	}
	if (INPUT->GetButtonDown(KEY_TYPE::NUM2))
	{
		if (_playerItemVec.size() < 2) 
			return;

		UseItem(1);
		GET_SINGLE(ItemSlotManager)->UseItem(2);
		DeletePlayerItem(1);
	}
	if (INPUT->GetButtonDown(KEY_TYPE::NUM3))
	{
		if (_playerItemVec.size() < 3) 
			return;
		
		UseItem(2);
		GET_SINGLE(ItemSlotManager)->UseItem(3);
		DeletePlayerItem(2);
	}

	//if (INPUT->GetButtonDown(KEY_TYPE::TEST_KEY))
	//	SlowDown();
}

void Player::UseItem(int itemNum)
{
	// Ű�Է��� ���� �� _curPlayerItem�� bool���� true�� ������ִ� �Լ�
	ITEM_EFFECT itemEffect = static_pointer_cast<Item>(_playerItemVec[itemNum]->GetScript(0))->GetItemEffect();

	switch (itemEffect)
	{
	case ITEM_EFFECT::SPEED_UP:
		_curPlayerItem[Player::ITEM::SPEED_UP] = true;
		break;
	case ITEM_EFFECT::TELEPORT:
		_curPlayerItem[Player::ITEM::TELEPORT] = true;
		break;
	case ITEM_EFFECT::SHIELD:
		_curPlayerItem[Player::ITEM::SHIELD] = true;
		break;
	case ITEM_EFFECT::SPEED_DOWN:
		// �ٸ� �÷��̾���� �ӵ� ���ҽ��Ѿ���
		//_curPlayerItem[Player::ITEM::SPEED_DOWN] = true;	// test
		Item_SpeedDown();
		break;
	case ITEM_EFFECT::BLIND:
		// �ٸ� �÷��̾���� �þ� �帮��
		//_curPlayerItem[Player::ITEM::BLIND] = true;	// test
		Item_Blind();
		break;
	case ITEM_EFFECT::DEBUFF_OFF:
		_curPlayerItem[Player::ITEM::DEBUFF_OFF] = true;	// test
		Item_DebuffOff();
		break;
	case ITEM_EFFECT::STUN:
		// �ٸ� �÷��̾�� ���ϰɱ�
		//_curPlayerItem[Player::ITEM::STUN] = true;	// test
		Item_Stun();
		break;
	}

}

void Player::ApplyItemEffect()
{
	// _curPlayerItem���� true�� ������ ȿ���� �÷��̾ ��������ִ� �Լ�, Update���� �Ź� ȣ��

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

	if (_curPlayerItem[Player::ITEM::DEBUFF_OFF])
		Item_DebuffOff();

	if (_curPlayerItem[Player::ITEM::STUN])
		Stunned();
}

void Player::DeletePlayerItem(int itemIndex)
{
	_playerItemVec.erase(_playerItemVec.begin() + itemIndex);
}

void Player::Item_SpeedUp()
{
	// ���ǵ� ���� �� �ѹ��� �ϵ���
	if (_speed == _originalSpeed)
	{
		_state->End(*_player);
		delete _state;_state = new DashState;
		_state->Enter(*_player);

		pkt.set_playerid(mysession->GetPlayerID());
		pkt.set_state(Protocol::DASH);
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
		mysession->Send(sendBuffer);
	}
}

void Player::Item_Teleport()
{
	// ī�޶�� �÷��̾� ���� ������ ������ �����̵�
	Vec3 pos = GetTransform()->GetLocalPosition();
	pos += GetTransform()->GetLook() * _fTeleportDis;
	GetTransform()->SetLocalPosition(pos);

	_curPlayerItem[Player::ITEM::TELEPORT] = false;
}

void Player::Item_Shield()
{
	// ���� ���¶�� ����� ���
	// ���� ���¶� ����� 1ȸ ���������� ���� ����
	// �ؿ��� ���� ���� �ö���� ��ƼŬ ȿ�� �߰� - ���� �����ΰ� �� �� �ֵ���
	_fShieldTime += DELTA_TIME;
	
	if (_fShieldTime <= 5.f)
	{
		if (!_bUseShield)
		{
			_curPlayerItem[Player::ITEM::SPEED_DOWN] = false;
			_curPlayerItem[Player::ITEM::BLIND] = false;
			_curPlayerItem[Player::ITEM::STUN] = false;

			_bUseShield = true;
		}
	}
	else if (_bUseShield || _fShieldTime > 5.f)
	{
		_fShieldTime = 0.f;
		_curPlayerItem[Player::ITEM::SHIELD] = false;
		_bUseShield = false;
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

void Player::Item_DebuffOff()
{
	// ����ũ ������ - �ڽſ��� �ɷ��ִ� ��� ����� ����
	// �����: SPEED_DOWN, BLIND, STUN
	_curPlayerItem[Player::ITEM::SPEED_DOWN] = false;
	_curPlayerItem[Player::ITEM::BLIND] = false;
	_curPlayerItem[Player::ITEM::STUN] = false;

	_curPlayerItem[Player::ITEM::DEBUFF_OFF] = false;
}

void Player::Item_Stun()
{
	/////////////// prod by. wc ///////////////
	Protocol::C_USE_STUN pkt;
	pkt.set_fromplayerid(mysession->GetPlayerID());
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
	mysession->Send(sendBuffer);
	///////////////////////////////////////////
}

void Player::SlowDown()
{
	// �ڽ� ���� ��� �÷��̾� 5�ʵ��� �ӵ� ����
	if (_speed == _originalSpeed)
	{
		_state->End(*GetGameObject());
		delete _state;
		_state = new SlowState;
		_state->Enter(*GetGameObject());
	}
}

void Player::Blinded()
{
	_fBlindTime += DELTA_TIME;

	// ����, �� ���� ������ �����ͼ� ��Ӱ�?
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
			static_pointer_cast<Light>(light)->SetDiffuse(Vec3(1.f, 1.f, 1.f));

		_fBlindTime = 0.f;
		_curPlayerItem[Player::ITEM::BLIND] = false;
	}
}

void Player::Stunned()
{
	// ����ũ ������ - 3�ʰ� ����
	if (!_bStunned)
	{
		_bStunned = true;

		if (!isFirstEnter) {
			_state->End(*_player);
			delete _state;
			_state = new StunState;
			_state->Enter(*_player);
		}
	}
}
