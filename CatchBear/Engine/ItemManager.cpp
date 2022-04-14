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
#include "Player.h"

#include "ServerSession.h"

void ItemManager::Init()
{
	_itemArray = { _commonItemList, _uniqueItemList, _treasureList };

	_commonItemMesh = GET_SINGLE(Resources)->LoadFBX(L"present1.bin");	// common
	_uniqueItemMesh = GET_SINGLE(Resources)->LoadFBX(L"present4.bin");	// unique
	_treasureMesh = GET_SINGLE(Resources)->LoadFBX(L"Diamond.bin");		// treasure

	SetItemPosition();
}

void ItemManager::Update()
{
	CreateCommonItem();
	//CreateUniqueItem();
	//CreateTreasure();


	Collision_ItemToPlayer();
}

void ItemManager::LateUpdate()
{
}

void ItemManager::SetItemPosition()
{
	float y = -2.f;		// y���� �� �Ȱ�

#pragma region ItemPos
	// ��ǥ ������ ������Ѵ�. ������ ������ ���߿� ,, ~
	_itemPosArray[0] = Vec3(-20.f, y, -29.f);
	_itemPosArray[1] = Vec3(-32.f, y, 12.f);
	_itemPosArray[2] = Vec3(18.f, y, -24.f);
	_itemPosArray[3] = Vec3(-2.f, y, -34.f);
	_itemPosArray[4] = Vec3(-8.f, y, -14.f);

	_itemPosArray[5] = Vec3(-19.f, y, -3.f);
	_itemPosArray[6] = Vec3(-30.f, y, -38.f);
	_itemPosArray[7] = Vec3(-18.f, y, -38.f);
	_itemPosArray[8] = Vec3(25.f, y, 12.f);
	_itemPosArray[9] = Vec3(18.f, y, -8.f);

	_itemPosArray[10] = Vec3(4.f, y, -12.f);
	_itemPosArray[11] = Vec3(1.5f, y, -21.f);
	_itemPosArray[12] = Vec3(-19.f, y, -41.f);
	_itemPosArray[13] = Vec3(-15.f, y, 16.f);
	_itemPosArray[14] = Vec3(10.f, y, 17.f);

	_itemPosArray[15] = Vec3(-8.f, y, 7.f);
	_itemPosArray[16] = Vec3(-30.f, y, -6.f);
	_itemPosArray[17] = Vec3(-3.f, y, -24.f);
	_itemPosArray[18] = Vec3(14.f, y, -35.f);
	_itemPosArray[19] = Vec3(18.f, y, -24.f);

	_itemPosArray[20] = Vec3(-32.f, y, 20.f);
	_itemPosArray[21] = Vec3(-34.f, y, 3.f);
	_itemPosArray[22] = Vec3(20.f, y, -24.f);
	_itemPosArray[23] = Vec3(26.f, y, 15.f);
	_itemPosArray[24] = Vec3(13.f, y, 18.f);

	_itemPosArray[25] = Vec3(5.f, y, 3.f);
	_itemPosArray[26] = Vec3(5.f, y, -39.f);
	_itemPosArray[27] = Vec3(-8.f, y, 1.f);
	_itemPosArray[28] = Vec3(26.f, y, -24.f);
	_itemPosArray[29] = Vec3(-14.f, y, -42.f);
#pragma endregion
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

				// ��ǥ ��̿��� ��ǥ�� ��������
				if (_itemIndex > _maxItemIndex) _itemIndex = 0;
				Vec3 pos = _itemPosArray[_itemIndex++];

				item->GetTransform()->SetLocalPosition(pos);
				item->GetTransform()->SetLocalScale(Vec3(2.f, 2.f, 2.f));
				item->GetMeshRenderer()->GetMaterial()->SetInt(0, 0);

				item->SetBoundingExtents(XMFLOAT3(0.5f, 0.5f, 0.5f));
				item->SetBoundingBox(BoundingOrientedBox(
					XMFLOAT3(0.0f, 0.0f, 0.0f), item->GetBoundingExtents(), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)));

				//item->_boundingExtents = XMFLOAT3(0.5f, 0.5f, 0.5f);
				//item->_boundingBox = BoundingOrientedBox(
				//	XMFLOAT3(0.0f, 0.0f, 0.0f), item->_boundingExtents, XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

				item->AddComponent(make_shared<Item>());

				// Item enum�� ���� - ItemType, ItemEffect
				static_pointer_cast<Item>(item->GetScript(0))->SetItemType(ITEM_TYPE::COMMON);
				static_pointer_cast<Item>(item->GetScript(0))->SetItemEffect((ITEM_EFFECT)i);
				//static_pointer_cast<Item>(item->GetScript(0))->SetItemEffect(ITEM_EFFECT::STUN);
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

				// ������ ��̿��� ��ǥ�� ��������
				if (_itemIndex > _maxItemIndex) _itemIndex = 0;
				Vec3 pos = _itemPosArray[_itemIndex++];

				item->GetTransform()->SetLocalPosition(pos);
				item->GetTransform()->SetLocalScale(Vec3(2.f, 2.f, 2.f));
				item->GetMeshRenderer()->GetMaterial()->SetInt(0, 0);

				item->SetBoundingExtents(XMFLOAT3(0.5f, 0.5f, 0.5f));
				item->SetBoundingBox(BoundingOrientedBox(
					XMFLOAT3(0.0f, 0.0f, 0.0f), item->GetBoundingExtents(), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)));


				//item->_boundingExtents = XMFLOAT3(0.5f, 0.5f, 0.5f);
				//item->_boundingBox = BoundingOrientedBox(
				//	XMFLOAT3(0.0f, 0.0f, 0.0f), item->_boundingExtents, XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

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
	// ���� ����, 1�и��� �ϳ��� ���� -> �� 3�� ����
	_treasureTimer += DELTA_TIME;

	if (_treasureTimer >= _treasureTime)	// test 10��
	{
		vector<shared_ptr<GameObject>>	treasure = _treasureMesh->Instantiate();

		for (auto& gameObject : treasure)
		{
			gameObject->SetName(L"Treasure");
			gameObject->SetCheckFrustum(false);

			// ������ ��̿��� ��ǥ�� ��������
			if (_itemIndex > _maxItemIndex) _itemIndex = 0;
			Vec3 pos = _itemPosArray[_itemIndex++];

			gameObject->GetTransform()->SetLocalPosition(pos);
			gameObject->GetTransform()->SetLocalRotation(Vec3(-90.f, 270.f, 0.f));
			gameObject->GetTransform()->SetLocalScale(Vec3(0.5f, 0.5f, 0.5f));
			gameObject->GetMeshRenderer()->GetMaterial()->SetInt(0, 0);

			gameObject->SetBoundingExtents(XMFLOAT3(0.5f, 0.5f, 0.5f));
			gameObject->SetBoundingBox(BoundingOrientedBox(
				XMFLOAT3(0.0f, 0.0f, 0.0f), gameObject->GetBoundingExtents(), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)));


			//gameObject->_boundingExtents = XMFLOAT3(0.5f, 0.5f, 0.5f);
			//gameObject->_boundingBox = BoundingOrientedBox(
			//	XMFLOAT3(0.0f, 0.0f, 0.0f), gameObject->_boundingExtents, XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

			gameObject->AddComponent(make_shared<Item>());

			// Item enum�� ���� - ItemType, ItemEffect
			static_pointer_cast<Item>(gameObject->GetScript(0))->SetItemType(ITEM_TYPE::TRESURE);
			static_pointer_cast<Item>(gameObject->GetScript(0))->SetItemEffect((ITEM_EFFECT)(7));
			_treasureList.push_back(gameObject);


			shared_ptr<Scene> scene = make_shared<Scene>();
			scene = GET_SINGLE(SceneManager)->GetActiveScene();
			scene->AddGameObject(gameObject);
		}
		_treasureTimer = 0.f;
	}
}

