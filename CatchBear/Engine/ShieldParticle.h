#pragma once

#include "MonoBehaviour.h"

class ShieldParticle : public MonoBehaviour
{
public:
	ShieldParticle();
	virtual ~ShieldParticle();

public:
	virtual void LateUpdate() override;

public:
	const bool GetDead() { return _isDead; }

public:
	void SetStartPos(Vec3 pos) { _startPos = pos; }
	void SetPlayerDis(float disX, float disZ) { _disX = disX; _disZ = disZ; }
	void SetShieldEnd(bool value) { _isShieldEnd = value; }
	void UpdatePlayerPos(Vec3 playerPos) { _playerPos = playerPos; }

private:
	Vec3	_startPos;
	Vec3	_playerPos;
	float	_speed;
	float	_lifeTime = 0.f;
	float	_curTime = 0.f;
	bool	_isDead = false;
	bool	_isShieldEnd = false;

	float	_disX;
	float	_disZ;
};

