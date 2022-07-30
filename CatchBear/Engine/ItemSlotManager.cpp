#include "pch.h"
#include "ItemSlotManager.h"
#include "Engine.h"
#include "ItemSlotUI.h"
#include "GameObject.h"
#include "Player.h"
#include "SceneManager.h"
#include "Scene.h"
#include "CoolTime.h"
#include "Transform.h"

void ItemSlotManager::SetItemSlot(int nSlot, shared_ptr<GameObject> slot)
{
	switch (nSlot)
	{
	case 1:
		_itemSlot1 = slot;
		break;
	case 2:
		_itemSlot2 = slot;
		break;
	case 3:
		_itemSlot3 = slot;
		break;
	}
}

void ItemSlotManager::AddItem(Item::ITEM_EFFECT itemType)
{
	if (static_pointer_cast<ItemSlotUI>(_itemSlot1->GetScript(0))->GetItem() == Item::ITEM_EFFECT::NONE)
	{
		static_pointer_cast<ItemSlotUI>(_itemSlot1->GetScript(0))->SetItem(itemType);
	}
	else if (static_pointer_cast<ItemSlotUI>(_itemSlot2->GetScript(0))->GetItem() == Item::ITEM_EFFECT::NONE)
	{
		static_pointer_cast<ItemSlotUI>(_itemSlot2->GetScript(0))->SetItem(itemType);
	}
	else if (static_pointer_cast<ItemSlotUI>(_itemSlot3->GetScript(0))->GetItem() == Item::ITEM_EFFECT::NONE)
	{
		static_pointer_cast<ItemSlotUI>(_itemSlot3->GetScript(0))->SetItem(itemType);
	}
}

void ItemSlotManager::AddItem_Cheat(Item::ITEM_EFFECT itemType)
{
	static_pointer_cast<ItemSlotUI>(_itemSlot1->GetScript(0))->SetItem(itemType);
}

