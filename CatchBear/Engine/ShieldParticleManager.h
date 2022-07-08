#pragma once

class MeshData;
class GameObject;

class ShieldParticleManager
{
	DECLARE_SINGLE(ShieldParticleManager);

public:
	void Init();
	void Update();
	void LateUpdate();

public:
	void UpdatePlayerPos(Vec3 playerPos) { _playerPos = playerPos; }
	void CreateParticles();
	void DeleteParticles();

public:
	void SetShieldParticleOn() { _shieldParticleOn = true; }
	void SetShieldParticleOff() { _shieldParticleOn = false; }

public:
	const Vec3 GetPlayerPos() { return _playerPos; }

private:
	shared_ptr<MeshData> _shieldMesh;

	list<shared_ptr<GameObject>> _listParticles;

	Vec3	_playerPos;
	float	_dis = 0.8f;
	bool	_shieldParticleOn = false;
	bool	_isCreate = true;
	float	_shieldTime = 5.f;
	float	_delayTime = 1.f;
	float	_curTime = 0.f;
};

