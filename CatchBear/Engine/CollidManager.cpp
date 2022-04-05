#include "pch.h"
#include "CollidManager.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "Scene.h"
#include "Transform.h"

void CollidManager::Update()
{
	ColiisionPlayerToStaticObj();
}

void CollidManager::ColiisionPlayerToStaticObj()
{
	auto& gameObjects = GET_SINGLE(SceneManager)->GetActiveScene()->GetGameObjects();
	shared_ptr<GameObject>	_player = make_shared<GameObject>();

	// �� ���� �÷��̾� ã��
	for (auto& gameObject : gameObjects)
	{
		if (gameObject->GetName() == L"Player")
		{
			_player = gameObject;
			break;
		}
	}

	auto& staticObjects = GET_SINGLE(SceneManager)->GetActiveScene()->GetStaticObj();

	// ������ �Ŵ����� ����Ʈ�� ��� �ִ� �����۵�� �÷��̾� �浹üũ
	for (auto mapobj = staticObjects.begin(); mapobj != staticObjects.end(); mapobj++)
	{
		if ((*mapobj)->_boundingBox.Intersects(_player->_boundingBox))
		{
			Vec3 lk = _player->GetTransform()->GetLook();
			_player->SetIsAllowPlayerMove(false);
			break;
		}
		else
			_player->SetIsAllowPlayerMove(true);

	}
}
