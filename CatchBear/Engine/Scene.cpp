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
	PushLightData();	// light 세팅

	ClearRTV();		// Render Target 정리

	RenderShadow();	// shadow는 순서 상관 없음, light 이전에만 해주면 됨

	RenderDeferred();	// Deferred 물체들 그려줌

	RenderLights();

	RenderFinal();	// 최종적으로 취합하는 부분

	RenderForward();
}

void Scene::ClearRTV()
{
	// SwapChain Group 초기화
	int8 backIndex = GEngine->GetSwapChain()->GetBackBufferIndex();
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->ClearRenderTargetView(backIndex);

	// Shadow Group 초기화
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->ClearRenderTargetView();

	// Deferred Group 초기화
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->ClearRenderTargetView();

	// Lighting Group 초기화
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
	// Render_Deferred()에서 그려질 애들은 우리가 deferred 셰이더로 받아준
	// PS_OUT에 해당하는 포지션, 노멀, 컬러를 이용해서 세팅이 된다.
	// 3가지의 렌더타겟이 포지션, 노멀, 컬러 값으로 세팅이 된다.
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->OMSetRenderTargets();

	// Render_Deferred()는 메인 카메라만 수행한다.
	shared_ptr<Camera> mainCamera = _cameras[0];
	mainCamera->SortGameObject();
	mainCamera->Render_Deferred();

	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->WaitTargetToResource();
}

void Scene::RenderLights()
{
	// 메인 카메라를 가져와서 뷰 매트릭스와 프로젝션 매트릭스를 뽑아온 다음 각각 설정해준다.
	// RenderLights()를 하기 전에 RenderShadow()를 하는 순간
	// RenderShadow()는 우리가 배치한 Light에 있는 카메라로 뷰 매트릭스와 프로젝션 매트릭스를 덮어쓰고 있기 때문에
	// 다시 복원시켜준다.
	shared_ptr<Camera> mainCamera = _cameras[0];
	Camera::S_MatView = mainCamera->GetViewMatrix();
	Camera::S_MatProjection = mainCamera->GetProjectionMatrix();

	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->OMSetRenderTargets();

	// 광원을 그린다.
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
	// 첫번째로 등록한 메인 카메라만 디퍼드 렌더링을 하도록 유도한다.
	// 메인 카메라만 Render_Deferred() 후 Render_Forward()
	// 나머지 카메라는 Render_Forward()만
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
	// 렌더링 해줄때 한 프레임에 한번씩 수행
	// Light를 하나씩 체크하면서 빛 정보를 꺼내온다
	// lightParams에 정보를 다 넣어줬으면 그것을 global constant buffer에 SetGraphicsGlobalData()를 이용해서
	// 내가 넣어주고 싶었던 정보를 밀어넣어준다.

	LightParams lightParams = {};

	for (auto& light : _lights)
	{
		const LightInfo& lightInfo = light->GetLightInfo();

		light->SetLightIndex(lightParams.lightCount);

		lightParams.lights[lightParams.lightCount] = lightInfo;
		lightParams.lightCount++;
		lightParams.time = DELTA_TIME;
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