void ItemSlotManager::UseItem(int nSlot)
{
	Vec3 pos, scale;
	float fCoolTime;

	Item::ITEM_EFFECT item;
	shared_ptr<GameObject>	coolTime;

	// slot->UseItem()
	// _settingItem = Item::ITEM_EFFECT::NONE;

	switch (nSlot)
	{
	case 1:
		item = static_pointer_cast<ItemSlotUI>(_itemSlot1->GetScript(0))->GetItem();
		

		if (item == Item::ITEM_EFFECT::BLIND)
		{
			static_pointer_cast<ItemSlotUI>(_itemSlot1->GetScript(0))->UseItem();
			ResetItemSlot(nSlot);
			return;
		}
		else if (item == Item::ITEM_EFFECT::SILENCE)
		{
			static_pointer_cast<ItemSlotUI>(_itemSlot1->GetScript(0))->UseItem();
			ResetItemSlot(nSlot);
			return;
		}
		else if (item == Item::ITEM_EFFECT::STUN)
		{
			static_pointer_cast<ItemSlotUI>(_itemSlot1->GetScript(0))->UseItem();
			ResetItemSlot(nSlot);
			return;
		}
		else if (item == Item::ITEM_EFFECT::SPEED_DOWN)
		{
			static_pointer_cast<ItemSlotUI>(_itemSlot1->GetScript(0))->UseItem();
			ResetItemSlot(nSlot);
			return;
		}
		else if (item == Item::ITEM_EFFECT::TELEPORT)
		{
			static_pointer_cast<ItemSlotUI>(_itemSlot1->GetScript(0))->UseItem();
			ResetItemSlot(nSlot);
			return;
		}
		else if (item == Item::ITEM_EFFECT::NONE) {
			int k = 0;
			static_pointer_cast<ItemSlotUI>(_itemSlot1->GetScript(0))->UseItem();
			ResetItemSlot(nSlot);
			return;
		}

		fCoolTime = static_pointer_cast<ItemSlotUI>(_itemSlot1->GetScript(0))->GetCoolTime();
		pos = _itemSlot1->GetTransform()->GetLocalPosition();
		scale = _itemSlot1->GetTransform()->GetLocalScale();
		//coolTime = GET_SINGLE(SceneManager)->GetActiveScene()->GetGameObject(L"ItemCoolTime1");
		coolTime = GET_SINGLE(SceneManager)->GetActiveScene()->GetItemCoolTimes()[0];
		static_pointer_cast<CoolTime>(coolTime->GetScript(0))->SetInitData(pos, scale, 1, (int)fCoolTime);
		break;

	case 2:
		item = static_pointer_cast<ItemSlotUI>(_itemSlot2->GetScript(0))->GetItem();
		

		if (item == Item::ITEM_EFFECT::BLIND)
		{
			static_pointer_cast<ItemSlotUI>(_itemSlot2->GetScript(0))->UseItem();
			ResetItemSlot(nSlot);
			return;
		}
		else if (item == Item::ITEM_EFFECT::SILENCE)
		{
			static_pointer_cast<ItemSlotUI>(_itemSlot2->GetScript(0))->UseItem();
			ResetItemSlot(nSlot);
			return;
		}
		else if (item == Item::ITEM_EFFECT::STUN)
		{
			static_pointer_cast<ItemSlotUI>(_itemSlot2->GetScript(0))->UseItem();
			ResetItemSlot(nSlot);
			return;
		}
		else if (item == Item::ITEM_EFFECT::SPEED_DOWN)
		{
			static_pointer_cast<ItemSlotUI>(_itemSlot2->GetScript(0))->UseItem();
			ResetItemSlot(nSlot);
			return;
		}
		else if (item == Item::ITEM_EFFECT::TELEPORT)
		{
			static_pointer_cast<ItemSlotUI>(_itemSlot2->GetScript(0))->UseItem();
			ResetItemSlot(nSlot);
			return;
		}
		else if (item == Item::ITEM_EFFECT::NONE) {
			int k = 0;
			static_pointer_cast<ItemSlotUI>(_itemSlot2->GetScript(0))->UseItem();
			ResetItemSlot(nSlot);
			return;
		}

		fCoolTime = static_pointer_cast<ItemSlotUI>(_itemSlot2->GetScript(0))->GetCoolTime();
		pos = _itemSlot2->GetTransform()->GetLocalPosition();
		scale = _itemSlot2->GetTransform()->GetLocalScale();
		//static_pointer_cast<CoolTime>(GET_SINGLE(SceneManager)->GetActiveScene()->GetGameObject(L"ItemCoolTime2")->GetScript(0))
		//	->SetInitData(pos, scale, 2, (int)fCoolTime);
		coolTime = GET_SINGLE(SceneManager)->GetActiveScene()->GetItemCoolTimes()[1];
		static_pointer_cast<CoolTime>(coolTime->GetScript(0))->SetInitData(pos, scale, 2, (int)fCoolTime);
		break;

	case 3:
		item = static_pointer_cast<ItemSlotUI>(_itemSlot3->GetScript(0))->GetItem();
		

		if (item == Item::ITEM_EFFECT::BLIND)
		{
			static_pointer_cast<ItemSlotUI>(_itemSlot3->GetScript(0))->UseItem();
			ResetItemSlot(nSlot);
			return;
		}
		else if (item == Item::ITEM_EFFECT::SILENCE)
		{
			static_pointer_cast<ItemSlotUI>(_itemSlot3->GetScript(0))->UseItem();
			ResetItemSlot(nSlot);
			return;
		}
		else if (item == Item::ITEM_EFFECT::STUN)
		{
			static_pointer_cast<ItemSlotUI>(_itemSlot3->GetScript(0))->UseItem();
			ResetItemSlot(nSlot);
			return;
		}
		else if (item == Item::ITEM_EFFECT::SPEED_DOWN)
		{
			static_pointer_cast<ItemSlotUI>(_itemSlot3->GetScript(0))->UseItem();
			ResetItemSlot(nSlot);
			return;
		}
		else if (item == Item::ITEM_EFFECT::TELEPORT)
		{
			static_pointer_cast<ItemSlotUI>(_itemSlot3->GetScript(0))->UseItem();
			ResetItemSlot(nSlot);
			return;
		}
		else if (item == Item::ITEM_EFFECT::NONE) {
			int k = 0;
			static_pointer_cast<ItemSlotUI>(_itemSlot3->GetScript(0))->UseItem();
			ResetItemSlot(nSlot);
			return;
		}

		fCoolTime = static_pointer_cast<ItemSlotUI>(_itemSlot3->GetScript(0))->GetCoolTime();
		pos = _itemSlot3->GetTransform()->GetLocalPosition();
		scale = _itemSlot3->GetTransform()->GetLocalScale();
		//static_pointer_cast<CoolTime>(GET_SINGLE(SceneManager)->GetActiveScene()->GetGameObject(L"ItemCoolTime3")->GetScript(0))
		//	->SetInitData(pos, scale, 3, (int)fCoolTime);
		coolTime = GET_SINGLE(SceneManager)->GetActiveScene()->GetItemCoolTimes()[2];
		static_pointer_cast<CoolTime>(coolTime->GetScript(0))->SetInitData(pos, scale, 3, (int)fCoolTime);
		break;
	}
}

