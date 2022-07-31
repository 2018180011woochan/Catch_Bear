#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"

#include "Engine.h"
#include "Material.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "Camera.h"
#include "Light.h"
#include "TimerUI.h"

#include "CameraScript.h"
#include "Resources.h"
#include "ParticleSystem.h"
#include "Player.h"
#include "MeshData.h"
#include "CharacterData.h"
#include "AnimationController.h"
#include "AnimationTrack.h"

#include "IdleState.h"
#include "TagMark.h"
#include "ItemSlotUI.h"
#include "ItemSlotManager.h"
#include "CoolTime.h"
#include "ScoreManager.h"

#include "ServerSession.h"
#include "ItemWindow.h"
#include "ScoreUI.h"
#include "ItemManager.h"
#include "ServerPacketHandler.h"
#include "FontDevice.h"
#include "Engine.h"
#include "Leaf.h"

#include "LoginScene.h"
#include "StageScene.h"
#include "LobbyScene.h"
#include "Button.h"
#include "PlayerIcon.h"
#include "ReadyButton.h"
#include "SoundManager.h"
//shared_ptr<Scene> scene = make_shared<Scene>();
shared_ptr<Scene> scene = NULL;

void SceneManager::Update()
{
	if (_activeScene == nullptr)
		return;
	_activeScene->Update();
	_activeScene->LateUpdate();
	_activeScene->FinalUpdate();
}

// TEMP
void SceneManager::Render()
{
	if (_activeScene)
		_activeScene->Render();
}

void SceneManager::LoadScene(SCENE_ID sceneID)
{
	// 현재 신(_curScene)과 새로 전환될 신(sceneID) 비교
	if (_curScene != sceneID)
	{
		_curScene = sceneID;	// 신 업데이트

		if (_activeScene)	// 지금까지 실행된 신 정리
		{
			_activeScene.reset();
			scene.reset();
		}

		switch (sceneID)	// 새로운 신 생성
		{
		case LOGIN:
			scene = make_shared<LoginScene>();
			_activeScene = make_shared<LoginScene>();
			_activeScene = LoadLoginScene();
			break;

		case LOBBY:
			if (mysession)
				cout << mysession->GetPlayerID() << " : " << "Enter Lobby Scene!\n";
			scene = make_shared<LobbyScene>();
			_activeScene = make_shared<LobbyScene>();
			_activeScene = LoadLobbyScene();
			break;

		case STAGE:
			scene = make_shared<StageScene>();
			_activeScene = make_shared<StageScene>();
			_activeScene = LoadTestScene();
			break;

		case SCENE_CNT:
			break;
		}
	}
	
	_activeScene->Awake();
	_activeScene->Start();
	// 원래 Render()까지 해줬는데 굳이 안해도 될듯
	//_activeScene->Render();
}

void SceneManager::SetLayerName(uint8 index, const wstring& name)
{
	// 기존 데이터 삭제
	const wstring& prevName = _layerNames[index];
	_layerIndex.erase(prevName);

	_layerNames[index] = name;
	_layerIndex[name] = index;
}

uint8 SceneManager::LayerNameToIndex(const wstring& name)
{
	auto findIt = _layerIndex.find(name);
	if (findIt == _layerIndex.end())
		return 0;

	return findIt->second;
}

void SceneManager::ReStart()
{
	cout << "RESTART!!!\n";
	// 일단 제한시간 30초 늘리는게 더 빠를듯
	//_activeScene->AddCurTime(-30.f);

	////////////// 서버로 늘어난 시간 동기화시킴 //////////////
	//Protocol::C_PLUSTIME pkt;
	//pkt.set_playerid(g_EnterPlayerCnt-1);
	//auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
	//mysession->Send(sendBuffer);
	///////////////////////////////////////////////////////////

	// 플레이어
	static_pointer_cast<Player>(_activeScene->GetPlayer(mysession->GetPlayerID())->GetScript(0))->Reset();
	//static_pointer_cast<Player>(_activeScene->GetGameObject(L"Player1")->GetScript(0))->Reset();

	// 타이머
	//_activeScene->SetCurTime(-1.0f);	// 이거 안해줘도 시간 늘어나네

	// 아이템
	GET_SINGLE(ItemManager)->Reset();

	// 점수
	GET_SINGLE(ScoreManager)->InitScore();

	// 사운드
	GET_SINGLE(SoundManager)->StopSound(SoundManager::CHANNELID::BGM);
	GET_SINGLE(SoundManager)->PlayBGM(L"stageScene.wav");
}


bool SceneManager::IsEnd()
{
	return _activeScene->_isEnd;
}

void SceneManager::SetPlayerType(int index, PLAYER_TYPE playerType)
{
	_playerTypes[index] = playerType;

	// UI에도 적용
	wstring infoName = L"playerInfo" + s2ws(to_string(index+1));
	shared_ptr<GameObject> playerInfo1 = GET_SINGLE(SceneManager)->GetActiveScene()->GetGameObject(infoName);
	
	shared_ptr<Texture> newTex;
	if (playerType == PLAYER_TYPE::GRAY)
		newTex = GET_SINGLE(Resources)->Get<Texture>(L"playerIcon_gray");
	else if (playerType == PLAYER_TYPE::BROWN)
		newTex = GET_SINGLE(Resources)->Get<Texture>(L"playerIcon_brown");
	if (playerType == PLAYER_TYPE::BLUE)
		newTex = GET_SINGLE(Resources)->Get<Texture>(L"playerIcon_blue");
	else if (playerType == PLAYER_TYPE::PANDA)
		newTex = GET_SINGLE(Resources)->Get<Texture>(L"playerIcon_panda");
	if (playerType == PLAYER_TYPE::WHITE)
		newTex = GET_SINGLE(Resources)->Get<Texture>(L"playerIcon_white");
	else if (playerType == PLAYER_TYPE::PINK)
		newTex = GET_SINGLE(Resources)->Get<Texture>(L"playerIcon_pink");

	playerInfo1->GetMeshRenderer()->GetMaterial()->SetTexture(0, newTex);
}

shared_ptr<Scene> SceneManager::LoadTestScene()
{
#pragma region LayerMask
	SetLayerName(0, L"Default");
	SetLayerName(1, L"UI");
#pragma endregion

#pragma region ComputeShader
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ComputeShader");

		// UAV 용 Texture 생성
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->CreateTexture(L"UAVTexture",
			DXGI_FORMAT_R8G8B8A8_UNORM, 1024, 1024,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"ComputeShader");
		material->SetShader(shader);
		material->SetInt(0, 1);
		GEngine->GetComputeDescHeap()->SetUAV(texture->GetUAVHandle(), UAV_REGISTER::u0);

		// 쓰레드 그룹 (1 * 1024 * 1)
		material->Dispatch(1, 1024, 1);
	}
#pragma endregion

#pragma region Camera
	{
		shared_ptr<GameObject> camera = make_shared<GameObject>();
		camera->SetName(L"Main_Camera");
		camera->AddComponent(make_shared<Transform>());
		camera->AddComponent(make_shared<Camera>()); // Near=1, Far=1000, FOV=45도
		camera->AddComponent(make_shared<CameraScript>());
		camera->GetTransform()->SetLocalPosition(Vec3(0.0f, 0.f, 0.f));
		uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
		camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, true); // UI는 안 찍음
		scene->AddGameObject(camera);
	}
#pragma endregion

#pragma region UI_Camera
	{
		shared_ptr<GameObject> camera = make_shared<GameObject>();
		camera->SetName(L"Orthographic_Camera");
		camera->AddComponent(make_shared<Transform>());
		camera->AddComponent(make_shared<Camera>()); // Near=1, Far=1000, 800*600
		camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
		camera->GetCamera()->SetProjectionType(PROJECTION_TYPE::ORTHOGRAPHIC);
		uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
		camera->GetCamera()->SetCullingMaskAll(); // 다 끄고
		camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, false); // UI만 찍음
		scene->AddGameObject(camera);
	}
#pragma endregion

#pragma region SkyBox
	{
		shared_ptr<GameObject> skybox = make_shared<GameObject>();
		skybox->AddComponent(make_shared<Transform>());
		skybox->SetCheckFrustum(false);
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadCubeMesh();
			meshRenderer->SetMesh(sphereMesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Skybox");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Sky", L"..\\Resources\\Texture\\Skybox.png");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		skybox->AddComponent(meshRenderer);
		scene->AddGameObject(skybox);
	}
#pragma endregion

#pragma region StaticMesh
	LoadMapFile(scene);
#pragma endregion

