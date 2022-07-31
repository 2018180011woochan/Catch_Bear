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
#include "SoundManager.h"

void ItemManager::Init()
{
	_commonItemMesh = GET_SINGLE(Resources)->LoadFBX(L"present1.bin");	// common
	_uniqueItemMesh = GET_SINGLE(Resources)->LoadFBX(L"present4.bin");	// unique
	_treasureMesh = GET_SINGLE(Resources)->LoadFBX(L"Diamond.bin");		// treasure

	SetItemPosition();
}

void ItemManager::Update()
{
	CreateCommonItem();
	CreateUniqueItem();
	CreateTreasure();

	// 타이머 돌아갈 때 부터 체크해야 함
	// stage->isTimerStart 일 때부터
	if (GET_SINGLE(SceneManager)->GetActiveScene()->GetIsStart())
	{
		Collision_ItemToPlayer();
	}

}

void ItemManager::SetItemPosition()
{
	float y = 1.f;		// y값은 다 똑같

#pragma region ItemPos
	_itemPosArray[0] = Vec3(-23.f, y, -3.f);
	_itemPosArray[1] = Vec3(21.f, y, -12.f);
	_itemPosArray[2] = Vec3(-8.f, y, 43.f);
	_itemPosArray[3] = Vec3(18.f, y, 44.f);
	_itemPosArray[4] = Vec3(18.f, y, 0.f);

	_itemPosArray[5] = Vec3(38.f, y, -36.f);
	_itemPosArray[6] = Vec3(-23.f, y, 24.f);
	_itemPosArray[7] = Vec3(0.f, y, -40.f);
	_itemPosArray[8] = Vec3(-8.f, y, 25.f);
	_itemPosArray[9] = Vec3(20.f, y, -9.f);

	_itemPosArray[10] = Vec3(35.f, y, 35.f);
	_itemPosArray[11] = Vec3(-45.f, y, -20.f);
	_itemPosArray[12] = Vec3(21.f, y, -36.f);
	_itemPosArray[13] = Vec3(-15.f, y, 1.f);
	_itemPosArray[14] = Vec3(34.f, y, 13.f);

	_itemPosArray[15] = Vec3(8.f, y, 16.f);
	_itemPosArray[16] = Vec3(-28.f, y, -40.f);
	_itemPosArray[17] = Vec3(-40.f, y, 15.f);
	_itemPosArray[18] = Vec3(10.f, y, 8.f);
	_itemPosArray[19] = Vec3(-25.f, y, -5.f);

	_itemPosArray[20] = Vec3(27.f, y, 20.f);
	_itemPosArray[21] = Vec3(22.f, y, -7.f);
	_itemPosArray[22] = Vec3(-1.8f, y, 8.2f);
	_itemPosArray[23] = Vec3(5.f, y, 17.f);
	_itemPosArray[24] = Vec3(-10.f, y, -11.f);

	_itemPosArray[25] = Vec3(12.f, y, -43.f);
	_itemPosArray[26] = Vec3(0.f, y, 5.f);
	_itemPosArray[27] = Vec3(22.f, y, 35.f);
	_itemPosArray[28] = Vec3(-25.f, y, 45.f);
	_itemPosArray[29] = Vec3(-18.f, y, 18.f);

	_itemPosArray[30] = Vec3(-1.47f, y, -4.8f);
	_itemPosArray[31] = Vec3(4.5f, y, -14.7f);
	_itemPosArray[32] = Vec3(22.77f, y, 3.9f);
	_itemPosArray[33] = Vec3(-25.f, y, 3.8f);
	_itemPosArray[34] = Vec3(0.6f, y, -6.7f);

	_itemPosArray[35] = Vec3(0.1f, y, 22.6f);
	_itemPosArray[36] = Vec3(14.8f, y, 23.7f);
	_itemPosArray[37] = Vec3(22.5f, y, 9.1f);
	_itemPosArray[38] = Vec3(11.7f, y, -14.3f);
	_itemPosArray[39] = Vec3(1.8f, y, 22.f);
#pragma endregion
}

