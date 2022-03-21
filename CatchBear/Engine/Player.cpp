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

Player::Player()
{
}

Player::~Player()
{
}

void Player::LateUpdate()
{
	KeyCheck();
}

void Player::KeyCheck()
{
	Vec3 pos = GetTransform()->GetLocalPosition();
	Vec3 rot = GetTransform()->GetLocalRotation();

	// �̵�
	if (INPUT->GetButton(KEY_TYPE::W))
		pos += GetTransform()->GetLook() * _speed * DELTA_TIME;

	if (INPUT->GetButton(KEY_TYPE::S))
		pos -= GetTransform()->GetLook() * _speed * DELTA_TIME;

	// ȸ��
	if (INPUT->GetButton(KEY_TYPE::A))
	{
		rot.y -= DELTA_TIME * _rotSpeed;
		GetTransform()->SetLocalRotation(rot);
	}

	if (INPUT->GetButton(KEY_TYPE::D))
	{
		rot.y += DELTA_TIME * _rotSpeed;
		GetTransform()->SetLocalRotation(rot);
	}


	//if (INPUT->GetButton(KEY_TYPE::A))
	//	pos -= GetTransform()->GetRight() * _speed * DELTA_TIME;

	//if (INPUT->GetButton(KEY_TYPE::D))
	//	pos += GetTransform()->GetRight() * _speed * DELTA_TIME;

	GetTransform()->SetLocalPosition(pos);

	// ���� ������ ī�޶�(Main_Camera)�� �����´�.
	// ī�޶� ������ �ִ� ��ũ��Ʈ(CameraScript)���� Followlayer()�� �����Ų��.

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

	shared_ptr<CameraScript> cameraScript = make_shared<CameraScript>();
	cameraScript = static_pointer_cast<CameraScript>(_camera->GetScript(0));
	cameraScript->GetTransform()->SetParent(_player->GetTransform());
	cameraScript->FollowPlayer(_player);
}
