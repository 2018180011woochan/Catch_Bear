#include "pch.h"
#include "Scene.h"
#include "GameObject.h"
#include "Camera.h"
#include "Engine.h"
#include "ConstantBuffer.h"
#include "Light.h"
#include "Engine.h"
#include "Resources.h"
#include "ServerSession.h"
#include "Timer.h"
#include "MeshRenderer.h"
#include "ItemWindow.h"
#include "ItemSlotUI.h"
#include "Transform.h"
#include "ScoreManager.h"
#include "ItemManager.h"
#include "CollidManager.h"
#include "Input.h"
#include "Player.h"
#include "ServerPacketHandler.h"
#include "TagMark.h"
#include "SceneManager.h"
#include "SoundManager.h"


shared_ptr<class Camera> Scene::GetMainCamera()
{
	if (_cameras.empty())
		return nullptr;

	return _cameras[0];
}

void Scene::Render()
{
	PushLightData();	// light ����

	ClearRTV();		// Render Target ����

	//RenderShadow();	// shadow�� ���� ��� ����, light �������� ���ָ� ��

	RenderDeferred();	// Deferred ��ü�� �׷���

	RenderLights();

	RenderFinal();	// ���������� �����ϴ� �κ�

	RenderForward();
}

void Scene::ClearRTV()
{
	// SwapChain Group �ʱ�ȭ
	int8 backIndex = GEngine->GetSwapChain()->GetBackBufferIndex();
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->ClearRenderTargetView(backIndex);

	// Shadow Group �ʱ�ȭ
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->ClearRenderTargetView();

	// Deferred Group �ʱ�ȭ
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->ClearRenderTargetView();

	// Lighting Group �ʱ�ȭ
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->ClearRenderTargetView();
}

void Scene::RenderShadow()
{
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->OMSetRenderTargets();

	for (auto& light : _lights)
	{
		if (light->GetLightType() != LIGHT_TYPE::DIRECTIONAL_LIGHT)
			continue;

		light->RenderShadow();
	}

	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->WaitTargetToResource();
}

void Scene::RenderDeferred()
{
	// Deferred OMSet
	// Render_Deferred()���� �׷��� �ֵ��� �츮�� deferred ���̴��� �޾���
	// PS_OUT�� �ش��ϴ� ������, ���, �÷��� �̿��ؼ� ������ �ȴ�.
	// 3������ ����Ÿ���� ������, ���, �÷� ������ ������ �ȴ�.
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->OMSetRenderTargets();

	if (_cameras.size() == 0)
	{
		cout << "ī�޶� ������ 0\n";
		return;
	}

	// Render_Deferred()�� ���� ī�޶� �����Ѵ�.
	shared_ptr<Camera> mainCamera = _cameras[0];
	mainCamera->SortGameObject();
	mainCamera->Render_Deferred();

	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->WaitTargetToResource();
}

void Scene::RenderLights()
{
	// ���� ī�޶� �����ͼ� �� ��Ʈ������ �������� ��Ʈ������ �̾ƿ� ���� ���� �������ش�.
	// RenderLights()�� �ϱ� ���� RenderShadow()�� �ϴ� ����
	// RenderShadow()�� �츮�� ��ġ�� Light�� �ִ� ī�޶�� �� ��Ʈ������ �������� ��Ʈ������ ����� �ֱ� ������
	// �ٽ� ���������ش�.
	shared_ptr<Camera> mainCamera = _cameras[0];
	Camera::S_MatView = mainCamera->GetViewMatrix();
	Camera::S_MatProjection = mainCamera->GetProjectionMatrix();

	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->OMSetRenderTargets();

	// ������ �׸���.
	for (auto& light : _lights)
	{
		light->Render();
	}

	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->WaitTargetToResource();
}

void Scene::RenderFinal()
{
	// Swapchain OMSet
	int8 backIndex = GEngine->GetSwapChain()->GetBackBufferIndex();
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->OMSetRenderTargets(1, backIndex);

	GET_SINGLE(Resources)->Get<Material>(L"Final")->PushGraphicsData();
	GET_SINGLE(Resources)->Get<Mesh>(L"Rectangle")->Render();
}

