#pragma once
#include "MonoBehaviour.h"
class GameObject;

class TagMark : public MonoBehaviour
{
public:
	TagMark();
	virtual ~TagMark();

public:
	virtual void Update() override;
	virtual void LateUpdate() override;

public:
	void SetRotationSpeed(float fSpeed) { _fRotationSpeed = fSpeed; }
	void SetPosition(Vec3 pos);
	void SetRotation();

private:
	shared_ptr<GameObject>	_tagMark = make_shared<GameObject>();
	shared_ptr<GameObject>	_target = make_shared<GameObject>();
	float _fRotationSpeed = 5.f;
	Vec3	_localPos = Vec3(0.f, 0.f, 0.f);
};

