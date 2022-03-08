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
	float		_speed = 100.f;
	float		_offset = 10.f;
	float		_angleX = 0.5f;

	//shared_ptr<GameObject> _testPlayer;
};

