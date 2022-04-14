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
#include "ParticleSystem.h"

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

	//CreateParticle();

}

void Player::LateUpdate()
{
	//if (_bStunned) return;

	// �������� ��Ʈ���ϴ� �÷��̾�� �������� ��ġ���� �޾ƿ��ϱ� �ʿ������
	KeyCheck();

	////////////////////////////////////////////////////////////////////
	// �� �κ��� ���� �÷����ϰ� �ִ� �÷��̾�� ����Ǿ� ��!!
	PlayerState* state = _state->Update(*GetGameObject(), _curState);
	GetGameObject()->_curState = _curState;

	if (state != NULL)
	{
		_state->End(*GetGameObject());
		delete _state;
		_state = state;
		_state->Enter(*GetGameObject());
	}
	////////////////////////////////////////////////////////////////////

	Vec3 pos = GetTransform()->GetLocalPosition();
	printf("%f, %f, %f\n", pos.x, pos.y, pos.z);

	// �ִϸ��̼� ����ϴ� �κ� -> ��� ����Ǿ� ��
	GetAnimationController()->AdvanceTime(DELTA_TIME);
	GetTransform()->UpdateTransform(NULL);
	GetAnimationController()->SetWorldMatrix();
	
}

void Player::AddPlayerItem(shared_ptr<GameObject> item)
{
	// �÷��̾ �������� �� �ִ� �ִ� ������ ���� 3��
	// �������� 3�� ���ϰ� ������ ��ó�� ������ ����
	if (_playerItemVec.size() < 3)
	{
		_playerItemVec.push_back(item);
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

	for (auto& gameObject : gameObjects)
	{
		if (gameObject->GetName() == L"Player" && gameObject->GetPlayerID() == mysession->GetPlayerID())
		{
			_player = gameObject;
			break;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// �� �κ��� ���� �÷����ϰ� �ִ� �÷��̾�� ����Ǿ� ��!!
	// State Check
	PlayerState* state = _state->KeyCheck(*GetGameObject(), _curState);
	GetGameObject()->_curState = _curState;

	if (state != NULL)
	{
		delete _state;
		_state = state;
		_state->Enter(*GetGameObject());
	}
	//////////////////////////////////////////////////////////////////////////

	//if (INPUT->GetButtonDown(KEY_TYPE::TEST_KEY))
	//	CreateParticle();

	Move();
	KeyCheck_Item();
}

void Player::Move()
{
	if (_bStunned) return;

	Protocol::C_MOVE pkt;

	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
	const vector<shared_ptr<GameObject>>& gameObjects = scene->GetGameObjects();

	Vec3 pos = _player->GetTransform()->GetLocalPosition();
	Vec3 rot = _player->GetTransform()->GetLocalRotation();

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
		pos += _player->GetTransform()->GetLook() * _speed * DELTA_TIME;

		pkt.set_xpos(pos.x);
		pkt.set_ypos(pos.y);
		pkt.set_zpos(pos.z);
		pkt.set_yrot(rot.y);
		pkt.set_playerid(mysession->GetPlayerID());
		pkt.set_state(Protocol::WALK);
		pkt.set_iskeydown(true);
		pkt.set_movedir(0);

		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
		mysession->Send(sendBuffer);
	}
	else if (INPUT->GetButton(KEY_TYPE::DOWN))
	{
		pos -= _player->GetTransform()->GetLook() * _speed * DELTA_TIME;

		pkt.set_xpos(pos.x);
		pkt.set_ypos(pos.y);
		pkt.set_zpos(pos.z);
		pkt.set_yrot(rot.y);
		pkt.set_playerid(mysession->GetPlayerID());
		pkt.set_state(Protocol::WALK);
		pkt.set_iskeydown(true);
		pkt.set_movedir(1);

		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
		mysession->Send(sendBuffer);
	}
	else		// ���⼭ �����Ӹ��� ��Ŷ �����°� ���� �ȵ� ���߿� �����Ҳ���
	{
		pkt.set_xpos(pos.x);
		pkt.set_ypos(pos.y);
		pkt.set_zpos(pos.z);
		pkt.set_yrot(rot.y);
		pkt.set_playerid(mysession->GetPlayerID());
		pkt.set_iskeydown(false);

		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
		mysession->Send(sendBuffer);
	}

	Vec3 look = GetTransform()->GetLook();
	int a = 0;

	// ȸ��
	float delta = 0.f;
	if (INPUT->GetButton(KEY_TYPE::RIGHT))
	{
		rot.y += DELTA_TIME * _rotSpeed;
		delta = DELTA_TIME * _rotSpeed;

		//GetTransform()->SetLocalRotation(rot);

		pkt.set_xpos(pos.x);
		pkt.set_ypos(pos.y);
		pkt.set_zpos(pos.z);
		pkt.set_yrot(rot.y);
		pkt.set_playerid(mysession->GetPlayerID());
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
		mysession->Send(sendBuffer);
	}

	if (INPUT->GetButton(KEY_TYPE::LEFT))
	{
		rot.y -= DELTA_TIME * _rotSpeed;
		delta = -DELTA_TIME * _rotSpeed;

		pkt.set_xpos(pos.x);
		pkt.set_ypos(pos.y);
		pkt.set_zpos(pos.z);
		pkt.set_yrot(rot.y);
		pkt.set_playerid(mysession->GetPlayerID());
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
		mysession->Send(sendBuffer);
	}

	GetTransform()->SetLocalPosition(pos);
	_cameraScript->Revolve(delta, GetTransform()->GetLocalPosition());
}

void Player::KeyCheck_Item()
{
	// ������ ��� Ű�Է� - 1, 2, 3
	if (INPUT->GetButtonDown(KEY_TYPE::NUM1))
	{
		if (_playerItemVec.empty()) return;
		UseItem(0);
	}
	if (INPUT->GetButtonDown(KEY_TYPE::NUM2))
	{
		if (_playerItemVec.size() < 1) return;
		UseItem(1);
	}
	if (INPUT->GetButtonDown(KEY_TYPE::NUM3))
	{
		if (_playerItemVec.size() < 2) return;
		UseItem(2);
	}
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
		_curPlayerItem[Player::ITEM::SPEED_DOWN] = true;	// test
		Item_SpeedDown();
		break;
	case ITEM_EFFECT::BLIND:
		// �ٸ� �÷��̾���� �þ� �帮��
		_curPlayerItem[Player::ITEM::BLIND] = true;	// test
		Item_Blind();
		break;
	case ITEM_EFFECT::DEBUFF_OFF:
		_curPlayerItem[Player::ITEM::DEBUFF_OFF] = true;	// test
		Item_DebuffOff();
		break;
	case ITEM_EFFECT::STUN:
		// �ٸ� �÷��̾�� ���ϰɱ�
		_curPlayerItem[Player::ITEM::STUN] = true;	// test
		Item_Stun();
		break;
	}

	_playerItemVec.erase(_playerItemVec.begin() + itemNum);
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

void Player::CreateParticle()
{
	auto& scene = GET_SINGLE(SceneManager)->GetActiveScene();

	Vec3 playerPos = GetTransform()->GetLocalPosition();

	shared_ptr<GameObject> particle = make_shared<GameObject>();
	particle->SetName(L"Particle");
	particle->AddComponent(make_shared<Transform>());
	particle->AddComponent(make_shared<ParticleSystem>());
	particle->SetCheckFrustum(false);
	particle->GetTransform()->SetLocalPosition(playerPos);
	scene->AddGameObject(particle);
}

void Player::RemoveParticle()
{
	auto& scene = GET_SINGLE(SceneManager)->GetActiveScene();
	auto& gameObjects = scene->GetGameObjects();

	for (auto& gameObject : gameObjects)
	{
		if (gameObject->GetName() == L"Particle")
		{

		}
	}
}

void Player::Item_SpeedUp()
{
	// ���ǵ� ���� �� �ѹ��� �ϵ���
	if (_speed == _originalSpeed)
	{
		_state->End(*GetGameObject());
		delete _state;
		_state = new DashState;
		_state->Enter(*GetGameObject());
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
	// �ٸ� �÷��̾�� �ӵ� ���ҽ�Ű�� ���� �Լ�
	// �������� �ؾ��ϳ� ?
	// Ŭ�󿡼� �Լ��� ���� �ʿ� ������ ������
}

void Player::Item_Blind()
{
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

		_state->End(*GetGameObject());
		delete _state;
		_state = new StunState;
		_state->Enter(*GetGameObject());
	}
}
