#include "pch.h"
#include "Leaf.h"
#include "Transform.h"
#include "Engine.h"
#include "Timer.h"
#include "GameObject.h"

Leaf::Leaf()
{
}

Leaf::~Leaf()
{
}

void Leaf::Start()
{
	random_device rd;
	uniform_real_distribution<float> distSpeed(0.8, 2);
	uniform_real_distribution<float> distTime(1, 5);
	_fallSpeed = distSpeed(rd);
	_respawnTime = distTime(rd);
}

void Leaf::LateUpdate()
{
	Vec3 pos = GetTransform()->GetLocalPosition();

	if (GetGameObject()->_isRender)
	{
		pos.y -= _fallSpeed * DELTA_TIME;
		GetTransform()->SetLocalPosition(pos);

		if (pos.y <= 0.5f)
		{
			GetGameObject()->_isRender = false;
			pos.y = 5.f;
			GetTransform()->SetLocalPosition(pos);
		}
	}
	else
	{
		_respawnTime += DELTA_TIME;
		if (_respawnTime >= 2.f)
		{
			_respawnTime = 0.f;
			GetGameObject()->_isRender = true;
		}
	}

}
