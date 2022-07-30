#include "pch.h"
#include "CoolTime.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "Component.h"
#include "Resources.h"
#include "Timer.h"
#include "Transform.h"
#include "ItemSlotManager.h"
#include "ItemSlotUI.h"

CoolTime::CoolTime()
{
}

CoolTime::~CoolTime()
{
}

void CoolTime::Update()
{
	if (GetGameObject()->_isRender)
	{
		if (_fCoolTime >= 0.f)
		{
			_fCoolTime -= DELTA_TIME /** 0.8f*/;
			//float fScale = 1.f - (_fCoolTime / 5.f);
			float fScale = 2.f / _fCoolTime;

			Vec3	localScale = GetGameObject()->GetTransform()->GetLocalScale();
			float fPos = fScale / 2.f;
			Vec3	localPos = GetGameObject()->GetTransform()->GetLocalPosition();

			if (localScale.y - fScale >= 0.f) {
				GetGameObject()->GetTransform()->SetLocalScale(Vec3(localScale.x, localScale.y - fScale, localScale.z));
				GetGameObject()->GetTransform()->SetLocalPosition(Vec3(localPos.x, localPos.y - fPos, localPos.z));


			}
			else
			{
				GET_SINGLE(ItemSlotManager)->ResetItemSlot(_nSlot);

				_fCoolTime = 0.f; _fItemCoolTime = 0.f;
				_nSlot = -1;
				GetGameObject()->_isRender = false;
				GetGameObject()->GetTransform()->SetLocalScale(_localScale);
				GetGameObject()->GetTransform()->SetLocalPosition(_localPos);
			}
		}
		else
		{
			GET_SINGLE(ItemSlotManager)->ResetItemSlot(_nSlot);

			_fCoolTime = 0.f; _fItemCoolTime = 0.f;
			_nSlot = -1;
			GetGameObject()->_isRender = false;
			GetGameObject()->GetTransform()->SetLocalScale(_localScale);
			GetGameObject()->GetTransform()->SetLocalPosition(_localPos);

		}
	}
}

void CoolTime::LateUpdate()
{
}
 
void CoolTime::SetInitData(const Vec3& pos, const Vec3& scale, int nSlot, int nCoolTime)
{
	GetGameObject()->_isRender = true;
	_localPos = pos;
	_localScale = scale;
	_nSlot = nSlot;
	//_fItemCoolTime = float(nCoolTime);
	//_fCoolTime = float(nCoolTime);
	_fItemCoolTime = 5.f;
	_fCoolTime = 5.f;
}

void CoolTime::Reset()
{
	_fCoolTime = 0.f; _fItemCoolTime = 0.f;
	_nSlot = -1;
	GetGameObject()->_isRender = false;
	GetGameObject()->GetTransform()->SetLocalScale(_localScale);
	GetGameObject()->GetTransform()->SetLocalPosition(_localPos);
}
