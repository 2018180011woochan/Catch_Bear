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

Player::Player()
{
	// �������� ��Ʈ���ϴ� �÷��̾�� _state ���������� �ȵ�
	_state = new IdleState();
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


	Vec3 pos = GetTransform()->GetLocalPosition();
	printf("%f, %f, %f\n", pos.x, pos.y, pos.z);

	// �ִϸ��̼� ����ϴ� �κ� -> ��� ����Ǿ� ��
	GetAnimationController()->AdvanceTime(DELTA_TIME);
	GetTransform()->UpdateTransform(NULL);
	GetAnimationController()->SetWorldMatrix();
	
}

void Player::KeyCheck()
{
	// ��������
	if (INPUT->GetButtonDown(KEY_TYPE::ESC))
		::PostQuitMessage(0);


	//////////////////////////////////////////////////////////////////////////
	// �� �κ��� ���� �÷����ϰ� �ִ� �÷��̾�� ����Ǿ� ��!!
	/// State Check
	//PlayerState* state = _state->KeyCheck(*shared_from_this());

	//if (state != NULL)
	//{
	//	delete _state;
	//	_state = state;
	//	_state->Enter(*shared_from_this());
	//}
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
	
	// �̵�
	if (INPUT->GetButton(KEY_TYPE::UP))
	{
		pos += GetTransform()->GetLook() * _speed * DELTA_TIME;
		//_curState = WALK;
	}

	else if (INPUT->GetButton(KEY_TYPE::DOWN))
	{
		pos -= GetTransform()->GetLook() * _speed * DELTA_TIME;
		//_curState = WALK;
	}

	//else _curState = IDLE;

	// ȸ��
	float delta = 0.f;
	if (INPUT->GetButton(KEY_TYPE::RIGHT))
	{
		rot.y += DELTA_TIME * _rotSpeed;
		delta = DELTA_TIME * _rotSpeed;

		GetTransform()->SetLocalRotation(rot);
		
		// �̵�+ȸ��: WALK
		//if (_curState != WALK)	_curState = IDLE;
	}

	if (INPUT->GetButton(KEY_TYPE::LEFT))
	{
		rot.y -= DELTA_TIME * _rotSpeed;
		delta = -DELTA_TIME * _rotSpeed;

		GetTransform()->SetLocalRotation(rot);

		// �̵�+ȸ��: WALK
		//if (_curState != WALK)	_curState = IDLE;
	}

	if (INPUT->GetButton(KEY_TYPE::SPACE))
	{
		//_curState = JUMP;
	}


	GetTransform()->SetLocalPosition(pos);
	_cameraScript->Revolve(delta, GetTransform()->GetLocalPosition());
}