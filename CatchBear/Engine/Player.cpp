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
	//PlayerState* state = _state->Update(*shared_from_this());

	//if (state != NULL)
	//{
	//	delete _state;
	//	_state = state;
	//	_state->Enter(*shared_from_this());
	//}
	////////////////////////////////////////////////////////////////////

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


	//////////////////////////////////////////////////////////////////////////
	// �� �κ��� ���� �÷����ϰ� �ִ� �÷��̾�� ����Ǿ� ��!!
	//State Check
	/*PlayerState* state = _state->KeyCheck(*shared_from_this());

	if (state != NULL)
	{
		delete _state;
		_state = state;
		_state->Enter(*shared_from_this());
	}*/
	//////////////////////////////////////////////////////////////////////////

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

	Vec3 pos = _player->GetTransform()->GetLocalPosition();
	Vec3 rot = _player->GetTransform()->GetLocalRotation();

	_player->_state = new IdleState();
	PlayerState* state = _player->_state->KeyCheck(*_player);

	if (state != NULL)
	{
		delete _player->_state;
		_player->_state = state;
		_player->_state->Enter(*_player);
	}

	state = _player->_state->Update(*_player);

	if (state != NULL)
	{
		delete _player->_state;
		_player->_state = state;
		_player->_state->Enter(*_player);
	}

	delete _player->_state;

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
		pkt.set_movedir(1);

		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
		mysession->Send(sendBuffer);
	}

	//else _curState = IDLE;

	// ȸ��
	float delta = 0.f;
	if (INPUT->GetButton(KEY_TYPE::RIGHT))
	{
		rot.y += DELTA_TIME * _rotSpeed;
		delta = DELTA_TIME * _rotSpeed;

		GetTransform()->SetLocalRotation(rot);
		
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

	if (INPUT->GetButton(KEY_TYPE::SPACE))
	{
		//_curState = JUMP;
	}


	//GetTransform()->SetLocalPosition(pos);
	_cameraScript->Revolve(delta, _player->GetTransform()->GetLocalPosition());
}