#pragma region TestPlayer
	{
		wstring key = L"";
		CheckPlayerType(0, key);
		//shared_ptr<CharacterData> CharacData = GET_SINGLE(Resources)->LoadCharacter(L"Evilbear_gray");
		shared_ptr<CharacterData> CharacData = GET_SINGLE(Resources)->LoadCharacter(key);

		// 다른 클라이언트에선 0번 플레이어의 gameObjects가 0으로 보임
		// key값으로 LoadCharacter불러오는거에 문제가 있는 것으로 보임
		//vector<shared_ptr<GameObject>> gameObjects = GET_SINGLE(Resources)->LoadCharacter(L"Evilbear_blue")->Instantiate();
		// 요 코드로 로드캐릭터하면 되긴 함

		vector<shared_ptr<GameObject>>	gameObjects = CharacData->Instantiate();
		g_EnterPlayerCnt = 1;
		for (auto& gameObject : gameObjects)
		{
			gameObject->SetName(L"Player1");
			gameObject->GetTransform()->SetLocalPosition(Vec3(10.f, 0.f, 0.f));
			gameObject->GetTransform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
			gameObject->AddComponent(make_shared<Player>());
			gameObject->GetAnimationController()->SetTrackAnimationSet(0, 0);
			gameObject->GetMeshRenderer()->GetMaterial()->SetInt(0, 0);
			gameObject->SetStatic(false);
			gameObject->SetBoundingExtents(XMFLOAT3(0.4f, 1.f, 0.4f));	// 여기서 z값만 늘려서 충돌테스트 해보기 테스트, 만약 안되면 충돌하는 오브젝트만 따로 만들기
			gameObject->SetBoundingBox(BoundingOrientedBox(
				XMFLOAT3(0.0f, 0.0f, 0.0f), gameObject->GetBoundingExtents(), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)));
			gameObject->SetCheckFrustum(false);
			gameObject->SetPlayerID(0);
			gameObject->_state = new IdleState();
			static_pointer_cast<Player>(gameObject->GetScript(0))->SetTextureKey(key);
			scene->AddGameObject(gameObject);
			scene->AddPlayers(0, gameObject);
			scene->AddVecPlayers(gameObject);
			_isPlayersEnterLobby[0] = false;
			_isPlayersReady[0] = false;
			//_playerTypes[0] = PLAYER_TYPE::PLAYER_TYPE_CNT;
		}

		g_EnterPlayerCnt = 2;
		CheckPlayerType(1, key);
		shared_ptr<CharacterData> CharacData2 = GET_SINGLE(Resources)->LoadCharacter(key);
		vector<shared_ptr<GameObject>>	gameObjects2 = CharacData2->Instantiate();

		for (auto& gameObject : gameObjects2)
		{
			gameObject->SetName(L"Player2");
			gameObject->GetTransform()->SetLocalPosition(Vec3(10.f, 0.f, 5.f));
			gameObject->GetTransform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
			gameObject->AddComponent(make_shared<Player>());
			gameObject->GetAnimationController()->SetTrackAnimationSet(0, 0);
			gameObject->GetMeshRenderer()->GetMaterial()->SetInt(0, 0);
			gameObject->SetStatic(false);
			gameObject->SetBoundingExtents(XMFLOAT3(0.4f, 1.f, 0.4f));
			gameObject->SetBoundingBox(BoundingOrientedBox(
				XMFLOAT3(0.0f, 0.0f, 0.0f), gameObject->GetBoundingExtents(), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)));
			gameObject->SetCheckFrustum(false);
			gameObject->SetPlayerID(1);
			gameObject->_state = new IdleState();
			static_pointer_cast<Player>(gameObject->GetScript(0))->SetTextureKey(key);
			scene->AddGameObject(gameObject);
			scene->AddPlayers(1, gameObject);
			scene->AddVecPlayers(gameObject);
			_isPlayersEnterLobby[0] = false;
			_isPlayersReady[0] = false;
			//_playerTypes[0] = PLAYER_TYPE::PLAYER_TYPE_CNT;
		}

		g_EnterPlayerCnt = 3;		// 최종적으로 3인게임으로 바꾸면 3으로 고정 
		CheckPlayerType(2, key);
		shared_ptr<CharacterData> CharacData3 = GET_SINGLE(Resources)->LoadCharacter(key);
		vector<shared_ptr<GameObject>>	gameObjects3 = CharacData3->Instantiate();

		for (auto& gameObject : gameObjects3)
		{
			gameObject->SetName(L"Player3");
			gameObject->GetTransform()->SetLocalPosition(Vec3(10.f, 0.f, 10.f));
			gameObject->GetTransform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
			gameObject->AddComponent(make_shared<Player>());
			gameObject->GetAnimationController()->SetTrackAnimationSet(0, 0);
			gameObject->GetMeshRenderer()->GetMaterial()->SetInt(0, 0);
			gameObject->SetStatic(false);
			gameObject->SetBoundingExtents(XMFLOAT3(0.4f, 1.f, 0.4f));
			gameObject->SetBoundingBox(BoundingOrientedBox(
				XMFLOAT3(0.0f, 0.0f, 0.0f), gameObject->GetBoundingExtents(), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)));
			gameObject->SetCheckFrustum(false);
			gameObject->SetPlayerID(2);
			gameObject->_state = new IdleState();
		 	static_pointer_cast<Player>(gameObject->GetScript(0))->SetTextureKey(key);
			scene->AddGameObject(gameObject);
			scene->AddPlayers(2, gameObject);
			scene->AddVecPlayers(gameObject);
			 _isPlayersEnterLobby[0] = false;
			 _isPlayersReady[0] = false;
			//_playerTypes[0] = PLAYER_TYPE::PLAYER_TYPE_CNT;
		}

	}
#pragma endregion

#pragma region leaf
	shared_ptr<MeshData> meshHeart2 = GET_SINGLE(Resources)->LoadFBX(L"SNature_Leaf.bin");

	random_device rd;
	uniform_real_distribution<float> distX(-50, 50);
	uniform_real_distribution<float> distZ(-50, 50);

	for (int i = 0; i < 60; ++i)
	{
		vector<shared_ptr<GameObject>>	objectsHeart2 = meshHeart2->Instantiate();

		for (auto& gameObject : objectsHeart2)
		{
			gameObject->SetName(L"leaf");
			gameObject->SetCheckFrustum(false);
			gameObject->GetTransform()->SetLocalPosition(Vec3(distX(rd), 2.f, distZ(rd)));
			gameObject->GetTransform()->SetLocalRotation(Vec3(0.f, 0.f, 95.f));
			gameObject->GetTransform()->SetLocalScale(Vec3(0.5f, 0.5f, 0.5f));
			gameObject->GetMeshRenderer()->GetMaterial()->SetShader(GET_SINGLE(Resources)->Get<Shader>(L"LeafParticle"));
			gameObject->AddComponent(make_shared<Leaf>());
			scene->AddGameObject(gameObject);
		}
	}
#pragma endregion


#pragma region TagMark
	// Heart
	shared_ptr<MeshData> meshHeart = GET_SINGLE(Resources)->LoadFBX(L"Heart.bin");

	vector<shared_ptr<GameObject>>	objectsHeart = meshHeart->Instantiate();

	for (auto& gameObject : objectsHeart)
	{
		gameObject->SetName(L"PlayerTag1");
		gameObject->SetCheckFrustum(false);
		gameObject->GetTransform()->SetLocalPosition(Vec3(10.f, -2.f, 0.f));
		gameObject->GetTransform()->SetLocalRotation(Vec3(-1.57079649, 0.f, 0.f));
		gameObject->GetTransform()->SetLocalScale(Vec3(0.2f, 0.2f, 0.2f));
		gameObject->GetMeshRenderer()->GetMaterial()->SetInt(0, 0);
		gameObject->GetMeshRenderer()->GetMaterial()->SetShader(GET_SINGLE(Resources)->Get<Shader>(L"TagMark"));
		gameObject->AddComponent(make_shared<TagMark>());
		scene->AddGameObject(gameObject);
		scene->AddTagMarks(0, gameObject);
	}
	vector<shared_ptr<GameObject>>	objectsHeart2 = meshHeart->Instantiate();
	for (auto& gameObject : objectsHeart2)
	{
		gameObject->SetName(L"PlayerTag2");
		gameObject->SetCheckFrustum(false);
		gameObject->GetTransform()->SetLocalPosition(Vec3(15.f, -2.f, 5.f));
		gameObject->GetTransform()->SetLocalRotation(Vec3(-1.57079649, 0.f, 0.f));
		gameObject->GetTransform()->SetLocalScale(Vec3(0.2f, 0.2f, 0.2f));
		gameObject->GetMeshRenderer()->GetMaterial()->SetInt(0, 0);
		gameObject->GetMeshRenderer()->GetMaterial()->SetShader(GET_SINGLE(Resources)->Get<Shader>(L"TagMark"));
		gameObject->AddComponent(make_shared<TagMark>());
		scene->AddGameObject(gameObject);
		scene->AddTagMarks(1, gameObject);
	}

	vector<shared_ptr<GameObject>>	objectsHeart3 = meshHeart->Instantiate();
	for (auto& gameObject : objectsHeart3)
	{
		gameObject->SetName(L"PlayerTag3");
		gameObject->SetCheckFrustum(false);
		gameObject->GetTransform()->SetLocalPosition(Vec3(5.f, -2.f, 10.f));
		gameObject->GetTransform()->SetLocalRotation(Vec3(-1.57079649, 0.f, 0.f));
		gameObject->GetTransform()->SetLocalScale(Vec3(0.2f, 0.2f, 0.2f));
		gameObject->GetMeshRenderer()->GetMaterial()->SetInt(0, 0);
		gameObject->GetMeshRenderer()->GetMaterial()->SetShader(GET_SINGLE(Resources)->Get<Shader>(L"TagMark"));
		gameObject->AddComponent(make_shared<TagMark>());
		scene->AddGameObject(gameObject);
		scene->AddTagMarks(2, gameObject);
	}
