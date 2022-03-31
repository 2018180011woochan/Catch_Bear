#include "pch.h"
#include "ItemManager.h"
#include "Item.h"
#include "Scene.h"
#include "Timer.h"
#include "GameObject.h"
#include "Transform.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "Resources.h"
#include "Scene.h"
#include "SceneManager.h"
#include "MeshData.h"

void ItemManager::Init()
{
	_itemArray = { _commonItemList, _uniqueItemList, _treasureList };

	_commonItemMesh = GET_SINGLE(Resources)->LoadFBX(L"present1.bin");	// common
	_uniqueItemMesh = GET_SINGLE(Resources)->LoadFBX(L"present4.bin");	// unique

	SetItemPosition();
}

void ItemManager::Update()
{
	CreateCommonItem();
	CreateUniqueItem();
	CreateTreasure();

	Collision_ItemToPlayer();
}

void ItemManager::LateUpdate()
{
}

void ItemManager::SetItemPosition()
{
	// ��ǥ ȸ�Ƕ� ���ϱ�
	float x = 0.f;
	float y = 0.f;

	for (int i = 0; i < 10; ++i)
	{
		_commonPosArray[i] = Vec3((float)(x + i * 20), y, 0.f);
	}

	for (int i = 0; i < 10; ++i)
	{
		_uniquePosArray[i] = Vec3((float)(x + i * 20), y, 30.f);
	}
}

void ItemManager::CreateCommonItem()
{
	// �Ϲ� ������ ����, 30�ʸ��� ��� ����(5��) �ϳ��� ���� -> �� 6�� ����
	_commonItemTimer += DELTA_TIME;

	if (_commonItemTimer >= _commonItemTime)	// test 5��
	{	
		for (int i = 0; i < 5; ++i)
		{
			vector<shared_ptr<GameObject>>	commonItem = _commonItemMesh->Instantiate();

			for (auto& item : commonItem)
			{
				item->SetName(L"CommonItem");
				item->SetCheckFrustum(false);

				// �ӽ� ��ǥ��
				if (_commonIndex > 9) _commonIndex = 0;
				Vec3 pos = _commonPosArray[_commonIndex++];
				item->GetTransform()->SetLocalPosition(pos);
				item->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));
				item->GetMeshRenderer()->GetMaterial()->SetInt(0, 0);
				item->_boundingExtents = XMFLOAT3(5.f, 5.f, 5.f);
				item->_boundingBox = BoundingOrientedBox(
					XMFLOAT3(0.0f, 0.0f, 0.0f), item->_boundingExtents, XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
				item->AddComponent(make_shared<Item>());

				// Item enum�� ���� - ItemType, ItemEffect
				static_pointer_cast<Item>(item->GetScript(0))->SetItemType(ITEM_TYPE::COMMON);
				static_pointer_cast<Item>(item->GetScript(0))->SetItemEffect((ITEM_EFFECT)i);
				_commonItemList.push_back(item);

				shared_ptr<Scene> scene = make_shared<Scene>();
				scene = GET_SINGLE(SceneManager)->GetActiveScene();
				scene->AddGameObject(item);
			}
		}
		_commonItemTimer = 0.f;
	}
}

void ItemManager::CreateUniqueItem()
{
	// ����ũ ������ ����, 1�и��� ��� ����(2) �ϳ��� -> �� 3�� ����
	_uniqueItemTimer += DELTA_TIME;

	if (_uniqueItemTimer >= _uniqueItemTime)	// test 10��
	{
		for (int i = 0; i < 2; ++i)
		{
			vector<shared_ptr<GameObject>>	uniqueItem = _uniqueItemMesh->Instantiate();

			for (auto& item : uniqueItem)
			{
				item->SetName(L"UniqueItem");
				item->SetCheckFrustum(false);

				// �ӽ� ��ǥ��
				if (_uniqueIndex > 9) _uniqueIndex = 0;
				Vec3 pos = _uniquePosArray[_uniqueIndex++];
				item->GetTransform()->SetLocalPosition(pos);
				item->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));
				item->GetMeshRenderer()->GetMaterial()->SetInt(0, 0);
				item->_boundingExtents = XMFLOAT3(5.f, 5.f, 5.f);
				item->_boundingBox = BoundingOrientedBox(
					XMFLOAT3(0.0f, 0.0f, 0.0f), item->_boundingExtents, XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
				item->AddComponent(make_shared<Item>());

				// Item enum�� ���� - ItemType, ItemEffect
				static_pointer_cast<Item>(item->GetScript(0))->SetItemType(ITEM_TYPE::UNIQUE);
				static_pointer_cast<Item>(item->GetScript(0))->SetItemEffect((ITEM_EFFECT)(i + 5));
				_uniqueItemList.push_back(item);

				shared_ptr<Scene> scene = make_shared<Scene>();
				scene = GET_SINGLE(SceneManager)->GetActiveScene();
				scene->AddGameObject(item);
			}
		}
		_uniqueItemTimer = 0.f;
	}

}

void ItemManager::CreateTreasure()
{
}

void ItemManager::Collision_ItemToPlayer()
{
	auto& gameObjects = GET_SINGLE(SceneManager)->GetActiveScene()->GetGameObjects();

	// �� ���� �÷��̾� ã��
	for (auto& gameObject : gameObjects)
	{
		if (gameObject->GetName() == L"Player")
		{
			_player = gameObject;
			break;
		}
	}

	// ������ �Ŵ����� ����Ʈ�� ��� �ִ� �����۵�� �÷��̾� �浹üũ
	for (auto item = _commonItemList.begin(); item != _commonItemList.end(); item++)
	{
		if ((*item)->_boundingBox.Intersects(_player->_boundingBox))
		{
			int a = 0;
		}

		// else item++;
	}

}
