#include "pch.h"
#include "ItemSlotUI.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "Component.h"
#include "Resources.h"
#include "Timer.h"
#include "Transform.h"

ItemSlotUI::ItemSlotUI()
{
	_settingItem = Item::ITEM_EFFECT::NONE;
}

ItemSlotUI::~ItemSlotUI()
{
}

void ItemSlotUI::Update()
{
}

void ItemSlotUI::LateUpdate()
{
}

void ItemSlotUI::SetItem(Item::ITEM_EFFECT item)
{
	_settingItem = item;
	wstring texPath;
	_isSetting = true;

	switch (item)
	{
	case Item::ITEM_EFFECT::SPEED_UP:
		_texName = L"speed_up";
		_fCoolTime = 5.f;
		break;
	case Item::ITEM_EFFECT::TELEPORT:
		_texName = L"teleport";
		break;
	case Item::ITEM_EFFECT::SHIELD:
		_texName = L"shield";
		_fCoolTime = 5.f;
		break;
	case Item::ITEM_EFFECT::SPEED_DOWN:
		_texName = L"speed_down";
		_fCoolTime = 5.f;
		break;
	case Item::ITEM_EFFECT::BLIND:
		_texName = L"blind";
		_fCoolTime = 5.f;
		break;
	case Item::ITEM_EFFECT::SILENCE:
		_texName = L"silence";
		break;
	case Item::ITEM_EFFECT::STUN:
		_texName = L"stun";
		_fCoolTime = 3.f;
		break;
	}
	
	texPath = L"..\\Resources\\Texture\\item\\" + _texName + L".png";
	shared_ptr<Texture> texItem = GET_SINGLE(Resources)->Load<Texture>(_texName, texPath);

	// 텍스처 바꿔줘야 함
	GetGameObject()->GetMeshRenderer()->GetMaterial()->SetTexture(0, texItem);
}

void ItemSlotUI::UseItem()
{
	// 사용한 아이템 종류를 확인한 후, 제한시간 설정하고
	//CheckItem();
	// 세팅된 아이템은 초기화
	//_settingItem = Item::ITEM_EFFECT::ITEM_EFFECT_END;
	_settingItem = Item::ITEM_EFFECT::NONE;

	//// before
	//shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"itemSlot", L"..\\Resources\\Texture\\item_slot.png");
	//GetGameObject()->GetMeshRenderer()->GetMaterial()->SetTexture(0, texture);
}

void ItemSlotUI::CheckItem()
{
	switch (_settingItem)
	{
	case Item::ITEM_EFFECT::SPEED_UP:
		//GetGameObject()->_isRender = false;
		break;
	case Item::ITEM_EFFECT::TELEPORT:
		break;
	case Item::ITEM_EFFECT::SHIELD:
		//GetGameObject()->_isRender = false;
		break;
	case Item::ITEM_EFFECT::SPEED_DOWN:
		//GetGameObject()->_isRender = false;
		break;
	case Item::ITEM_EFFECT::BLIND:
		//GetGameObject()->_isRender = false;
		break;
	case Item::ITEM_EFFECT::SILENCE:
		break;
	case Item::ITEM_EFFECT::STUN:
		//GetGameObject()->_isRender = false;
		break;
	}
}

void ItemSlotUI::ResetItemSlot()
{
	_fCoolTime = 0.f;
	_texName = L" ";
	_isSetting = false;
	_settingItem = Item::ITEM_EFFECT::NONE;
	shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"itemSlot", L"..\\Resources\\Texture\\item_slot.png");
	GetGameObject()->GetMeshRenderer()->GetMaterial()->SetTexture(0, texture);
}