void Scene::RenderForward()
{
	// ù��°�� ����� ���� ī�޶� ���۵� �������� �ϵ��� �����Ѵ�.
	// ���� ī�޶� Render_Deferred() �� Render_Forward()
	// ������ ī�޶�� Render_Forward()��
	shared_ptr<Camera> mainCamera = _cameras[0];
	mainCamera->Render_Forward();

	for (auto& camera : _cameras)
	{
		if (camera == mainCamera)
			continue;

		camera->SortGameObject();
		camera->Render_Forward();
	}
}

void Scene::PushLightData()
{
	// ������ ���ٶ� �� �����ӿ� �ѹ��� ����
	// Light�� �ϳ��� üũ�ϸ鼭 �� ������ �����´�
	// lightParams�� ������ �� �־������� �װ��� global constant buffer�� SetGraphicsGlobalData()�� �̿��ؼ�
	// ���� �־��ְ� �;��� ������ �о�־��ش�.

	LightParams lightParams = {};

	for (auto& light : _lights)
	{
		const LightInfo& lightInfo = light->GetLightInfo();

		light->SetLightIndex(lightParams.lightCount);

		lightParams.lights[lightParams.lightCount] = lightInfo;
		lightParams.lightCount++;
		lightParams.time = GET_SINGLE(Timer)->GetAllTime();
	}

	CONST_BUFFER(CONSTANT_BUFFER_TYPE::GLOBAL)->SetGraphicsGlobalData(&lightParams, sizeof(lightParams));
}

void Scene::AddGameObject(shared_ptr<GameObject> gameObject)
{
	if (gameObject->GetCamera() != nullptr)
	{
		_cameras.push_back(gameObject->GetCamera());
	}
	else if (gameObject->GetLight() != nullptr)
	{
		_lights.push_back(gameObject->GetLight());
	}

	_gameObjects.push_back(gameObject);
}

void Scene::AddPlayers(uint64 _playerid, shared_ptr<GameObject> gameObject)
{
	//_players[_playerid] = gameObject;
	_players.emplace(_playerid, gameObject);
}

void Scene::AddTagMarks(uint64 _objid, shared_ptr<GameObject> gameObject)
{
	_TagMarks.emplace(_objid, gameObject);
}

void Scene::AddVecPlayers(shared_ptr<GameObject> gameObject)
{
	_vecPlayers.push_back(gameObject);
}

void Scene::AddStaticObj(shared_ptr<GameObject> gameobject)
{
	_vecStaticObject.push_back(gameobject);
}

void Scene::AddItemCoolTime(shared_ptr<GameObject> gameobject)
{
	_vecItemCoolTimes.push_back(gameobject);
}

void Scene::RemoveGameObject(shared_ptr<GameObject> gameObject)
{
	if (gameObject->GetCamera())
	{
		auto findIt = std::find(_cameras.begin(), _cameras.end(), gameObject->GetCamera());
		if (findIt != _cameras.end())
			_cameras.erase(findIt);
	}
	else if (gameObject->GetLight())
	{
		auto findIt = std::find(_lights.begin(), _lights.end(), gameObject->GetLight());
		if (findIt != _lights.end())
			_lights.erase(findIt);
	}

	auto findIt = std::find(_gameObjects.begin(), _gameObjects.end(), gameObject);
	if (findIt != _gameObjects.end())
	{
		_gameObjects.erase(findIt);
	}
}

void Scene::RemoveItems()
{
	for (auto item = _gameObjects.begin(); item != _gameObjects.end(); )
	{
		if ((*item)->GetName() == L"CommonItem" || (*item)->GetName() == L"UniqueItem" || (*item)->GetName() == L"Treasure")
		{
			item = _gameObjects.erase(item);
			int a = 0;
		}

		else item++;
	}
}

shared_ptr<GameObject> Scene::GetGameObject(wstring name)
{
	for (int i = 0; i < _gameObjects.size(); ++i)
	{
		if (_gameObjects[i]->GetName() == name)
		{
			int k = 0;
			return _gameObjects[i];
		}
	}
	return NULL;
}

void Scene::AddCurTime(float _time)
{
	if (_curTime + _time <= 0) _curTime = 0.f;
	else _curTime += _time;
}
