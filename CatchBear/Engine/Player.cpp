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

Player::Player()
{
	// 서버에서 컨트롤하는 플레이어는 _state 갖고있으면 안됨
	_state = new IdleState();
}

Player::~Player()
{
}

void Player::LateUpdate()
{
	// 서버에서 컨트롤하는 플레이어는 서버에서 위치값도 받아오니까 필요없을듯
	KeyCheck();

	////////////////////////////////////////////////////////////////////
	// 이 부분은 직접 플레이하고 있는 플레이어에만 적용되야 함!!
	PlayerState* state = _state->Update(*shared_from_this());

	if (state != NULL)
	{
		_state->End(*shared_from_this());
		delete _state;
		_state = state;
		_state->Enter(*shared_from_this());
	}
	////////////////////////////////////////////////////////////////////

	Vec3 pos = GetTransform()->GetLocalPosition();
	printf("%f, %f, %f\n", pos.x, pos.y, pos.z);

	// 애니메이션 재생하는 부분 -> 모두 적용되야 함
	GetAnimationController()->AdvanceTime(DELTA_TIME);
	GetTransform()->UpdateTransform(NULL);
	GetAnimationController()->SetWorldMatrix();
	
}

void Player::AddPlayerItem(shared_ptr<GameObject> item)
{
	// 플레이어가 갖고있을 수 있는 최대 아이템 수는 3개
	// 아이템을 3개 지니고 있으면 맨처음 아이템 삭제
	if (_playerItem.size() < 3)
	{
		_playerItem.push_back(item);
		int a = 0;
	}

	//auto findIt = std::find(_gameObjects.begin(), _gameObjects.end(), gameObject);
	//if (findIt != _gameObjects.end())
	//{
	//	_gameObjects.erase(findIt);
	//}
}

void Player::KeyCheck()
{
	// 게임종료
	if (INPUT->GetButtonDown(KEY_TYPE::ESC))
		::PostQuitMessage(0);


	//////////////////////////////////////////////////////////////////////////
	// 이 부분은 직접 플레이하고 있는 플레이어에만 적용되야 함!!
	// State Check
	PlayerState* state = _state->KeyCheck(*shared_from_this());

	if (state != NULL)
	{
		delete _state;
		_state = state;
		_state->Enter(*shared_from_this());
	}
	//////////////////////////////////////////////////////////////////////////



	Vec3 pos = GetTransform()->GetLocalPosition();
	Vec3 rot = GetTransform()->GetLocalRotation();

	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
	const vector<shared_ptr<GameObject>>& gameObjects = scene->GetGameObjects();

	_player = GetGameObject();

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
		pos += GetTransform()->GetLook() * _speed * DELTA_TIME;

	else if (INPUT->GetButton(KEY_TYPE::DOWN))
		pos -= GetTransform()->GetLook() * _speed * DELTA_TIME;

	// 회전
	float delta = 0.f;
	if (INPUT->GetButton(KEY_TYPE::RIGHT))
	{
		rot.y += DELTA_TIME * _rotSpeed;
		delta = DELTA_TIME * _rotSpeed;

		GetTransform()->SetLocalRotation(rot);
	}

	if (INPUT->GetButton(KEY_TYPE::LEFT))
	{
		rot.y -= DELTA_TIME * _rotSpeed;
		delta = -DELTA_TIME * _rotSpeed;

		GetTransform()->SetLocalRotation(rot);
	}

	GetTransform()->SetLocalPosition(pos);
	_cameraScript->Revolve(delta, GetTransform()->GetLocalPosition());
}