void ItemSlotManager::ResetItemSlot(int nSlot)
{
	switch (nSlot)
	{
	case 1:
		static_pointer_cast<ItemSlotUI>(_itemSlot1->GetScript(0))->ResetItemSlot();
		break;
	case 2:
		static_pointer_cast<ItemSlotUI>(_itemSlot2->GetScript(0))->ResetItemSlot();
		break;
	case 3:
		static_pointer_cast<ItemSlotUI>(_itemSlot3->GetScript(0))->ResetItemSlot();
		break;
	}
}

void ItemSlotManager::UseShieldItem()
{
	// 아이템 슬롯에서도 제거, 쿨타임 렌더링도 끝내기
	int k = 0;
	if (static_pointer_cast<ItemSlotUI>(_itemSlot1->GetScript(0))->GetItem() == Item::ITEM_EFFECT::SHIELD)
	{
		static_pointer_cast<ItemSlotUI>(_itemSlot1->GetScript(0))->ResetItemSlot();
		ResetItemSlot(1);
		shared_ptr<GameObject> coolTime = GET_SINGLE(SceneManager)->GetActiveScene()->GetItemCoolTimes()[0];
		static_pointer_cast<CoolTime>(coolTime->GetScript(0))->Reset();
	}
	else if (static_pointer_cast<ItemSlotUI>(_itemSlot2->GetScript(0))->GetItem() == Item::ITEM_EFFECT::SHIELD)
	{
		static_pointer_cast<ItemSlotUI>(_itemSlot2->GetScript(0))->ResetItemSlot();
		ResetItemSlot(2);
		shared_ptr<GameObject> coolTime = GET_SINGLE(SceneManager)->GetActiveScene()->GetItemCoolTimes()[1];
		static_pointer_cast<CoolTime>(coolTime->GetScript(0))->Reset();
	}
	else if (static_pointer_cast<ItemSlotUI>(_itemSlot3->GetScript(0))->GetItem() == Item::ITEM_EFFECT::SHIELD)
	{
		static_pointer_cast<ItemSlotUI>(_itemSlot3->GetScript(0))->ResetItemSlot();
		ResetItemSlot(3);
		shared_ptr<GameObject> coolTime = GET_SINGLE(SceneManager)->GetActiveScene()->GetItemCoolTimes()[2];
		static_pointer_cast<CoolTime>(coolTime->GetScript(0))->Reset();
	}
}

void ItemSlotManager::IsSilenced(bool isSilenced)
{
	shared_ptr<GameObject> silenceEffect1 = GET_SINGLE(SceneManager)->GetActiveScene()->GetGameObject(L"ItemSlot1_Silence");
	if (isSilenced) silenceEffect1->_isRender = true;
	else silenceEffect1->_isRender = false;

	shared_ptr<GameObject> silenceEffect2 = GET_SINGLE(SceneManager)->GetActiveScene()->GetGameObject(L"ItemSlot2_Silence");
	if (isSilenced) silenceEffect2->_isRender = true;
	else silenceEffect2->_isRender = false;

	shared_ptr<GameObject> silenceEffect3 = GET_SINGLE(SceneManager)->GetActiveScene()->GetGameObject(L"ItemSlot3_Silence");
	if (isSilenced) silenceEffect3->_isRender = true;
	else silenceEffect3->_isRender = false;
}

