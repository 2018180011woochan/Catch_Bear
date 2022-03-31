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
#include "ServerPacketHandler.h"
#include "Session.h"
#include "ServerSession.h"

Player::Player()
{
}

Player::~Player()
{
}

void Player::LateUpdate()
{
	KeyCheck();
	StateCheck();
	AnimationCheck();

	GetAnimationController()->AdvanceTime(DELTA_TIME);
	GetTransform()->UpdateTransform(NULL);
	GetAnimationController()->SetWorldMatrix();
}

void Player::KeyCheck()
{
	Protocol::C_MOVE pkt;

	// 게임종료
	if (INPUT->GetButtonDown(KEY_TYPE::ESC))
		::PostQuitMessage(0);

	Vec3 pos = GetTransform()->GetLocalPosition();
	Vec3 rot = GetTransform()->GetLocalRotation();

	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
	const vector<shared_ptr<GameObject>>& gameObjects = scene->GetGameObjects();

	//_player = GetGameObject();
	for (auto& gameObject : gameObjects)
	{
		if (gameObject->GetName() == L"Player")
		{
			_player = gameObject;
			break;
		}
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
		pos += GetTransform()->GetLook() * _speed * DELTA_TIME;
		_curState = WALK;

		// 서버로 MOVE패킷 전송
		pkt.set_xpos(pos.x);
		pkt.set_ypos(pos.y);
		pkt.set_zpos(pos.z);
		pkt.set_playerid(mysession->GetPlayerID());
		pkt.set_movedir(0);

		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
		mysession->Send(sendBuffer);
	}

	else if (INPUT->GetButton(KEY_TYPE::DOWN))
	{
		pos -= GetTransform()->GetLook() * _speed * DELTA_TIME;
		_curState = WALK;

		// 서버로 MOVE패킷 전송
		pkt.set_xpos(pos.x);
		pkt.set_ypos(pos.y);
		pkt.set_zpos(pos.z);
		pkt.set_playerid(mysession->GetPlayerID());
		pkt.set_movedir(1);

		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
		mysession->Send(sendBuffer);
	}

	else _curState = IDLE;

	// 회전
	float delta = 0.f;
	if (INPUT->GetButton(KEY_TYPE::RIGHT))
	{
		rot.y += DELTA_TIME * _rotSpeed;
		delta = DELTA_TIME * _rotSpeed;

		GetTransform()->SetLocalRotation(rot);
		
		// 이동+회전: WALK
		if (_curState != WALK)	_curState = IDLE;
	}

	if (INPUT->GetButton(KEY_TYPE::LEFT))
	{
		rot.y -= DELTA_TIME * _rotSpeed;
		delta = -DELTA_TIME * _rotSpeed;

		GetTransform()->SetLocalRotation(rot);

		// 이동+회전: WALK
		if (_curState != WALK)	_curState = IDLE;
	}

	if (INPUT->GetButton(KEY_TYPE::SPACE))
	{
		_curState = JUMP;
	}
	
	

	//GetTransform()->SetLocalPosition(pos);
	_cameraScript->Revolve(delta, GetTransform()->GetLocalPosition());
}

void Player::StateCheck()
{
	if (_curState != _preState)
	{
		switch (_curState)
		{
		case Player::IDLE:
			GetAnimationController()->SetTrackAnimationSet(0, 0);
			break;
		case Player::WALK:
			GetAnimationController()->SetTrackAnimationSet(0, 1);
			break;
		case Player::DASH:
			GetAnimationController()->SetTrackAnimationSet(0, 3);
			break;
		case Player::JUMP:
			GetAnimationController()->SetTrackAnimationSet(0, 2);
			break;
		case Player::ATTACK:
			GetAnimationController()->SetTrackAnimationSet(0, 4);
			break;
		case Player::END:
			break;
		default:
			break;
		}

		_preState = _curState;
	}
}

void Player::AnimationCheck()
{
	if (_curState == DASH || _curState == JUMP || _curState == ATTACK)
	{
		if (GetAnimationController()->IsAnimationFinish(0))
			_curState = IDLE;
	}
}
