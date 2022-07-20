#pragma once
#include "MonoBehaviour.h"

class Player : public MonoBehaviour
{
	enum STATE
	{
		IDLE, WALK, DASH, JUMP, ATTACK, END
	};

public:
	Player();
	virtual ~Player();

	virtual void LateUpdate() override;

private:
	void KeyCheck();
	void StateCheck();
	void AnimationCheck();

private:
	float	_speed = 10.f;
	float	_rotSpeed = 5.f;

	shared_ptr<GameObject>		_player = make_shared<GameObject>();
	shared_ptr<GameObject>		_camera = make_shared<GameObject>();
	shared_ptr<class CameraScript>	_cameraScript = make_shared<CameraScript>();

private:
	STATE	_curState = STATE::END;
	STATE	_preState = STATE::END;
};