#pragma endregion


#pragma region CenterHouse
	{
		shared_ptr<MeshData> meshHeart = GET_SINGLE(Resources)->LoadFBX(L"House01.bin");

		vector<shared_ptr<GameObject>>	objectsHeart = meshHeart->Instantiate();

		for (auto& gameObject : objectsHeart)
		{
			gameObject->SetName(L"House");
			gameObject->SetCheckFrustum(false);
			gameObject->GetTransform()->SetLocalPosition(Vec3(5.f, 0.0f, 5.f));
			gameObject->GetTransform()->SetLocalScale(Vec3(0.015f, 0.015f, 0.015f));
			scene->AddGameObject(gameObject);
		}
	}
#pragma endregion


#pragma region ItemSlotUI
	// 1.
	shared_ptr<GameObject> itemSlot1 = make_shared<GameObject>();
	itemSlot1->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
	itemSlot1->SetName(L"ItemSlot1");
	itemSlot1->AddComponent(make_shared<Transform>());
	itemSlot1->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
	itemSlot1->GetTransform()->SetLocalPosition(Vec3(-450.f, -300.f, 500.f));
	itemSlot1->AddComponent(make_shared<ItemSlotUI>());

	shared_ptr<MeshRenderer> itemSlotRenderer = make_shared<MeshRenderer>();
	{
		shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
		itemSlotRenderer->SetMesh(mesh);
	}
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemSlot");
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"itemSlot", L"..\\Resources\\Texture\\item_slot.png");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, texture);
		itemSlotRenderer->SetMaterial(material);
	}
	itemSlot1->AddComponent(itemSlotRenderer);

	scene->AddGameObject(itemSlot1);
	GET_SINGLE(ItemSlotManager)->SetItemSlot(1, itemSlot1);



	// 2.
	shared_ptr<GameObject> itemSlot2 = make_shared<GameObject>();
	itemSlot2->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
	itemSlot2->SetName(L"ItemSlot2");
	itemSlot2->AddComponent(make_shared<Transform>());
	itemSlot2->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
	itemSlot2->GetTransform()->SetLocalPosition(Vec3(-330.f, -300.f, 500.f));
	itemSlot2->AddComponent(make_shared<ItemSlotUI>());

	shared_ptr<MeshRenderer> itemSlot2Renderer = make_shared<MeshRenderer>();
	{
		shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
		itemSlot2Renderer->SetMesh(mesh);
	}
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemSlot");
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"itemSlot", L"..\\Resources\\Texture\\item_slot.png");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, texture);
		itemSlot2Renderer->SetMaterial(material);
	}
	itemSlot2->AddComponent(itemSlot2Renderer);

	scene->AddGameObject(itemSlot2);
	GET_SINGLE(ItemSlotManager)->SetItemSlot(2, itemSlot2);



	// 3.
	shared_ptr<GameObject> itemSlot3 = make_shared<GameObject>();
	itemSlot3->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
	itemSlot3->SetName(L"ItemSlot3");
	itemSlot3->AddComponent(make_shared<Transform>());
	itemSlot3->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
	itemSlot3->GetTransform()->SetLocalPosition(Vec3(-210.f, -300.f, 500.f));
	itemSlot3->AddComponent(make_shared<ItemSlotUI>());

	shared_ptr<MeshRenderer> itemSlot3Renderer = make_shared<MeshRenderer>();
	{
		shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
		itemSlot3Renderer->SetMesh(mesh);
	}
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemSlot");
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"itemSlot", L"..\\Resources\\Texture\\item_slot.png");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, texture);
		itemSlot3Renderer->SetMaterial(material);
	}
	itemSlot3->AddComponent(itemSlot3Renderer);

	scene->AddGameObject(itemSlot3);
	GET_SINGLE(ItemSlotManager)->SetItemSlot(3, itemSlot3);

	// 1-1.
	shared_ptr<GameObject> itemCoolTime1 = make_shared<GameObject>();
	itemCoolTime1->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
	itemCoolTime1->SetName(L"ItemCoolTime1");
	itemCoolTime1->AddComponent(make_shared<Transform>());
	itemCoolTime1->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
	itemCoolTime1->GetTransform()->SetLocalPosition(Vec3(-450.f, -300.f, 500.f));
	itemCoolTime1->AddComponent(make_shared<CoolTime>());
	itemCoolTime1->_isRender = false;

	shared_ptr<MeshRenderer> itemCoolTimeRenderer = make_shared<MeshRenderer>();
	{
		shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
		itemCoolTimeRenderer->SetMesh(mesh);
	}
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemCoolTime");
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"itemCoolTime", L"..\\Resources\\Texture\\coolTime.png");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, texture);
		itemCoolTimeRenderer->SetMaterial(material);
	}
	itemCoolTime1->AddComponent(itemCoolTimeRenderer);

	scene->AddItemCoolTime(itemCoolTime1);
	scene->AddGameObject(itemCoolTime1);

	// 2-2.
	shared_ptr<GameObject> itemCoolTime2 = make_shared<GameObject>();
	itemCoolTime2->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
	itemCoolTime2->SetName(L"ItemCoolTime2");
	itemCoolTime2->AddComponent(make_shared<Transform>());
	itemCoolTime2->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
	itemCoolTime2->GetTransform()->SetLocalPosition(Vec3(-330.f, -300.f, 500.f));
	itemCoolTime2->AddComponent(make_shared<CoolTime>());
	itemCoolTime2->_isRender = false;

	shared_ptr<MeshRenderer> itemCoolTime2Renderer = make_shared<MeshRenderer>();
	{
		shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
		itemCoolTime2Renderer->SetMesh(mesh);
	}
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemCoolTime");
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"itemCoolTime", L"..\\Resources\\Texture\\coolTime.png");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, texture);
		itemCoolTime2Renderer->SetMaterial(material);
	}
	itemCoolTime2->AddComponent(itemCoolTime2Renderer);

	scene->AddItemCoolTime(itemCoolTime2);
	scene->AddGameObject(itemCoolTime2);

	// 3-2
	shared_ptr<GameObject> itemCoolTime3 = make_shared<GameObject>();
	itemCoolTime3->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
	itemCoolTime3->SetName(L"ItemCoolTime3");
	itemCoolTime3->AddComponent(make_shared<Transform>());
	itemCoolTime3->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
	itemCoolTime3->GetTransform()->SetLocalPosition(Vec3(-210.f, -300.f, 500.f));
	itemCoolTime3->AddComponent(make_shared<CoolTime>());
	itemCoolTime3->_isRender = false;

	shared_ptr<MeshRenderer> itemCoolTime3Renderer = make_shared<MeshRenderer>();
	{
		shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
		itemCoolTime3Renderer->SetMesh(mesh);
	}
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemCoolTime");
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"itemCoolTime", L"..\\Resources\\Texture\\coolTime.png");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, texture);
		itemCoolTime3Renderer->SetMaterial(material);
	}
	itemCoolTime3->AddComponent(itemCoolTime3Renderer);

	scene->AddItemCoolTime(itemCoolTime3);
	scene->AddGameObject(itemCoolTime3);
#pragma endregion

#pragma region Silence
	{
		// 1.
		shared_ptr<GameObject> itemSlot1 = make_shared<GameObject>();
		itemSlot1->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		itemSlot1->SetName(L"ItemSlot1_Silence");
		itemSlot1->AddComponent(make_shared<Transform>());
		itemSlot1->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
		itemSlot1->GetTransform()->SetLocalPosition(Vec3(-450.f, -300.f, 500.f));
		itemSlot1->_isRender = false;

		shared_ptr<MeshRenderer> itemSlotRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			itemSlotRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemCoolTime");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"itemSlot_silence", L"..\\Resources\\Texture\\silence_effect.png");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			itemSlotRenderer->SetMaterial(material);
		}
		itemSlot1->AddComponent(itemSlotRenderer);

		scene->AddGameObject(itemSlot1);
	}

	{
		// 2.
		shared_ptr<GameObject> itemSlot1 = make_shared<GameObject>();
		itemSlot1->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		itemSlot1->SetName(L"ItemSlot2_Silence");
		itemSlot1->AddComponent(make_shared<Transform>());
		itemSlot1->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
		itemSlot1->GetTransform()->SetLocalPosition(Vec3(-330.f, -300.f, 500.f));
		itemSlot1->_isRender = false;

		shared_ptr<MeshRenderer> itemSlotRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			itemSlotRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemCoolTime");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"itemSlot_silence", L"..\\Resources\\Texture\\silence_effect.png");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			itemSlotRenderer->SetMaterial(material);
		}
		itemSlot1->AddComponent(itemSlotRenderer);

		scene->AddGameObject(itemSlot1);
	}

	{
		// 3.
		shared_ptr<GameObject> itemSlot1 = make_shared<GameObject>();
		itemSlot1->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		itemSlot1->SetName(L"ItemSlot3_Silence");
		itemSlot1->AddComponent(make_shared<Transform>());
		itemSlot1->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
		itemSlot1->GetTransform()->SetLocalPosition(Vec3(-210.f, -300.f, 500.f));
		itemSlot1->_isRender = false;

		shared_ptr<MeshRenderer> itemSlotRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			itemSlotRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemCoolTime");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"itemSlot_silence", L"..\\Resources\\Texture\\silence_effect.png");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			itemSlotRenderer->SetMaterial(material);
		}
		itemSlot1->AddComponent(itemSlotRenderer);

		scene->AddGameObject(itemSlot1);
	}