void ItemManager::Collision_ItemToPlayer()
{
	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
	const vector<shared_ptr<GameObject>>& gameObjects = scene->GetGameObjects();

	if (mysession == NULL)
		return;

	// �� ���� �÷��̾� ã��
	for (auto& gameObject : gameObjects)
	{
		if (gameObject->GetName() == L"Player" && gameObject->GetPlayerID() == mysession->GetPlayerID())
		{
			_player = gameObject;
			break;
		}
	}

	// common item & player
	for (auto item = _commonItemList.begin(); item != _commonItemList.end();)
	{
		if ((*item)->GetBoundingBox().Intersects(_player->GetBoundingBox()))
		{
			// �÷��̾�� ������ �߰� �� ItemManager�� ItemList������ ����, �� ���� gameObject ���Ϳ����� ����
			static_pointer_cast<Player>(_player->GetScript(0))->AddPlayerItem(*item);			
			scene->RemoveGameObject(*item);
			item = _commonItemList.erase(item);
		}

		else item++;
	}

	// unique item & player
	for (auto item = _uniqueItemList.begin(); item != _uniqueItemList.end();)
	{
		if ((*item)->GetBoundingBox().Intersects(_player->GetBoundingBox()))
		{
			// ItemManager�� ItemList������ ����, �� ���� gameObject ���Ϳ����� ����
			scene->RemoveGameObject(*item);
			item = _uniqueItemList.erase(item);
		}

		else item++;
	}
}