void ItemManager::CreateCommonItem()
{
	// 일반 아이템 생성, 30초마다 모든 종류(5개) 하나씩 생성 -> 총 6번 생성
	_commonItemTimer += DELTA_TIME;

	if (_commonItemTimer >= _commonItemTime)	// test 5초
	{	
		for (int i = 0; i < 5; ++i)
		{
			vector<shared_ptr<GameObject>>	commonItem = _commonItemMesh->Instantiate();

			for (auto& item : commonItem)
			{
				item->SetName(L"CommonItem");
				item->SetCheckFrustum(false);

				// 좌표 어레이에서 좌표값 꺼내오기
				if (_itemIndex > _maxItemIndex) _itemIndex = 0;
				Check_ItemPos(_itemIndex);
				Vec3 pos = _itemPosArray[_itemIndex++];

				item->GetTransform()->SetLocalPosition(pos);
				item->GetTransform()->SetLocalScale(Vec3(2.f, 2.f, 2.f));
				item->GetMeshRenderer()->GetMaterial()->SetInt(0, 0);

				item->SetBoundingExtents(XMFLOAT3(0.5f, 0.5f, 0.5f));
				item->SetBoundingBox(BoundingOrientedBox(
					XMFLOAT3(0.0f, 0.0f, 0.0f), item->GetBoundingExtents(), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)));
				item->AddComponent(make_shared<Item>());

				// Item enum값 설정 - ItemType, ItemEffect
				static_pointer_cast<Item>(item->GetScript(0))->SetItemType(Item::ITEM_TYPE::COMMON);
				static_pointer_cast<Item>(item->GetScript(0))->SetItemEffect((Item::ITEM_EFFECT)i);
				//static_pointer_cast<Item>(item->GetScript(0))->SetItemEffect(Item::ITEM_EFFECT::SILENCE);
				_commonItemList.push_back(item);
			
				GET_SINGLE(SceneManager)->GetActiveScene()->AddGameObject(item);
			}
		}
		_commonItemTimer = 0.f;
	}
}

void ItemManager::CreateUniqueItem()
{
	// 유니크 아이템 생성, 1분마다 모든 종류(2) 하나씩 -> 총 3번 생성
	_uniqueItemTimer += DELTA_TIME;

	if (_uniqueItemTimer >= _uniqueItemTime)	// test 10초
	{
		for (int i = 0; i < 2; ++i)
		{
			vector<shared_ptr<GameObject>>	uniqueItem = _uniqueItemMesh->Instantiate();

			for (auto& item : uniqueItem)
			{
				item->SetName(L"UniqueItem");
				item->SetCheckFrustum(false);

				// 아이템 어레이에서 좌표값 꺼내오기
				if (_itemIndex > _maxItemIndex) _itemIndex = 0;
				Check_ItemPos(_itemIndex);
				Vec3 pos = _itemPosArray[_itemIndex++];

				item->GetTransform()->SetLocalPosition(pos);
				item->GetTransform()->SetLocalScale(Vec3(2.f, 2.f, 2.f));
				item->GetMeshRenderer()->GetMaterial()->SetInt(0, 0);

				item->SetBoundingExtents(XMFLOAT3(0.5f, 0.5f, 0.5f));
				item->SetBoundingBox(BoundingOrientedBox(
					XMFLOAT3(0.0f, 0.0f, 0.0f), item->GetBoundingExtents(), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)));
				item->AddComponent(make_shared<Item>());

				// Item enum값 설정 - ItemType, ItemEffect
				static_pointer_cast<Item>(item->GetScript(0))->SetItemType(Item::ITEM_TYPE::UNIQUE);
				static_pointer_cast<Item>(item->GetScript(0))->SetItemEffect((Item::ITEM_EFFECT)(i + 5));
				//static_pointer_cast<Item>(item->GetScript(0))->SetItemEffect(Item::ITEM_EFFECT::STUN);
				_uniqueItemList.push_back(item);

				GET_SINGLE(SceneManager)->GetActiveScene()->AddGameObject(item);
			}
		}
		_uniqueItemTimer = 0.f;
	}

}

void ItemManager::CreateTreasure()
{
	// 보물 생성, 1분마다 하나씩 생성 -> 총 3번 생성
	_treasureTimer += DELTA_TIME;

	if (_treasureTimer >= _treasureTime)	// test 10초
	{
		vector<shared_ptr<GameObject>>	treasure = _treasureMesh->Instantiate();

		for (auto& gameObject : treasure)
		{
			gameObject->SetName(L"Treasure");
			gameObject->SetCheckFrustum(false);

			// 아이템 어레이에서 좌표값 꺼내오기
			if (_itemIndex > _maxItemIndex) _itemIndex = 0;
			Check_ItemPos(_itemIndex);
			Vec3 pos = _itemPosArray[_itemIndex++];

			gameObject->GetTransform()->SetLocalPosition(pos);
			gameObject->GetTransform()->SetLocalRotation(Vec3(-90.f, 270.f, 0.f));
			gameObject->GetTransform()->SetLocalScale(Vec3(0.5f, 0.5f, 0.5f));
			gameObject->GetMeshRenderer()->GetMaterial()->SetInt(0, 0);

			gameObject->SetBoundingExtents(XMFLOAT3(0.5f, 0.5f, 0.5f));
			gameObject->SetBoundingBox(BoundingOrientedBox(
				XMFLOAT3(0.0f, 0.0f, 0.0f), gameObject->GetBoundingExtents(), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)));
			gameObject->AddComponent(make_shared<Item>());

			// Item enum값 설정 - ItemType, ItemEffect
			static_pointer_cast<Item>(gameObject->GetScript(0))->SetItemType(Item::ITEM_TYPE::TRESURE);
			static_pointer_cast<Item>(gameObject->GetScript(0))->SetItemEffect(Item::ITEM_EFFECT::TREASURE);
			_treasureList.push_back(gameObject);

			GET_SINGLE(SceneManager)->GetActiveScene()->AddGameObject(gameObject);
		}
		_treasureTimer = 0.f;
	}
}