#pragma endregion

#pragma region ItemWnd
	// ItemWindow
	shared_ptr<GameObject> itemWnd = make_shared<GameObject>();
	itemWnd->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
	itemWnd->SetName(L"ItemWindow");
	itemWnd->AddComponent(make_shared<Transform>());
	itemWnd->GetTransform()->SetLocalScale(Vec3(300.f, 150.f, 100.f));
	itemWnd->GetTransform()->SetLocalPosition(Vec3(-330.f, -300.f, 500.f));
	itemWnd->AddComponent(make_shared<ItemWindow>());

	shared_ptr<MeshRenderer> itemWndRenderer = make_shared<MeshRenderer>();
	{
		shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
		itemWndRenderer->SetMesh(mesh);
	}
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemSlot");
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"itemSlot", L"..\\Resources\\Texture\\item_slot.png");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, texture);
		itemWndRenderer->SetMaterial(material);
	}
	itemWnd->AddComponent(itemWndRenderer);
	itemWnd->_isRender = false;
	scene->AddGameObject(itemWnd);
#pragma endregion

#pragma region Terrain
	{
		shared_ptr<GameObject> obj = make_shared<GameObject>();
		obj->AddComponent(make_shared<Transform>());
		obj->GetTransform()->SetLocalScale(Vec3(8.f, 500.f, 8.f));
		obj->GetTransform()->SetLocalPosition(Vec3(-60.f, 0.f, -60.f));
		obj->SetStatic(true);
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadTerrainMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"Terrain");
			material->SetInt(0, 0);
			meshRenderer->SetMaterial(material);
		}
		obj->AddComponent(meshRenderer);
		scene->AddGameObject(obj);
	}
#pragma endregion

#pragma region TimerUI
	{
		// :
		shared_ptr<GameObject> minuteTimer = make_shared<TimerUI>();
		minuteTimer->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		minuteTimer->SetName(L"TIMER");
		minuteTimer->AddComponent(make_shared<Transform>());
		minuteTimer->GetTransform()->SetLocalScale(Vec3(130.f, 130.f, 100.f));
		minuteTimer->GetTransform()->SetLocalPosition(Vec3(-40.f, 340.f, 500.f));
		minuteTimer->_isRender = true;

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"TimerTexture");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"timerr", L"..\\Resources\\Texture\\timer\\timerr.png");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		minuteTimer->AddComponent(meshRenderer);

		scene->AddGameObject(minuteTimer);
	}
	// minute
	shared_ptr<GameObject> minuteTimer = make_shared<TimerUI>();
	minuteTimer->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
	minuteTimer->SetName(L"minuteTimer");
	minuteTimer->AddComponent(make_shared<Transform>());
	minuteTimer->GetTransform()->SetLocalScale(Vec3(130.f, 130.f, 100.f));
	minuteTimer->GetTransform()->SetLocalPosition(Vec3(-100.f, 340.f, 500.f));
	minuteTimer->_isRender = true;

	shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
	{
		shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
		meshRenderer->SetMesh(mesh);
	}
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"TimerTexture");
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"timer3", L"..\\Resources\\Texture\\timer\\timer3.png");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, texture);
		meshRenderer->SetMaterial(material);
	}
	minuteTimer->AddComponent(meshRenderer);

	scene->AddGameObject(minuteTimer);

	// tenSec
	shared_ptr<GameObject> sec1Timer = make_shared<TimerUI>();
	sec1Timer->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
	sec1Timer->SetName(L"tenSecond");
	sec1Timer->AddComponent(make_shared<Transform>());
	sec1Timer->GetTransform()->SetLocalScale(Vec3(130.f, 130.f, 100.f));
	sec1Timer->GetTransform()->SetLocalPosition(Vec3(20.f, 340.f, 500.f));
	sec1Timer->_isRender = true;

	shared_ptr<MeshRenderer> meshRenderer1 = make_shared<MeshRenderer>();
	{
		shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
		meshRenderer1->SetMesh(mesh);
	}
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"TimerTexture");
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"timer0", L"..\\Resources\\Texture\\timer\\timer0.png");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, texture);
		meshRenderer1->SetMaterial(material);
	}
	sec1Timer->AddComponent(meshRenderer1);

	scene->AddGameObject(sec1Timer);

	// oneSec
	shared_ptr<GameObject> sec2Timer = make_shared<TimerUI>();
	sec2Timer->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
	sec2Timer->SetName(L"oneSecond");
	sec2Timer->AddComponent(make_shared<Transform>());
	sec2Timer->GetTransform()->SetLocalScale(Vec3(130.f, 130.f, 100.f));
	sec2Timer->GetTransform()->SetLocalPosition(Vec3(100.f, 340.f, 500.f));
	sec2Timer->_isRender = true;

	shared_ptr<MeshRenderer> meshRenderer2 = make_shared<MeshRenderer>();
	{
		shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
		meshRenderer2->SetMesh(mesh);
	}
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"TimerTexture");
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"timer0", L"..\\Resources\\Texture\\timer\\timer0.png");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, texture);
		meshRenderer2->SetMaterial(material);
	}
	sec2Timer->AddComponent(meshRenderer2);

	scene->AddGameObject(sec2Timer);
#pragma endregion

#pragma region final Ranking
	{
		shared_ptr<GameObject> finalRanking = make_shared<GameObject>();
		finalRanking->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		finalRanking->SetName(L"finalRanking");
		finalRanking->AddComponent(make_shared<Transform>());
		finalRanking->GetTransform()->SetLocalScale(Vec3(800.f, 800.f, 50.f));
		finalRanking->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 500.f));
		finalRanking->AddComponent(make_shared<ScoreUI>());
		finalRanking->_isRender = false;

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemSlot");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"rankingImage", L"..\\Resources\\Texture\\ranking.png");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		finalRanking->AddComponent(meshRenderer);

		scene->AddGameObject(finalRanking);
	}
#pragma endregion

#pragma region Player1 ScoreUI
	// icon
	{
		wstring key = L"";
		CheckPlayerType(0, key, true);

		shared_ptr<GameObject> player1Score1 = make_shared<GameObject>();
		player1Score1->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		player1Score1->SetName(L"Player1ScoreIcon");
		player1Score1->AddComponent(make_shared<Transform>());
		player1Score1->GetTransform()->SetLocalScale(Vec3(70.f, 70.f, 50.f));
		player1Score1->GetTransform()->SetLocalPosition(Vec3(400.f, 200.f, 500.f));
		player1Score1->AddComponent(make_shared<ScoreUI>());

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemSlot");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Get<Texture>(key);
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		player1Score1->AddComponent(meshRenderer);

		scene->AddGameObject(player1Score1);
	}

	// 
	{
	shared_ptr<GameObject> player1Score1 = make_shared<GameObject>();
	player1Score1->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
	player1Score1->SetName(L"Player1Score1");
	player1Score1->AddComponent(make_shared<Transform>());
	player1Score1->GetTransform()->SetLocalScale(Vec3(50.f, 50.f, 50.f));
	player1Score1->GetTransform()->SetLocalPosition(Vec3(450.f, 200.f, 500.f));
	player1Score1->AddComponent(make_shared<ScoreUI>());

	shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
	{
		shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
		meshRenderer->SetMesh(mesh);
	}
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemSlot");
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"score0", L"..\\Resources\\Texture\\score\\score0.png");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, texture);
		meshRenderer->SetMaterial(material);
	}
	player1Score1->AddComponent(meshRenderer);

	scene->AddGameObject(player1Score1);
	}

	//
	{
		shared_ptr<GameObject> player1Score2 = make_shared<GameObject>();
		player1Score2->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		player1Score2->SetName(L"Player1Score2");
		player1Score2->AddComponent(make_shared<Transform>());
		player1Score2->GetTransform()->SetLocalScale(Vec3(50.f, 50.f, 50.f));
		player1Score2->GetTransform()->SetLocalPosition(Vec3(480.f, 200.f, 500.f));
		player1Score2->AddComponent(make_shared<ScoreUI>());

		shared_ptr<MeshRenderer> meshRenderer1 = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer1->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemSlot");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"score0", L"..\\Resources\\Texture\\score\\score0.png");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer1->SetMaterial(material);
		}
		player1Score2->AddComponent(meshRenderer1);

		scene->AddGameObject(player1Score2);
	}
	//
	{
		shared_ptr<GameObject> player1Score3 = make_shared<GameObject>();
		player1Score3->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		player1Score3->SetName(L"Player1Score3");
		player1Score3->AddComponent(make_shared<Transform>());
		player1Score3->GetTransform()->SetLocalScale(Vec3(50.f, 50.f, 50.f));
		player1Score3->GetTransform()->SetLocalPosition(Vec3(510.f, 200.f, 500.f));
		player1Score3->AddComponent(make_shared<ScoreUI>());

		shared_ptr<MeshRenderer> meshRenderer2 = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer2->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemSlot");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"score0", L"..\\Resources\\Texture\\score\\score0.png");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer2->SetMaterial(material);
		}
		player1Score3->AddComponent(meshRenderer2);

		scene->AddGameObject(player1Score3);
	}
