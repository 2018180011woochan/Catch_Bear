#pragma once
#include "MonoBehaviour.h"

// Ű���� �Է¿� ���� ī�޶� ��ġ�� �ٲ���
class TestCameraScript : public MonoBehaviour
{
public:
	TestCameraScript();
	virtual ~TestCameraScript();

	virtual void LateUpdate() override;

private:
	float		_speed = 100.f;
};

