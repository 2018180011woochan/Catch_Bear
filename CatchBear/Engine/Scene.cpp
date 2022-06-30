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

void Scene::Awake()
{
	// 순회만 한다
	// shared_ptr: 어떤 물체를 복사하게 되면 내부적으로 RC가 1이 증가된다
	// shared_ptr이라고 해도 &로 받게 되면 그 RC가 1 증가시키지 않는다
	// gameObject를 복사해서 다른 곳으로 넘기지 않을거니까 
	// 복사하기보다는 &로 들고 있으면 성능적으로 더 좋은 향상을 얻을 수 있다
	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		gameObject->Awake();
	}
}

void Scene::Start()
{
	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		gameObject->Start();
	}

}

void Scene::Update()
{
	if (GET_SINGLE(SceneManager)->getSceneID() == LOGIN)
	{
		UpdateLoginScene();
	}
	else if (GET_SINGLE(SceneManager)->getSceneID() == STAGE)
	{
		UpdateStageScene();
	}
}

void Scene::LateUpdate()
{
	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		gameObject->LateUpdate();
	}
}

void Scene::FinalUpdate()
{
	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		gameObject->FinalUpdate();
	}
}

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

	//RenderShadow();	// shadow는 순서 상관 없음, light 이전에만 해주면 됨

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

//void Scene::RenderShadow()
//{
//	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->OMSetRenderTargets();
//
//	for (auto& light : _lights)
//	{
//		if (light->GetLightType() != LIGHT_TYPE::DIRECTIONAL_LIGHT)
//			continue;
//
//		light->RenderShadow();
//	}
//
//	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->WaitTargetToResource();
//}

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
	//shared_ptr<Camera> mainCamera = _cameras[0];
	//Camera::S_MatView = mainCamera->GetViewMatrix();
	//Camera::S_MatProjection = mainCamera->GetProjectionMatrix();

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
		lightParams.time = GET_SINGLE(Timer)->GetAllTime();
	}

	CONST_BUFFER(CONSTANT_BUFFER_TYPE::GLOBAL)->SetGraphicsGlobalData(&lightParams, sizeof(lightParams));
}

void Scene::UpdateLoginScene()
{
}

void Scene::UpdateStageScene()
{
	CheckTagger();

	if (_isStart)
	{
		_toStartTime += DELTA_TIME;
		if (_toStartTime >= 7.f)
		{
			CheckMouse();
			GET_SINGLE(Input)->Update();
			GET_SINGLE(CollidManager)->Update();

			if (!_isFinish)
			{
				SetTimer();
				GET_SINGLE(ItemManager)->Update();
				GET_SINGLE(ScoreManager)->Update();
				GET_SINGLE(CollidManager)->Update();
			}

			for (const shared_ptr<GameObject>& gameObject : _gameObjects)
			{
				gameObject->Update();
			}

		}
	}
}

void Scene::SetTimer()
{
	Protocol::C_PLAYERINFO pkt;

	if (_FinalPlayerEnter)
	{
		_curTime = 0;
		_FinalPlayerEnter = false;
	}

	shared_ptr<GameObject> mTimer = GetGameObject(L"minuteTimer");
	shared_ptr<GameObject> tTimer = GetGameObject(L"tenSecond");
	shared_ptr<GameObject> oTimer = GetGameObject(L"oneSecond");
	shared_ptr<Texture> textureMinute, textureTenSec, textureOneSec;
	
	_curTime += DELTA_TIME;

	float time = 180.0f - _curTime;
	if (_curTime >= 180.0f)
		int k = 0;

	// minuteTimer
	int minute = (int)(_curTime / 60.f);
	if (minute == 0)
		textureMinute = GET_SINGLE(Resources)->Load<Texture>(L"timer2", L"..\\Resources\\Texture\\timer\\timer2.png");
	else if (minute == 1)
		textureMinute = GET_SINGLE(Resources)->Load<Texture>(L"timer1", L"..\\Resources\\Texture\\timer\\timer1.png");
	else if (minute == 2)
		textureMinute = GET_SINGLE(Resources)->Load<Texture>(L"timer0", L"..\\Resources\\Texture\\timer\\timer0.png");
	else
		textureMinute = GET_SINGLE(Resources)->Load<Texture>(L"timer0", L"..\\Resources\\Texture\\timer\\timer0.png");
	mTimer->GetMeshRenderer()->GetMaterial()->SetTexture(0, textureMinute);

	// secondTimer
	int second = (int)(time) % 60;
	int ten = second / 10;
	int one = second % 10;
	if (minute == 0 && second == 0)
	{
		ten = 5; one = 9;
	}
	if (minute == 2 && second < 1)
	{
		// 3분 다 지났으면 랭킹 정하기
		SetFinalRanking();
		_isFinish = true;
	}
	wstring texTenName = L"timer" + s2ws(to_string(ten));
	wstring texOneName = L"timer" + s2ws(to_string(one));

	textureTenSec = GET_SINGLE(Resources)->Load<Texture>(texTenName, L"..\\Resources\\Texture\\timer\\" + texTenName + L".png");
	textureOneSec = GET_SINGLE(Resources)->Load<Texture>(texOneName, L"..\\Resources\\Texture\\timer\\" + texOneName + L".png");
	tTimer->GetMeshRenderer()->GetMaterial()->SetTexture(0, textureTenSec);
	oTimer->GetMeshRenderer()->GetMaterial()->SetTexture(0, textureOneSec);

	uint64 myscore = static_pointer_cast<Player>(_players[mysession->GetPlayerID()]->GetScript(0))->GetPlayerScore();
	pkt.set_score(myscore);
	if (mysession->GetPlayerID() == (g_EnterPlayerCnt-1))
		pkt.set_timer(_curTime);
	pkt.set_playerid(mysession->GetPlayerID());
	if (gPacketControl % 60 == 1)
	{
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
		mysession->Send(sendBuffer);
	}
}

