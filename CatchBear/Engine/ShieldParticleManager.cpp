#include "pch.h"
#include "ShieldParticleManager.h"
#include "ShieldParticle.h"
#include "Timer.h"
#include "GameObject.h"
#include "MeshData.h"
#include "Resources.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Scene.h"
#include "SceneManager.h"

void ShieldParticleManager::Init()
{
	_shieldMesh = GET_SINGLE(Resources)->LoadFBX(L"present1.bin");
}

void ShieldParticleManager::Update()
{
	if (_isCreate) CreateParticles();

	else	// delay
	{
		_curTime += DELTA_TIME;
		if (_curTime >= _delayTime)
		{
			_isCreate = true;
			_curTime = 0.f;
		}
	}

	DeleteParticles();
}

void ShieldParticleManager::CreateParticles()
{
	if (!_shieldParticleOn) return;

	for (int i = 0; i < 5; ++i)
	{
		vector<shared_ptr<GameObject>> particle = _shieldMesh->Instantiate();

		random_device rd;
		uniform_real_distribution<float> x(_playerPos.x - _dis, _playerPos.x + _dis);
		uniform_real_distribution<float> z(_playerPos.z - _dis, _playerPos.z + _dis);
		float disX = _playerPos.x - x(rd);
		float disZ = _playerPos.z - z(rd);
		Vec3 pos = Vec3(x(rd), _playerPos.y, z(rd));

		for (auto& gameObject : particle)
		{
			gameObject->SetName(L"ShieldParticle");
			gameObject->SetCheckFrustum(false);
			gameObject->GetTransform()->SetLocalPosition(pos);
			gameObject->GetTransform()->SetLocalRotation(Vec3(0.f, 0.f, 0.f));
			gameObject->GetTransform()->SetLocalScale(Vec3(0.2f, 0.2f, 0.2f));
			gameObject->GetMeshRenderer()->GetMaterial()->SetInt(0, 0);
			gameObject->AddComponent(make_shared<ShieldParticle>());
			static_pointer_cast<ShieldParticle>(gameObject->GetScript(0))->SetStartPos(pos);
			static_pointer_cast<ShieldParticle>(gameObject->GetScript(0))->SetPlayerDis(disX, disZ);
			_listParticles.push_back(gameObject);
			GET_SINGLE(SceneManager)->GetActiveScene()->AddGameObject(gameObject);
		}

		if (i == 2) _isCreate = false;
	}

}

void ShieldParticleManager::DeleteParticles()
{
	if (!_shieldParticleOn)
	{
		_isCreate = false;
		_curTime = 0.f;
		_shieldTime = 0.f;

		for (auto p = _listParticles.begin(); p != _listParticles.end();)
		{
			GET_SINGLE(SceneManager)->GetActiveScene()->RemoveGameObject(*p);
			p = _listParticles.erase(p);
		}

		_listParticles.clear();
	}

	// Life Time 끝난 파티클 삭제
	for (auto p = _listParticles.begin(); p != _listParticles.end();)
	{
		if (static_pointer_cast<ShieldParticle>((*p)->GetScript(0))->GetDead())
		{
			GET_SINGLE(SceneManager)->GetActiveScene()->RemoveGameObject(*p);
			p = _listParticles.erase(p);
		}

		else p++;
	}
}

void ShieldParticleManager::SetShieldParticleOff()
{
	_shieldParticleOn = false;
}
