#pragma once
#include "MonoBehaviour.h"
class GameObject;

class Leaf : public MonoBehaviour
{
public:
	Leaf();
	virtual ~Leaf();

public:
	virtual void Start() override;
	virtual void LateUpdate() override;

public:
	void SetPrePosition(const Vec3& pos) { _prePosition = pos; }

private:
	shared_ptr<GameObject>	_leaf = make_shared<GameObject>();
	Vec3	_prePosition = Vec3(0.f, 0.f, 0.f);
	float	_fallSpeed = 0.f;
	float	_respawnTime = 0.f;
	bool	_isLive = true;
};