#pragma endregion

#pragma region Player2 ScoreUI
	// icon
	{
		wstring key = L"";
		CheckPlayerType(1, key, true);

		shared_ptr<GameObject> player1Score1 = make_shared<GameObject>();
		player1Score1->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		player1Score1->SetName(L"Player2ScoreIcon");
		player1Score1->AddComponent(make_shared<Transform>());
		player1Score1->GetTransform()->SetLocalScale(Vec3(80.f, 80.f, 50.f));
		player1Score1->GetTransform()->SetLocalPosition(Vec3(400.f, 100.f, 500.f));
		player1Score1->AddComponent(make_shared<ScoreUI>());

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemSlot");
			//shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Gicon", L"..\\Resources\\Texture\\Gicon.png");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Get<Texture>(key);
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		player1Score1->AddComponent(meshRenderer);

		scene->AddGameObject(player1Score1);
	}

	// 
	{
		shared_ptr<GameObject> player1Score1 = make_shared<GameObject>();
		player1Score1->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		player1Score1->SetName(L"Player2Score1");
		player1Score1->AddComponent(make_shared<Transform>());
		player1Score1->GetTransform()->SetLocalScale(Vec3(50.f, 50.f, 50.f));
		player1Score1->GetTransform()->SetLocalPosition(Vec3(450.f, 100.f, 500.f));
		player1Score1->AddComponent(make_shared<ScoreUI>());

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemSlot");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"score0", L"..\\Resources\\Texture\\score\\score0.png");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		player1Score1->AddComponent(meshRenderer);

		scene->AddGameObject(player1Score1);
	}

	//
	{
		shared_ptr<GameObject> player1Score2 = make_shared<GameObject>();
		player1Score2->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		player1Score2->SetName(L"Player2Score2");
		player1Score2->AddComponent(make_shared<Transform>());
		player1Score2->GetTransform()->SetLocalScale(Vec3(50.f, 50.f, 50.f));
		player1Score2->GetTransform()->SetLocalPosition(Vec3(480.f, 100.f, 500.f));
		player1Score2->AddComponent(make_shared<ScoreUI>());

		shared_ptr<MeshRenderer> meshRenderer1 = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer1->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemSlot");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"score0", L"..\\Resources\\Texture\\score\\score0.png");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer1->SetMaterial(material);
		}
		player1Score2->AddComponent(meshRenderer1);

		scene->AddGameObject(player1Score2);
	}
	//
	{
		shared_ptr<GameObject> player1Score3 = make_shared<GameObject>();
		player1Score3->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		player1Score3->SetName(L"Player2Score3");
		player1Score3->AddComponent(make_shared<Transform>());
		player1Score3->GetTransform()->SetLocalScale(Vec3(50.f, 50.f, 50.f));
		player1Score3->GetTransform()->SetLocalPosition(Vec3(510.f, 100.f, 500.f));
		player1Score3->AddComponent(make_shared<ScoreUI>());

		shared_ptr<MeshRenderer> meshRenderer2 = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer2->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemSlot");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"score0", L"..\\Resources\\Texture\\score\\score0.png");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer2->SetMaterial(material);
		}
		player1Score3->AddComponent(meshRenderer2);

		scene->AddGameObject(player1Score3);
	}
#pragma endregion

#pragma region Player3 ScoreUI
	// icon
	{
		wstring key = L"";
		CheckPlayerType(2, key, true);

		shared_ptr<GameObject> player1Score1 = make_shared<GameObject>();
		player1Score1->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		player1Score1->SetName(L"Player3ScoreIcon");
		player1Score1->AddComponent(make_shared<Transform>());
		player1Score1->GetTransform()->SetLocalScale(Vec3(80.f, 80.f, 50.f));
		player1Score1->GetTransform()->SetLocalPosition(Vec3(400.f, 0.f, 500.f));
		player1Score1->AddComponent(make_shared<ScoreUI>());

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemSlot");
			//shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Licon", L"..\\Resources\\Texture\\Licon.png");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Get<Texture>(key);
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		player1Score1->AddComponent(meshRenderer);

		scene->AddGameObject(player1Score1);
	}

	// 
	{
		shared_ptr<GameObject> player1Score1 = make_shared<GameObject>();
		player1Score1->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		player1Score1->SetName(L"Player3Score1");
		player1Score1->AddComponent(make_shared<Transform>());
		player1Score1->GetTransform()->SetLocalScale(Vec3(50.f, 50.f, 50.f));
		player1Score1->GetTransform()->SetLocalPosition(Vec3(450.f, 0.f, 500.f));
		player1Score1->AddComponent(make_shared<ScoreUI>());

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemSlot");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"score0", L"..\\Resources\\Texture\\score\\score0.png");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		player1Score1->AddComponent(meshRenderer);

		scene->AddGameObject(player1Score1);
	}

	//
	{
		shared_ptr<GameObject> player1Score2 = make_shared<GameObject>();
		player1Score2->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		player1Score2->SetName(L"Player3Score2");
		player1Score2->AddComponent(make_shared<Transform>());
		player1Score2->GetTransform()->SetLocalScale(Vec3(50.f, 50.f, 50.f));
		player1Score2->GetTransform()->SetLocalPosition(Vec3(480.f, 0.f, 500.f));
		player1Score2->AddComponent(make_shared<ScoreUI>());

		shared_ptr<MeshRenderer> meshRenderer1 = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer1->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemSlot");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"score0", L"..\\Resources\\Texture\\score\\score0.png");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer1->SetMaterial(material);
		}
		player1Score2->AddComponent(meshRenderer1);

		scene->AddGameObject(player1Score2);
	}
	//
	{
		shared_ptr<GameObject> player1Score3 = make_shared<GameObject>();
		player1Score3->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		player1Score3->SetName(L"Player3Score3");
		player1Score3->AddComponent(make_shared<Transform>());
		player1Score3->GetTransform()->SetLocalScale(Vec3(50.f, 50.f, 50.f));
		player1Score3->GetTransform()->SetLocalPosition(Vec3(510.f, 0.f, 500.f));
		player1Score3->AddComponent(make_shared<ScoreUI>());

		shared_ptr<MeshRenderer> meshRenderer2 = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer2->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemSlot");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"score0", L"..\\Resources\\Texture\\score\\score0.png");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer2->SetMaterial(material);
		}
		player1Score3->AddComponent(meshRenderer2);

		scene->AddGameObject(player1Score3);
	}
#pragma endregion



#pragma region UI_Test
	//for (int32 i = 0; i < 6; i++)
	//{
	//	shared_ptr<GameObject> obj = make_shared<GameObject>();
	//	obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
	//	obj->AddComponent(make_shared<Transform>());
	//	obj->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
	//	obj->GetTransform()->SetLocalPosition(Vec3(-350.f + (i * 120), 250.f, 500.f));
	//	shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
	//	{
	//		shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
	//		meshRenderer->SetMesh(mesh);
	//	}
	//	{
	//		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Texture");

	//		shared_ptr<Texture> texture;
	//		if (i < 3)
	//			texture = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->GetRTTexture(i);
	//		else if (i < 5)
	//			texture = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->GetRTTexture(i - 3);
	//		else
	//			texture = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->GetRTTexture(0);

	//		shared_ptr<Material> material = make_shared<Material>();
	//		material->SetShader(shader);
	//		material->SetTexture(0, texture);
	//		meshRenderer->SetMaterial(material);
	//	}
	//	obj->AddComponent(meshRenderer);
	//	scene->AddGameObject(obj);
	//}
#pragma endregion

#pragma region Directional Light
	{
		shared_ptr<GameObject> light = make_shared<GameObject>();
		light->AddComponent(make_shared<Transform>());
		light->GetTransform()->SetLocalPosition(Vec3(0, 100, 0));
		light->AddComponent(make_shared<Light>());
		light->GetLight()->SetLightDirection(Vec3(0, -1, 0.f));
		light->GetLight()->SetLightType(LIGHT_TYPE::DIRECTIONAL_LIGHT);
		light->GetLight()->SetDiffuse(Vec3(1.f, 1.f, 1.f));
		light->GetLight()->SetAmbient(Vec3(0.3f, 0.3f, 0.3f));
		light->GetLight()->SetSpecular(Vec3(0.1f, 0.1f, 0.1f));
		scene->AddGameObject(light);
	}
