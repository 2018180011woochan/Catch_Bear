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
	int k = 0;
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

void ItemSlotManager::UseItem(int nSlot)
{
	Vec3 pos, scale;
	float fCoolTime;

	Item::ITEM_EFFECT item;
	shared_ptr<GameObject>	coolTime;

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
		else if (item == Item::ITEM_EFFECT::DEBUFF_OFF) 
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
		else if (item == Item::ITEM_EFFECT::DEBUFF_OFF)
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

		fCoolTime = static_pointer_cast<ItemSlotUI>(_itemSlot1->GetScript(0))->GetCoolTime();
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
		else if (item == Item::ITEM_EFFECT::DEBUFF_OFF)
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

		fCoolTime = static_pointer_cast<ItemSlotUI>(_itemSlot1->GetScript(0))->GetCoolTime();
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
	// ������ ���Կ����� ����, ��Ÿ�� �������� ������

	if (static_pointer_cast<ItemSlotUI>(_itemSlot1->GetScript(0))->GetItem() == Item::ITEM_EFFECT::SHIELD)
	{
		static_pointer_cast<ItemSlotUI>(_itemSlot1->GetScript(0))->UseItem();
		ResetItemSlot(1);
		GET_SINGLE(SceneManager)->GetActiveScene()->GetItemCoolTimes()[0]->_isRender = false;
	}
	else if (static_pointer_cast<ItemSlotUI>(_itemSlot2->GetScript(0))->GetItem() == Item::ITEM_EFFECT::SHIELD)
	{
		static_pointer_cast<ItemSlotUI>(_itemSlot2->GetScript(0))->UseItem();
		ResetItemSlot(2);
		GET_SINGLE(SceneManager)->GetActiveScene()->GetItemCoolTimes()[1]->_isRender = false;
	}
	else if (static_pointer_cast<ItemSlotUI>(_itemSlot3->GetScript(0))->GetItem() == Item::ITEM_EFFECT::SHIELD)
	{
		static_pointer_cast<ItemSlotUI>(_itemSlot3->GetScript(0))->UseItem();
		ResetItemSlot(3);
		GET_SINGLE(SceneManager)->GetActiveScene()->GetItemCoolTimes()[2]->_isRender = false;
	}
}