void Scene::SetFinalRanking()
{
	shared_ptr<GameObject>	ranking = GetGameObject(L"finalRanking");
	ranking->_isRender = true;

	GET_SINGLE(ScoreManager)->Rank();
	vector<shared_ptr<GameObject>> players = GET_SINGLE(ScoreManager)->GetVecRankedPlayers();
	int scores[3] = {};
	wstring playerName[3] = {};

	for (size_t i = 0; i < players.size(); ++i)
	{
		scores[i] = static_pointer_cast<Player>(players[i]->GetScript(0))->GetPlayerScore();
		playerName[i] = players[i]->GetName();

		wstring scoreUIName1 = playerName[i] + L"Score1";
		wstring scoreUIName2 = playerName[i] + L"Score2";
		wstring scoreUIName3 = playerName[i] + L"Score3";
		wstring scoreIconName = playerName[i] + L"ScoreIcon";

		shared_ptr<GameObject> mScore1 = GetGameObject(scoreUIName1);
		shared_ptr<GameObject> mScore2 = GetGameObject(scoreUIName2);
		shared_ptr<GameObject> mScore3 = GetGameObject(scoreUIName3);
		shared_ptr<GameObject> mIcon = GetGameObject(scoreIconName);

		// 위치, 크기 재설정해야 함
	
		mIcon->GetTransform()->SetLocalPosition(Vec3(-180.f, 110.f - (i * 180.f), 500.f));
		mIcon->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 50.f));

		mScore1->GetTransform()->SetLocalPosition(Vec3(-100.f, 110.f - (i * 180.f), 500.f));
		mScore1->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 50.f));

		mScore2->GetTransform()->SetLocalPosition(Vec3(0.f, 110.f - (i * 180.f), 500.f));
		mScore2->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 50.f));

		mScore3->GetTransform()->SetLocalPosition(Vec3(100.f, 110.f - (i * 180.f), 500.f));
		mScore3->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 50.f));
	}
}

void Scene::CheckMouse()
{
	GetCursorPos(&_mousePos);
	ScreenToClient(GET_WINDOW.hwnd, &_mousePos);

	// 1. ItemWnd
	shared_ptr<GameObject>	itemWnd = GetGameObject(L"ItemWindow");

	for (int i = 0; i < 3; ++i)
	{
		shared_ptr<GameObject>	slot = GetGameObject(L"ItemSlot" + s2ws(to_string(i + 1)));
		wstring itemName = static_pointer_cast<ItemSlotUI>(slot->GetScript(0))->GetSettingItemName();

		if (static_pointer_cast<ItemSlotUI>(slot->GetScript(0))->GetIsSettingItem())
		{
			if (PtInRect(&_slotRt[i], _mousePos))
			{
				itemWnd->_isRender = true;

				wstring name = static_pointer_cast<ItemSlotUI>(slot->GetScript(0))->_texName;
				static_pointer_cast<ItemWindow>(itemWnd->GetScript(0))->_itemName = name;
				static_pointer_cast<ItemWindow>(itemWnd->GetScript(0))->SetItemName();

				Vec3 localPos = slot->GetTransform()->GetLocalPosition();
				localPos.y += 110.f;
				itemWnd->GetTransform()->SetLocalPosition(localPos);
				return;
			}
			else itemWnd->_isRender = false;

		}
	}

	// End & Restart
	shared_ptr<GameObject>	finalWnd = GetGameObject(L"finalRanking");

	if (finalWnd->_isRender)
	{
		if (PtInRect(&_endRt, _mousePos))
		{
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"end", L"..\\Resources\\Texture\\end.png");
			finalWnd->GetMeshRenderer()->GetMaterial()->SetTexture(0, texture);
			if (INPUT->GetButton(KEY_TYPE::LBUTTON))
			{
				_isEnd = true;
			}
		}
		else if (PtInRect(&_restartRt, _mousePos))
		{
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"restart", L"..\\Resources\\Texture\\restart.png");
			finalWnd->GetMeshRenderer()->GetMaterial()->SetTexture(0, texture);
		}
		else
		{
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"ranking", L"..\\Resources\\Texture\\ranking.png");
			finalWnd->GetMeshRenderer()->GetMaterial()->SetTexture(0, texture);
		}
	}
}

void Scene::CheckTagger()
{
	shared_ptr<GameObject> tagMark1 = _TagMarks[0];
	shared_ptr<GameObject> tagMark2 = _TagMarks[1];
	shared_ptr<GameObject> tagMark3 = _TagMarks[2];
	for (const shared_ptr<GameObject>& gameObject : _vecPlayers)
	{
		switch (gameObject->GetPlayerID())
		{
		case 0:		// 1번 플레이어
			if (gameObject->GetIsTagger()) {
				_isStart = true;
				_tagNum = 0;
			}
			break;
		case 1:		// 2번 플레이어
			if (gameObject->GetIsTagger()) {
				_isStart = true;
				_tagNum = 1;
			}
			break;
		case 2:		// 3번 플레이어
			if (gameObject->GetIsTagger()) {
				_isStart = true;
				_tagNum = 2;
			}
			break;
		}
	}
}

void Scene::StartGame()
{
	_toStartTime += DELTA_TIME;
	if (_toStartTime >= 3.f)
	{
		printf("start Game!");
	}
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
