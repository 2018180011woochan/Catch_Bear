#pragma once
#include "MonoBehaviour.h"

// �÷��̾�� ������ ī�޶�
class CameraScript : public MonoBehaviour
{
public:
	CameraScript();
	virtual ~CameraScript();

	virtual void LateUpdate() override;

private:
	float		_speed = 5.f;
	float		_offset = 300.f;
	float		_angleX = 0.5f;

	bool		_getPlayer = false;

	shared_ptr<GameObject> _testPlayer;
};

