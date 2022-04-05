#pragma once
#include "PlayerState.h"

class MoveState : public PlayerState
{
public:
	MoveState() {}

public:
	virtual PlayerState* KeyCheck(Player& player);
	virtual PlayerState* Update(Player& player);
	virtual void Enter(Player& player);
	virtual void End(Player& player);

private:
	float	_fSpeedDownTime = 0.f;
	float	_fSlowSpeed = 5.f;
};