#pragma endregion

	return scene;
}

shared_ptr<Scene> SceneManager::LoadLoginScene()
{
#pragma region LayerMask
	SetLayerName(0, L"Default");
	SetLayerName(1, L"UI");
#pragma endregion

#pragma region UI_Camera
	{
		shared_ptr<GameObject> camera = make_shared<GameObject>();
		camera->SetName(L"Orthographic_Camera");
		camera->AddComponent(make_shared<Transform>());
		camera->AddComponent(make_shared<Camera>()); // Near=1, Far=1000, 800*600
		camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
		camera->GetCamera()->SetProjectionType(PROJECTION_TYPE::ORTHOGRAPHIC);
		uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
		camera->GetCamera()->SetCullingMaskAll(); // 다 끄고
		camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, false); // UI만 찍음
		scene->AddGameObject(camera);
	}
#pragma endregion

#pragma region loginScene
	{
		shared_ptr<GameObject> finalRanking = make_shared<GameObject>();
		finalRanking->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		finalRanking->SetName(L"loginScene");
		finalRanking->AddComponent(make_shared<Transform>());
		finalRanking->GetTransform()->SetLocalScale(Vec3(1200.f, 800.f, 100.f));
		finalRanking->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 100.f));
		finalRanking->_isRender = true;

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemSlot");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"login_background", L"..\\Resources\\Texture\\Login.png");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		finalRanking->AddComponent(meshRenderer);

		scene->AddGameObject(finalRanking);
	}
#pragma endregion

	return scene;
}

void SceneManager::CheckPlayerType(int index, wstring& key, bool isIcon)
{
	switch (_playerTypes[index])
	{
	case PLAYER_TYPE::BLUE:
		if (isIcon) key = L"playerIcon_blue";
		else key = L"Evilbear_blue";
		break;

	case PLAYER_TYPE::BROWN:
		if (isIcon) key = L"playerIcon_brown";
		else key = L"Evilbear_brown";
		break;

	case PLAYER_TYPE::GRAY:
		if (isIcon) key = L"playerIcon_gray";
		else key = L"Evilbear_gray";
		break;

	case PLAYER_TYPE::PANDA:
		if (isIcon) key = L"playerIcon_panda";
		else key = L"Evilbear_panda";
		break;

	case PLAYER_TYPE::WHITE:
		if (isIcon) key = L"playerIcon_white";
		else key = L"Evilbear_white";
		break;

	case PLAYER_TYPE::PINK:
		if (isIcon) key = L"playerIcon_pink";
		else key = L"Evilbear_pink";
		break;
	}
}

shared_ptr<Scene> SceneManager::LoadLobbyScene()
{
	_playerTypes.resize(3);
	_playerTypes[0] = PLAYER_TYPE::BROWN;
	_playerTypes[1] = PLAYER_TYPE::BROWN;
	_playerTypes[2] = PLAYER_TYPE::BROWN;

#pragma region LayerMask
	SetLayerName(0, L"Default");
	SetLayerName(1, L"UI");
#pragma endregion

#pragma region UI_Camera
	{
		shared_ptr<GameObject> camera = make_shared<GameObject>();
		camera->SetName(L"Orthographic_Camera");
		camera->AddComponent(make_shared<Transform>());
		camera->AddComponent(make_shared<Camera>()); // Near=1, Far=1000, 800*600
		camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
		camera->GetCamera()->SetProjectionType(PROJECTION_TYPE::ORTHOGRAPHIC);
		uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
		camera->GetCamera()->SetCullingMaskAll(); // 다 끄고
		camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, false); // UI만 찍음
		scene->AddGameObject(camera);
	}
#pragma endregion

#pragma region lobbyScene background
	{
		shared_ptr<GameObject> finalRanking = make_shared<GameObject>();
		finalRanking->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		finalRanking->SetName(L"background");
		finalRanking->AddComponent(make_shared<Transform>());
		finalRanking->GetTransform()->SetLocalScale(Vec3(1200.f, 800.f, 100.f));
		finalRanking->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 100.f));
		finalRanking->_isRender = true;

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemSlot");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"lobbyUI", L"..\\Resources\\Texture\\Lobby\\background.png");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		finalRanking->AddComponent(meshRenderer);

		scene->AddGameObject(finalRanking);
	}
#pragma endregion

#pragma region start
	{
		shared_ptr<GameObject> finalRanking = make_shared<GameObject>();
		finalRanking->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		finalRanking->SetName(L"start");
		finalRanking->AddComponent(make_shared<Transform>());
		finalRanking->GetTransform()->SetLocalScale(Vec3(316.f, 113.f, 100.f));
		finalRanking->GetTransform()->SetLocalPosition(Vec3(300.f, -300.f, 100.f));
		finalRanking->_isRender = true;
		finalRanking->AddComponent(make_shared<ReadyButton>());

		static_pointer_cast<ReadyButton>(finalRanking->GetScript(0))->SetCollRect(RECT{ 740, 610, 1040, 680 });
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"lobby_start_click", L"..\\Resources\\Texture\\Lobby\\start_click.png");
		static_pointer_cast<ReadyButton>(finalRanking->GetScript(0))->SetCollTexture(texture);

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemSlot");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"lobby_start", L"..\\Resources\\Texture\\Lobby\\start.png");
			static_pointer_cast<ReadyButton>(finalRanking->GetScript(0))->SetOriginalTexture(texture, L"lobby_start");

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		finalRanking->AddComponent(meshRenderer);

		scene->AddGameObject(finalRanking);
	}

#pragma endregion

#pragma region playerIcon_blue
	{
		shared_ptr<GameObject> finalRanking = make_shared<GameObject>();
		finalRanking->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		finalRanking->SetName(L"playerIcon_blue");
		finalRanking->AddComponent(make_shared<Transform>());
		finalRanking->GetTransform()->SetLocalScale(Vec3(200.f, 220.f, 100.f));
		finalRanking->GetTransform()->SetLocalPosition(Vec3(90.f, 230.f, 100.f));
		finalRanking->_isRender = true;
		finalRanking->AddComponent(make_shared<PlayerIcon>());

		static_pointer_cast<PlayerIcon>(finalRanking->GetScript(0))->SetCollRect(RECT{ 590, 70, 760, 190 });
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"playerIcon_blue_click", L"..\\Resources\\Texture\\Lobby\\blue_click.png");
		static_pointer_cast<PlayerIcon>(finalRanking->GetScript(0))->SetCollTexture(texture);

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemSlot");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Get<Texture>(L"playerIcon_blue");
			static_pointer_cast<PlayerIcon>(finalRanking->GetScript(0))->SetOriginalTexture(texture, L"playerIcon_blue");

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		finalRanking->AddComponent(meshRenderer);

		scene->AddGameObject(finalRanking);
	}
#pragma endregion

#pragma region playerIcon_brown
	{
		shared_ptr<GameObject> finalRanking = make_shared<GameObject>();
		finalRanking->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		finalRanking->SetName(L"playerIcon_brown");
		finalRanking->AddComponent(make_shared<Transform>());
		finalRanking->GetTransform()->SetLocalScale(Vec3(200.f, 220.f, 100.f));
		finalRanking->GetTransform()->SetLocalPosition(Vec3(490.f, 230.f, 100.f));
		finalRanking->_isRender = true;
		finalRanking->AddComponent(make_shared<PlayerIcon>());

		static_pointer_cast<PlayerIcon>(finalRanking->GetScript(0))->SetCollRect(RECT{ 995, 70, 1155, 190 });

		shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"playerIcon_brown_click", L"..\\Resources\\Texture\\Lobby\\brown_click.png");
		static_pointer_cast<PlayerIcon>(finalRanking->GetScript(0))->SetCollTexture(texture);

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemSlot");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Get<Texture>(L"playerIcon_brown");
			static_pointer_cast<PlayerIcon>(finalRanking->GetScript(0))->SetOriginalTexture(texture, L"playerIcon_brown");

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		finalRanking->AddComponent(meshRenderer);

		scene->AddGameObject(finalRanking);
	}
#pragma endregion

#pragma region playerIcon_gray
	{
		shared_ptr<GameObject> finalRanking = make_shared<GameObject>();
		finalRanking->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		finalRanking->SetName(L"playerIcon_gray");
		finalRanking->AddComponent(make_shared<Transform>());
		finalRanking->GetTransform()->SetLocalScale(Vec3(200.f, 220.f, 100.f));
		finalRanking->GetTransform()->SetLocalPosition(Vec3(290.f, 230.f, 100.f));
		finalRanking->_isRender = true;
		finalRanking->AddComponent(make_shared<PlayerIcon>());

		static_pointer_cast<PlayerIcon>(finalRanking->GetScript(0))->SetCollRect(RECT{ 790, 70, 955, 190 });
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"playerIcon_gray_click", L"..\\Resources\\Texture\\Lobby\\gray_click.png");
		static_pointer_cast<PlayerIcon>(finalRanking->GetScript(0))->SetCollTexture(texture);

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemSlot");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Get<Texture>(L"playerIcon_gray");
			static_pointer_cast<PlayerIcon>(finalRanking->GetScript(0))->SetOriginalTexture(texture, L"playerIcon_gray");

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		finalRanking->AddComponent(meshRenderer);

		scene->AddGameObject(finalRanking);
	}
