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

Player::Player()
{
	// �������� ��Ʈ���ϴ� �÷��̾�� _state ���������� �ȵ�
	_player->_state = new IdleState();
}

Player::~Player()
{
}

void Player::LateUpdate()
{
	// �������� ��Ʈ���ϴ� �÷��̾�� �������� ��ġ���� �޾ƿ��ϱ� �ʿ������
	KeyCheck();

	////////////////////////////////////////////////////////////////////
	// �� �κ��� ���� �÷����ϰ� �ִ� �÷��̾�� ����Ǿ� ��!!
	//PlayerState* state = _player->_state->Update(*shared_from_this());

	//if (state != NULL)
	//{
	//	_player->_state->End(*shared_from_this());
	//	delete _player->_state;
	//	_player->_state = state;
	//	_player->_state->Enter(*shared_from_this());
	//}
	////////////////////////////////////////////////////////////////////

	Vec3 pos = GetTransform()->GetLocalPosition();
	printf("%f, %f, %f\n", pos.x, pos.y, pos.z);

	// �ִϸ��̼� ����ϴ� �κ� -> ��� ����Ǿ� ��
	GetAnimationController()->AdvanceTime(DELTA_TIME);
	GetTransform()->UpdateTransform(NULL);
	GetAnimationController()->SetWorldMatrix();
	
}

void Player::KeyCheck()
{
	Protocol::C_MOVE pkt;

	// ��������
	if (INPUT->GetButtonDown(KEY_TYPE::ESC))
		::PostQuitMessage(0);

	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
	const vector<shared_ptr<GameObject>>& gameObjects = scene->GetGameObjects();

	if (mysession == NULL)
		return;

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
	//PlayerState* state = _player->_state->KeyCheck(*shared_from_this());

	//if (state != NULL)
	//{
	//	delete _player->_state;
	//	_player->_state = state;
	//	_player->_state->Enter(*shared_from_this());
	//}
	//////////////////////////////////////////////////////////////////////////

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

	//GetTransform()->SetLocalPosition(pos);
	_cameraScript->Revolve(delta, _player->GetTransform()->GetLocalPosition());
}