void ItemManager::Reset()
{
	// 아이템 생성 타이머
	_commonItemTimer = 0.f;
	_uniqueItemTimer = 0.f;
	_treasureTimer = 0.f;

	// 아이템 좌표 인덱스 변수
	_itemIndex = 0;

	auto& scene = GET_SINGLE(SceneManager)->GetActiveScene();

	// 씬에 있는 아이템 삭제
	for (auto cItem = _commonItemList.begin(); cItem != _commonItemList.end();)
	{
		scene->RemoveGameObject(*cItem);
		cItem = _commonItemList.erase(cItem);
	}

	for (auto uItem = _uniqueItemList.begin(); uItem != _uniqueItemList.end();)
	{
		scene->RemoveGameObject(*uItem);
		uItem = _uniqueItemList.erase(uItem);
	}
	for (auto t = _treasureList.begin(); t != _treasureList.end();)
	{
		scene->RemoveGameObject(*t);
		t = _treasureList.erase(t);
	}

	// 아이템 컨테이너 비우기
	_commonItemList.clear();
	_uniqueItemList.clear();
	_treasureList.clear();
}

void ItemManager::Collision_ItemToPlayer()
{
	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();

	if (mysession == NULL)
		return;

	const vector<shared_ptr<GameObject>>& vecPlayers = scene->GetVecPlayers();
	for (int i = 0; i < g_EnterPlayerCnt; ++i)
	{
		_player = vecPlayers[i];
		// common item & player
		for (auto item = _commonItemList.begin(); item != _commonItemList.end();)
		{
			if ((*item)->GetBoundingBox().Intersects(_player->GetBoundingBox()))
			{
				if (_player->GetPlayerID() == mysession->GetPlayerID())
				{
					Item::ITEM_EFFECT itemEffect = static_pointer_cast<Item>((*item)->GetScript(0))->GetItemEffect();
					static_pointer_cast<Player>(_player->GetScript(0))->AddPlayerItem(itemEffect);
				}
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
				if (_player->GetPlayerID() == mysession->GetPlayerID())
				{
					Item::ITEM_EFFECT itemEffect = static_pointer_cast<Item>((*item)->GetScript(0))->GetItemEffect();
					static_pointer_cast<Player>(_player->GetScript(0))->AddPlayerItem(itemEffect);
				}
				scene->RemoveGameObject(*item);
				item = _uniqueItemList.erase(item);
			}

			else item++;
		}


		// treasure & player
		for (auto treasure = _treasureList.begin(); treasure != _treasureList.end();)
		{
			if ((*treasure)->GetBoundingBox().Intersects(_player->GetBoundingBox()))
			{
				if (!_player->GetIsTagger() &&
					_player->GetPlayerID() == mysession->GetPlayerID())
				{
					static_pointer_cast<Player>(_player->GetScript(0))->AddPlayerScore(30);

					// 효과음
					GET_SINGLE(SoundManager)->PlaySound(L"treasure.mp3", SoundManager::CHANNELID::TREAUSE);
				}
				scene->RemoveGameObject(*treasure);
				treasure = _treasureList.erase(treasure);
			}

			else treasure++;
		}

	}
}

void ItemManager::Check_ItemPos(int itemIndex)
{
	// 생성될 좌표에 이미 아이템이 있는지 체크하는 함수

	auto& scene = GET_SINGLE(SceneManager)->GetActiveScene();

	// CommonItemList
	for (auto& cItem = _commonItemList.begin(); cItem != _commonItemList.end();/* ++cItem*/)
	{
		if ((*cItem)->GetTransform()->GetLocalPosition() == _itemPosArray[itemIndex])
		{
			scene->RemoveGameObject(*cItem);
			cItem = _commonItemList.erase(cItem);
			return;
		}

		else cItem++;
	}

	// UniqueItemList
	for (auto& uItem = _uniqueItemList.begin(); uItem != _uniqueItemList.end();/* ++uItem*/)
	{
		if ((*uItem)->GetTransform()->GetLocalPosition() == _itemPosArray[itemIndex])
		{
			scene->RemoveGameObject(*uItem);
			uItem = _uniqueItemList.erase(uItem);
			return;
		}

		else uItem++;
	}

	// TeasuerList
	for (auto& t = _treasureList.begin(); t != _treasureList.end();/* ++treasure*/)
	{
		if ((*t)->GetTransform()->GetLocalPosition() == _itemPosArray[itemIndex])
		{
			scene->RemoveGameObject(*t);
			t = _treasureList.erase(t);
			return;
		}

		else t++;
	}
}