#pragma endregion

	// new
#pragma region playerIcon_panda
	{
		shared_ptr<GameObject> finalRanking = make_shared<GameObject>();
		finalRanking->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		finalRanking->SetName(L"playerIcon_panda");
		finalRanking->AddComponent(make_shared<Transform>());
		finalRanking->GetTransform()->SetLocalScale(Vec3(200.f, 220.f, 100.f));
		finalRanking->GetTransform()->SetLocalPosition(Vec3(90.f, 30.f, 100.f));
		finalRanking->_isRender = true;
		finalRanking->AddComponent(make_shared<PlayerIcon>());

		static_pointer_cast<PlayerIcon>(finalRanking->GetScript(0))->SetCollRect(RECT{ 590, 260, 760, 385 });
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"playerIcon_panda_click", L"..\\Resources\\Texture\\Lobby\\panda_click.png");
		static_pointer_cast<PlayerIcon>(finalRanking->GetScript(0))->SetCollTexture(texture);

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemSlot");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Get<Texture>(L"playerIcon_panda");
			static_pointer_cast<PlayerIcon>(finalRanking->GetScript(0))->SetOriginalTexture(texture, L"playerIcon_panda");

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		finalRanking->AddComponent(meshRenderer);

		scene->AddGameObject(finalRanking);
	}
#pragma endregion

#pragma region playerIcon_pink
	{
		shared_ptr<GameObject> finalRanking = make_shared<GameObject>();
		finalRanking->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		finalRanking->SetName(L"playerIcon_pink");
		finalRanking->AddComponent(make_shared<Transform>());
		finalRanking->GetTransform()->SetLocalScale(Vec3(200.f, 220.f, 100.f));
		finalRanking->GetTransform()->SetLocalPosition(Vec3(490.f, 30.f, 100.f));
		finalRanking->_isRender = true;
		finalRanking->AddComponent(make_shared<PlayerIcon>());

		static_pointer_cast<PlayerIcon>(finalRanking->GetScript(0))->SetCollRect(RECT{ 995, 260, 1155, 385 });

		shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"playerIcon_pink_click", L"..\\Resources\\Texture\\Lobby\\pink_click.png");
		static_pointer_cast<PlayerIcon>(finalRanking->GetScript(0))->SetCollTexture(texture);

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemSlot");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Get<Texture>(L"playerIcon_pink");
			static_pointer_cast<PlayerIcon>(finalRanking->GetScript(0))->SetOriginalTexture(texture, L"playerIcon_pink");

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		finalRanking->AddComponent(meshRenderer);

		scene->AddGameObject(finalRanking);
	}
#pragma endregion

#pragma region playerIcon_white
	{
		shared_ptr<GameObject> finalRanking = make_shared<GameObject>();
		finalRanking->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		finalRanking->SetName(L"playerIcon_white");
		finalRanking->AddComponent(make_shared<Transform>());
		//finalRanking->GetTransform()->SetLocalScale(Vec3(110.f, 89.f, 100.f));
		finalRanking->GetTransform()->SetLocalScale(Vec3(200.f, 220.f, 100.f));
		finalRanking->GetTransform()->SetLocalPosition(Vec3(290.f, 30.f, 100.f));
		finalRanking->_isRender = true;
		finalRanking->AddComponent(make_shared<PlayerIcon>());

		static_pointer_cast<PlayerIcon>(finalRanking->GetScript(0))->SetCollRect(RECT{ 790, 260, 955, 385 });
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"playerIcon_white_click", L"..\\Resources\\Texture\\Lobby\\white_click.png");
		static_pointer_cast<PlayerIcon>(finalRanking->GetScript(0))->SetCollTexture(texture);

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemSlot");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Get<Texture>(L"playerIcon_white");
			static_pointer_cast<PlayerIcon>(finalRanking->GetScript(0))->SetOriginalTexture(texture, L"playerIcon_white");

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		finalRanking->AddComponent(meshRenderer);

		scene->AddGameObject(finalRanking);
	}
#pragma endregion


#pragma region playerInfo1
	{
		shared_ptr<GameObject> finalRanking = make_shared<GameObject>();
		finalRanking->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		finalRanking->SetName(L"playerInfo1");
		finalRanking->AddComponent(make_shared<Transform>());
		finalRanking->GetTransform()->SetLocalScale(Vec3(120.f, 140.f, 100.f));
		finalRanking->GetTransform()->SetLocalPosition(Vec3(-470.f, 280.f, 100.f));
		finalRanking->_isRender = false;
		finalRanking->AddComponent(make_shared<Button>());

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemSlot");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Get<Texture>(L"playerIcon_brown");

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		finalRanking->AddComponent(meshRenderer);

		scene->AddGameObject(finalRanking);
	}
#pragma endregion

#pragma region playerInfo1_Num
	{
		shared_ptr<GameObject> finalRanking = make_shared<GameObject>();
		finalRanking->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		finalRanking->SetName(L"playerInfo1_num");
		finalRanking->AddComponent(make_shared<Transform>());
		finalRanking->GetTransform()->SetLocalScale(Vec3(60.f, 70.f, 100.f));
		finalRanking->GetTransform()->SetLocalPosition(Vec3(-550.f, 280.f, 100.f));
		finalRanking->_isRender = false;
		finalRanking->AddComponent(make_shared<Button>());

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemSlot");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"playerNum0", L"..\\Resources\\Texture\\playerNum0.png");

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		finalRanking->AddComponent(meshRenderer);

		scene->AddGameObject(finalRanking);
	}
#pragma endregion

#pragma region playerInfo1_start
	{
		shared_ptr<GameObject> finalRanking = make_shared<GameObject>();
		finalRanking->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		finalRanking->SetName(L"playerInfo1_start");
		finalRanking->AddComponent(make_shared<Transform>());
		finalRanking->GetTransform()->SetLocalScale(Vec3(303.f, 100.f, 100.f));
		finalRanking->GetTransform()->SetLocalPosition(Vec3(-220.f, 280.f, 100.f));
		finalRanking->_isRender = false;
		finalRanking->AddComponent(make_shared<Button>());

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemSlot");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"lobby_start", L"..\\Resources\\Texture\\Lobby\\start.png");
			static_pointer_cast<Button>(finalRanking->GetScript(0))->SetOriginalTexture(texture, L"lobby_start");

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		finalRanking->AddComponent(meshRenderer);

		scene->AddGameObject(finalRanking);
	}

#pragma endregion

#pragma region playerInfo2
	{
		shared_ptr<GameObject> finalRanking = make_shared<GameObject>();
		finalRanking->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		finalRanking->SetName(L"playerInfo2");
		finalRanking->AddComponent(make_shared<Transform>());
		//finalRanking->GetTransform()->SetLocalScale(Vec3(110.f, 89.f, 100.f));
		finalRanking->GetTransform()->SetLocalScale(Vec3(120.f, 140.f, 100.f));
		finalRanking->GetTransform()->SetLocalPosition(Vec3(-470.f, 140.f, 100.f));
		finalRanking->_isRender = false;
		finalRanking->AddComponent(make_shared<Button>());

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemSlot");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Get<Texture>(L"playerIcon_brown");

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		finalRanking->AddComponent(meshRenderer);

		scene->AddGameObject(finalRanking);
	}
#pragma endregion

#pragma region playerInfo2_Num
	{
		shared_ptr<GameObject> finalRanking = make_shared<GameObject>();
		finalRanking->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		finalRanking->SetName(L"playerInfo2_num");
		finalRanking->AddComponent(make_shared<Transform>());
		finalRanking->GetTransform()->SetLocalScale(Vec3(60.f, 70.f, 100.f));
		finalRanking->GetTransform()->SetLocalPosition(Vec3(-550.f, 140.f, 100.f));
		finalRanking->_isRender = false;
		finalRanking->AddComponent(make_shared<Button>());

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemSlot");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"playerNum1", L"..\\Resources\\Texture\\playerNum1.png");

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		finalRanking->AddComponent(meshRenderer);

		scene->AddGameObject(finalRanking);
	}
#pragma endregion

#pragma region playerInfo2_start
	{
		shared_ptr<GameObject> finalRanking = make_shared<GameObject>();
		finalRanking->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		finalRanking->SetName(L"playerInfo2_start");
		finalRanking->AddComponent(make_shared<Transform>());
		finalRanking->GetTransform()->SetLocalScale(Vec3(316.f, 113.f, 100.f));
		finalRanking->GetTransform()->SetLocalPosition(Vec3(-220.f, 140.f, 100.f));
		finalRanking->_isRender = false;
		finalRanking->AddComponent(make_shared<Button>());

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemSlot");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"lobby_start", L"..\\Resources\\Texture\\Lobby\\start.png");
			static_pointer_cast<Button>(finalRanking->GetScript(0))->SetOriginalTexture(texture, L"lobby_start");

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		finalRanking->AddComponent(meshRenderer);

		scene->AddGameObject(finalRanking);
	}

