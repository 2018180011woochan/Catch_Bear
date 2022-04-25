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

void Scene::Awake()
{
	// ��ȸ�� �Ѵ�
	// shared_ptr: � ��ü�� �����ϰ� �Ǹ� ���������� RC�� 1�� �����ȴ�
	// shared_ptr�̶�� �ص� &�� �ް� �Ǹ� �� RC�� 1 ������Ű�� �ʴ´�
	// gameObject�� �����ؼ� �ٸ� ������ �ѱ��� �����Ŵϱ� 
	// �����ϱ⺸�ٴ� &�� ��� ������ ���������� �� ���� ����� ���� �� �ִ�
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
	CheckTagger();

	if (_isStart)
	{
		_toStartTime += DELTA_TIME;
		if (_toStartTime >= 7.f)
		{
			CheckMouse();
			GET_SINGLE(Input)->Update();
			GET_SINGLE(ItemManager)->Update();
			GET_SINGLE(ScoreManager)->Update();
			GET_SINGLE(CollidManager)->Update();
			SetTimer();

			for (const shared_ptr<GameObject>& gameObject : _gameObjects)
			{
				gameObject->Update();
			}	
		}
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

void Scene::SetTimer()
{
	Protocol::C_PLAYERINFO pkt;

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
	mTimer->GetMeshRenderer()->GetMaterial()->SetTexture(0, textureMinute);

	// secondTimer
	int second = (int)(time) % 60;
	int ten = second / 10;
	int one = second % 10;
	if (second == 0)
	{
		ten = 5; one = 9;
	}
	wstring texTenName = L"timer" + s2ws(to_string(ten));
	wstring texOneName = L"timer" + s2ws(to_string(one));

	textureTenSec = GET_SINGLE(Resources)->Load<Texture>(texTenName, L"..\\Resources\\Texture\\timer\\" + texTenName + L".png");
	textureOneSec = GET_SINGLE(Resources)->Load<Texture>(texOneName, L"..\\Resources\\Texture\\timer\\" + texOneName + L".png");
	tTimer->GetMeshRenderer()->GetMaterial()->SetTexture(0, textureTenSec);
	oTimer->GetMeshRenderer()->GetMaterial()->SetTexture(0, textureOneSec);

	uint64 myscore = static_pointer_cast<Player>(_players[mysession->GetPlayerID()]->GetScript(0))->GetPlayerScore();
	pkt.set_score(myscore);
	if (mysession->GetPlayerID() == 0)
	{
		pkt.set_timer(_curTime);
	}
	pkt.set_playerid(mysession->GetPlayerID());
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
	mysession->Send(sendBuffer);
}

void Scene::CheckMouse()
{
	GetCursorPos(&_mousePos);
	ScreenToClient(GET_WINDOW.hwnd, &_mousePos);

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
}

void Scene::CheckTagger()
{
	/*for (const shared_ptr<GameObject>& gameObject : _vecPlayers)
	{
		wstring name = gameObject->GetName();
		if (gameObject->GetIsTagger())
		{
			if (name == L"Player1")
			{
				shared_ptr<GameObject> tagMark = GetGameObject(L"PlayerTag1");
				tagMark->GetMeshRenderer()->GetMaterial()->SetShader(GET_SINGLE(Resources)->Get<Shader>(L"TagMark"));
			}
			else if (name == L"Player2")
			{
				shared_ptr<GameObject> tagMark = GetGameObject(L"PlayerTag2");
				tagMark->GetMeshRenderer()->GetMaterial()->SetShader(GET_SINGLE(Resources)->Get<Shader>(L"TagMark"));
			}
			else if (name == L"Player3")
			{
				shared_ptr<GameObject> tagMark = GetGameObject(L"PlayerTag3");
				tagMark->GetMeshRenderer()->GetMaterial()->SetShader(GET_SINGLE(Resources)->Get<Shader>(L"TagMark"));
			}
			_isStart = true;
		}
		else if (!gameObject->GetIsTagger())
		{
			if (name == L"Player1")
			{
				shared_ptr<GameObject> tagMark = GetGameObject(L"PlayerTag1");
				tagMark->GetMeshRenderer()->GetMaterial()->SetShader(GET_SINGLE(Resources)->Get<Shader>(L"NormalTagMark"));
			}
			else if (name == L"Player2")
			{
				shared_ptr<GameObject> tagMark = GetGameObject(L"PlayerTag2");
				tagMark->GetMeshRenderer()->GetMaterial()->SetShader(GET_SINGLE(Resources)->Get<Shader>(L"NormalTagMark"));
			}
			else if (name == L"Player3")
			{
				shared_ptr<GameObject> tagMark = GetGameObject(L"PlayerTag3");
				tagMark->GetMeshRenderer()->GetMaterial()->SetShader(GET_SINGLE(Resources)->Get<Shader>(L"NormalTagMark"));
			}
		}
	}*/

	shared_ptr<GameObject> tagMark1 = _TagMarks[0];
	shared_ptr<GameObject> tagMark2 = _TagMarks[1];
	shared_ptr<GameObject> tagMark3 = _TagMarks[2];
	for (const shared_ptr<GameObject>& gameObject : _vecPlayers)
	{
		switch (gameObject->GetPlayerID())
		{
		case 0:		// 1�� �÷��̾�
			if (gameObject->GetIsTagger()) {
				tagMark1->GetMeshRenderer()->GetMaterial()->SetShader(GET_SINGLE(Resources)->Get<Shader>(L"TagMark"));
				_isStart = true;
				_tagNum = 0;
			}
			else
				tagMark1->GetMeshRenderer()->GetMaterial()->SetShader(GET_SINGLE(Resources)->Get<Shader>(L"NormalTagMark"));
			break;
		case 1:		// 2�� �÷��̾�
			if (gameObject->GetIsTagger()) {
				tagMark2->GetMeshRenderer()->GetMaterial()->SetShader(GET_SINGLE(Resources)->Get<Shader>(L"TagMark"));
				_isStart = true;
				_tagNum = 1;
			}
			else
				tagMark2->GetMeshRenderer()->GetMaterial()->SetShader(GET_SINGLE(Resources)->Get<Shader>(L"NormalTagMark"));
			break;
		case 2:		// 3�� �÷��̾�
			if (gameObject->GetIsTagger()) {
				tagMark3->GetMeshRenderer()->GetMaterial()->SetShader(GET_SINGLE(Resources)->Get<Shader>(L"TagMark"));
				_isStart = true;
				_tagNum = 2;
			}
			else
				tagMark3->GetMeshRenderer()->GetMaterial()->SetShader(GET_SINGLE(Resources)->Get<Shader>(L"NormalTagMark"));
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