#pragma endregion

#pragma region playerInfo3
	{
		shared_ptr<GameObject> finalRanking = make_shared<GameObject>();
		finalRanking->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		finalRanking->SetName(L"playerInfo3");
		finalRanking->AddComponent(make_shared<Transform>());
		finalRanking->GetTransform()->SetLocalScale(Vec3(120.f, 140.f, 100.f));
		finalRanking->GetTransform()->SetLocalPosition(Vec3(-470.f, 0.f, 100.f));
		finalRanking->_isRender = false;
		finalRanking->AddComponent(make_shared<Button>());

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemSlot");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Get<Texture>(L"playerIcon_brown");

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		finalRanking->AddComponent(meshRenderer);

		scene->AddGameObject(finalRanking);
	}
#pragma endregion

#pragma region playerInfo3_Num
	{
		shared_ptr<GameObject> finalRanking = make_shared<GameObject>();
		finalRanking->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		finalRanking->SetName(L"playerInfo3_num");
		finalRanking->AddComponent(make_shared<Transform>());
		finalRanking->GetTransform()->SetLocalScale(Vec3(60.f, 70.f, 100.f));
		finalRanking->GetTransform()->SetLocalPosition(Vec3(-550.f, 0.f, 100.f));
		finalRanking->_isRender = false;
		finalRanking->AddComponent(make_shared<Button>());

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemSlot");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"playerNum2", L"..\\Resources\\Texture\\playerNum2.png");

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		finalRanking->AddComponent(meshRenderer);

		scene->AddGameObject(finalRanking);
	}
#pragma endregion

#pragma region playerInfo3_start
	{
		shared_ptr<GameObject> finalRanking = make_shared<GameObject>();
		finalRanking->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		finalRanking->SetName(L"playerInfo3_start");
		finalRanking->AddComponent(make_shared<Transform>());
		finalRanking->GetTransform()->SetLocalScale(Vec3(316.f, 113.f, 100.f));
		finalRanking->GetTransform()->SetLocalPosition(Vec3(-220.f, 0.f, 100.f));
		finalRanking->_isRender = false;
		finalRanking->AddComponent(make_shared<Button>());

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemSlot");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"lobby_start", L"..\\Resources\\Texture\\Lobby\\start.png");
			static_pointer_cast<Button>(finalRanking->GetScript(0))->SetOriginalTexture(texture, L"lobby_start");

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		finalRanking->AddComponent(meshRenderer);

		scene->AddGameObject(finalRanking);
	}

#pragma endregion


#pragma region chat
	{
		shared_ptr<GameObject> finalRanking = make_shared<GameObject>();
		finalRanking->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		finalRanking->SetName(L"chat");
		finalRanking->AddComponent(make_shared<Transform>());
		finalRanking->GetTransform()->SetLocalScale(Vec3(550.f, 300.f, 100.f));
		finalRanking->GetTransform()->SetLocalPosition(Vec3(-270.f, -230.f, 100.f));
		finalRanking->_isRender = true;
		finalRanking->AddComponent(make_shared<Button>());

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ItemSlot");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"chatting", L"..\\Resources\\Texture\\Lobby\\chat.png");

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		finalRanking->AddComponent(meshRenderer);

		scene->AddGameObject(finalRanking);
	}

#pragma endregion

	return scene;
}

void SceneManager::LoadMapFile(shared_ptr<Scene> scene)
{
	FILE* pFile;
	char pStrTocken[64] = { '\0' };
	UINT	nReads = 0;
	wstring strpath = L"..\\Resources\\Binary\\Demo_Objects.bin";

	fopen_s(&pFile, ws2s(strpath).c_str(), "rb");
	if (pFile == NULL)		return;
	rewind(pFile);
	int l = 0;

	for (; ;)
	{
		ReadStringFromFileForCharac(pFile, pStrTocken);

		for (; ;)
		{
			if (!strcmp(pStrTocken, "<ObjectName>:"))
			{
				Vec3 scale, rotate, trans;

				ReadStringFromFileForCharac(pFile, pStrTocken);
				wstring name = s2ws(pStrTocken);

				shared_ptr<MeshData> meshData = NULL;
				vector<shared_ptr<GameObject>> obj;

				if (strcmp(pStrTocken, "Plane"))
				{
					if (!strcmp(pStrTocken, "wooden_fence_04:Mesh"))
						name = L"wooden_fence_04";

					meshData = GET_SINGLE(Resources)->LoadFBX(name + L".bin");
					obj = meshData->Instantiate();
				}

				ReadStringFromFileForCharac(pFile, pStrTocken);
				if (!strcmp(pStrTocken, "<Transform>:"))
					nReads = (UINT)::fread(&trans, sizeof(Vec3), 1, pFile);

				ReadStringFromFileForCharac(pFile, pStrTocken);
				if (!strcmp(pStrTocken, "<Rotation>:"))
					nReads = (UINT)::fread(&rotate, sizeof(Vec3), 1, pFile);

				ReadStringFromFileForCharac(pFile, pStrTocken);
				if (!strcmp(pStrTocken, "<Scale>:"))
				{
					nReads = (UINT)::fread(&scale, sizeof(Vec3), 1, pFile);
					if (name == L"Plane")
					{
						scale = Vec3(0.0098f, 0.02f, 0.02f);
						//AddMapObject(scene, obj, name, trans, scale, rotate);
					}
					else if (name == L"wooden_fence_04")
					{
						rotate.y -= 10.f;
						AddMapObject(scene, obj, name, trans, scale, rotate);
					}
					else AddMapObject(scene, obj, name, trans, scale, Vec3(0.f, 0.f, 0.f));
					break;
				}
				
			}
			else
			{
				fclose(pFile);
				return;
			}
		}
	}
}

void SceneManager::AddMapObject(shared_ptr<Scene> scene, vector<shared_ptr<GameObject>> gameObj, wstring name, Vec3 trans, Vec3 scale, Vec3 rotate)
{
	for (auto& object : gameObj)
	{
		object->SetName(name);
		object->SetCheckFrustum(false);
		object->GetTransform()->SetLocalPosition(trans);
		object->GetTransform()->SetLocalScale(scale);
		object->GetTransform()->SetLocalRotation(rotate);
		if (name == L"Tree_01") {
			object->SetBoundingExtents(XMFLOAT3(0.4f, 2.7f, 0.4f));
			object->SetBoundingBox(BoundingOrientedBox(
				XMFLOAT3(0.0f, 2.7f, 1.2f), object->GetBoundingExtents(), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)));
		}
		else if (name == L"Tree_02") {
			object->SetBoundingExtents(XMFLOAT3(0.7f, 2.1f, 0.7f));
			object->SetBoundingBox(BoundingOrientedBox(
				XMFLOAT3(0.0f, 2.0f, 0.0f), object->GetBoundingExtents(), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)));
		}
		else if (name == L"Tree_03") {
			object->SetBoundingExtents(XMFLOAT3(0.7f, 2.4f, 0.7f));
			object->SetBoundingBox(BoundingOrientedBox(
				XMFLOAT3(0.0f, 2.4f, -0.0f), object->GetBoundingExtents(), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)));
		}
		else if (name == L"Rock_02") {
			object->SetBoundingExtents(XMFLOAT3(0.08748f, 0.04690f, 0.08150f));
			object->SetBoundingBox(BoundingOrientedBox(
				XMFLOAT3(0.0f, 0.0f, 0.0f), object->GetBoundingExtents(), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)));
		}
		else if (name == L"Rock_03") {
			object->SetBoundingExtents(XMFLOAT3(2.f, 0.075f, 2.f));
			object->SetBoundingBox(BoundingOrientedBox(
				XMFLOAT3(0.0f, 0.1f, 0.001f), object->GetBoundingExtents(), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)));
		}
		else if (name == L"Rock_04") {
			object->SetBoundingExtents(XMFLOAT3(0.5f, 0.5f, 0.5f));
			object->SetBoundingBox(BoundingOrientedBox(
				XMFLOAT3(0.0f, 0.5f, 0.0f), object->GetBoundingExtents(), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)));
		}
		else if (name == L"wooden_fence_04") {
			object->SetBoundingExtents(XMFLOAT3(2.4f, 1.3f, 2.3f));
			object->SetBoundingBox(BoundingOrientedBox(
				XMFLOAT3(0.f, 1.3f, 0.f), object->GetBoundingExtents(), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)));
		}
		else {
			object->SetBoundingExtents(XMFLOAT3(0.f, 0.0f, 0.0f));
			object->SetBoundingBox(BoundingOrientedBox(
				XMFLOAT3(-0.9f, 0.6f, -2.4f), object->GetBoundingExtents(), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)));
		}
		scene->AddGameObject(object);
		scene->AddStaticObj(object);
	}
}