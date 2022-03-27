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

#include "CameraScript.h"
#include "Resources.h"
#include "ParticleSystem.h"
#include "Player.h"
#include "MeshData.h"
#include "CharacterData.h"
#include "AnimationController.h"
#include "AnimationTrack.h"

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

void SceneManager::LoadScene(wstring sceneName)
{
	// TODO : ���� Scene ����
	// TODO : ���Ͽ��� Scene ���� �ε�

	_activeScene = LoadTestScene();

	_activeScene->Awake();
	_activeScene->Start();
}

void SceneManager::SetLayerName(uint8 index, const wstring& name)
{
	// ���� ������ ����
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

shared_ptr<Scene> SceneManager::LoadTestScene()
{
#pragma region LayerMask
	SetLayerName(0, L"Default");
	SetLayerName(1, L"UI");
#pragma endregion

#pragma region ComputeShader
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ComputeShader");

		// UAV �� Texture ����
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->CreateTexture(L"UAVTexture",
			DXGI_FORMAT_R8G8B8A8_UNORM, 1024, 1024,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"ComputeShader");
		material->SetShader(shader);
		material->SetInt(0, 1);
		GEngine->GetComputeDescHeap()->SetUAV(texture->GetUAVHandle(), UAV_REGISTER::u0);

		// ������ �׷� (1 * 1024 * 1)
		material->Dispatch(1, 1024, 1);
	}
#pragma endregion

	shared_ptr<Scene> scene = make_shared<Scene>();

#pragma region Camera
	{
		shared_ptr<GameObject> camera = make_shared<GameObject>();
		camera->SetName(L"Main_Camera");
		camera->AddComponent(make_shared<Transform>());
		camera->AddComponent(make_shared<Camera>()); // Near=1, Far=1000, FOV=45��
		camera->AddComponent(make_shared<CameraScript>());
		camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
		uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
		camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, true); // UI�� �� ����
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
		camera->GetCamera()->SetCullingMaskAll(); // �� ����
		camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, false); // UI�� ����
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

#pragma region 
	// Tree_01
	shared_ptr<MeshData> meshTree01 = GET_SINGLE(Resources)->LoadFBX(L"Tree_01.bin");

	vector<shared_ptr<GameObject>>	objectsTree01 = meshTree01->Instantiate();

	for (auto& gameObject : objectsTree01)
	{
		gameObject->SetName(L"Tree01");
		gameObject->SetCheckFrustum(false);
		gameObject->GetTransform()->SetLocalPosition(Vec3(0.f, -10.f, 40.f));
		gameObject->GetTransform()->SetLocalScale(Vec3(0.03f, 0.03f, 0.03f));
		scene->AddGameObject(gameObject);
	}

	// Tree_02
	shared_ptr<MeshData> meshTree02 = GET_SINGLE(Resources)->LoadFBX(L"Tree_02.bin");

	vector<shared_ptr<GameObject>>	objectsTree02 = meshTree02->Instantiate();

	for (auto& gameObject : objectsTree02)
	{
		gameObject->SetName(L"Tree02");
		gameObject->SetCheckFrustum(false);
		gameObject->GetTransform()->SetLocalPosition(Vec3(-10.f, -10.f, 40.f));
		gameObject->GetTransform()->SetLocalScale(Vec3(5.f, 5.f, 5.f));
		scene->AddGameObject(gameObject);
	}

	// Tree_03
	shared_ptr<MeshData> meshTree03 = GET_SINGLE(Resources)->LoadFBX(L"Tree_03.bin");

	vector<shared_ptr<GameObject>>	objectsTree03 = meshTree03->Instantiate();

	for (auto& gameObject : objectsTree03)
	{
		gameObject->SetName(L"Tree03");
		gameObject->SetCheckFrustum(false);
		gameObject->GetTransform()->SetLocalPosition(Vec3(10.f, -10.f, 40.f));
		gameObject->GetTransform()->SetLocalScale(Vec3(2.f, 2.f, 2.f));
		scene->AddGameObject(gameObject);
	}

	//// Mushroom_01
	//shared_ptr<MeshData> meshMushroom01 = GET_SINGLE(Resources)->LoadFBX(L"Mushroom_01.bin");

	//vector<shared_ptr<GameObject>>	objectsMushromm01 = meshMushroom01->Instantiate();

	//for (auto& gameObject : objectsMushromm01)
	//{
	//	gameObject->SetName(L"Mushroom01");
	//	gameObject->SetCheckFrustum(false);
	//	gameObject->GetTransform()->SetLocalPosition(Vec3(10.f, -10.f, 30.f));
	//	gameObject->GetTransform()->SetLocalScale(Vec3(5.f, 5.f, 5.f));
	//	scene->AddGameObject(gameObject);
	//}

	//// Mushroom_02
	//shared_ptr<MeshData> meshMushroom02 = GET_SINGLE(Resources)->LoadFBX(L"Mushroom_02.bin");

	//vector<shared_ptr<GameObject>>	objectsMushromm02 = meshMushroom02->Instantiate();

	//for (auto& gameObject : objectsMushromm02)
	//{
	//	gameObject->SetName(L"Mushroom02");
	//	gameObject->SetCheckFrustum(false);
	//	gameObject->GetTransform()->SetLocalPosition(Vec3(10.f, -10.f, 10.f));
	//	gameObject->GetTransform()->SetLocalScale(Vec3(5.f, 5.f, 5.f));
	//	scene->AddGameObject(gameObject);
	//}

	//// Rock_01
	//shared_ptr<MeshData> meshRock01 = GET_SINGLE(Resources)->LoadFBX(L"Rock_01.bin");

	//vector<shared_ptr<GameObject>>	objectsRock01 = meshRock01->Instantiate();

	//for (auto& gameObject : objectsRock01)
	//{
	//	gameObject->SetName(L"Rock01");
	//	gameObject->SetCheckFrustum(false);
	//	gameObject->GetTransform()->SetLocalPosition(Vec3(10.f, -10.f, 10.f));
	//	gameObject->GetTransform()->SetLocalScale(Vec3(5.f, 5.f, 5.f));
	//	scene->AddGameObject(gameObject);
	//}

	//// Rock_02
	//shared_ptr<MeshData> meshRock02 = GET_SINGLE(Resources)->LoadFBX(L"Rock_02.bin");

	//vector<shared_ptr<GameObject>>	objectsRock02 = meshRock02->Instantiate();

	//for (auto& gameObject : objectsRock02)
	//{
	//	gameObject->SetName(L"Rock02");
	//	gameObject->SetCheckFrustum(false);
	//	gameObject->GetTransform()->SetLocalPosition(Vec3(10.f, -10.f, 10.f));
	//	gameObject->GetTransform()->SetLocalScale(Vec3(5.f, 5.f, 5.f));
	//	scene->AddGameObject(gameObject);
	//}

	//// Rock_03
	//shared_ptr<MeshData> meshRock03 = GET_SINGLE(Resources)->LoadFBX(L"Rock_03.bin");

	//vector<shared_ptr<GameObject>>	objectsRock03 = meshRock03->Instantiate();

	//for (auto& gameObject : objectsRock03)
	//{
	//	gameObject->SetName(L"Rock03");
	//	gameObject->SetCheckFrustum(false);
	//	gameObject->GetTransform()->SetLocalPosition(Vec3(10.f, -10.f, 10.f));
	//	gameObject->GetTransform()->SetLocalScale(Vec3(5.f, 5.f, 5.f));
	//	scene->AddGameObject(gameObject);
	//}

	//// Stump_01
	//shared_ptr<MeshData> meshStump01 = GET_SINGLE(Resources)->LoadFBX(L"Stump_01.bin");

	//vector<shared_ptr<GameObject>>	objectsStump01 = meshStump01->Instantiate();

	//for (auto& gameObject : objectsStump01)
	//{
	//	gameObject->SetName(L"Stump01");
	//	gameObject->SetCheckFrustum(false);
	//	gameObject->GetTransform()->SetLocalPosition(Vec3(10.f, -10.f, 10.f));
	//	gameObject->GetTransform()->SetLocalScale(Vec3(5.f, 5.f, 5.f));
	//	scene->AddGameObject(gameObject);
	//}

	//// Grass_01
	//shared_ptr<MeshData> meshGrass01 = GET_SINGLE(Resources)->LoadFBX(L"Grass_01.bin");

	//vector<shared_ptr<GameObject>>	objectsGrass01 = meshGrass01->Instantiate();

	//for (auto& gameObject : objectsGrass01)
	//{
	//	gameObject->SetName(L"Grass01");
	//	gameObject->SetCheckFrustum(false);
	//	gameObject->GetTransform()->SetLocalPosition(Vec3(10.f, -10.f, 10.f));
	//	gameObject->GetTransform()->SetLocalScale(Vec3(5.f, 5.f, 5.f));
	//	scene->AddGameObject(gameObject);
	//}
#pragma endregion

#pragma region TestPlayer
	{
		shared_ptr<CharacterData> CharacData = GET_SINGLE(Resources)->LoadCharacter(L"EvilbearL.bin");
		
		vector<shared_ptr<GameObject>>	gameObjects = CharacData->Instantiate();

		for (auto& gameObject : gameObjects)
		{
			gameObject->SetName(L"Player");
			gameObject->SetCheckFrustum(false);
			gameObject->GetTransform()->SetLocalPosition(Vec3(15.f, -10.f, 50.f));
			gameObject->GetTransform()->SetLocalScale(Vec3(3.f, 3.f, 3.f));
			gameObject->AddComponent(make_shared<Player>());
			gameObject->GetAnimationController()->SetTrackAnimationSet(0, 0);
			scene->AddGameObject(gameObject);
		}
	}
#pragma endregion

#pragma region Terrain
	{
		shared_ptr<GameObject> obj = make_shared<GameObject>();
		obj->AddComponent(make_shared<Transform>());
		obj->GetTransform()->SetLocalScale(Vec3(1000.f, 1000.f, 50.f));
		obj->GetTransform()->SetLocalPosition(Vec3(-500.f, -50.f, 0.f));
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

#pragma region UI_Test
	for (int32 i = 0; i < 6; i++)
	{
		shared_ptr<GameObject> obj = make_shared<GameObject>();
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
		obj->AddComponent(make_shared<Transform>());
		obj->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
		obj->GetTransform()->SetLocalPosition(Vec3(-350.f + (i * 120), 250.f, 500.f));
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Texture");

			shared_ptr<Texture> texture;
			if (i < 3)
				texture = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->GetRTTexture(i);
			else if (i < 5)
				texture = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->GetRTTexture(i - 3);
			else
				texture = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->GetRTTexture(0);

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		//obj->AddComponent(meshRenderer);
		scene->AddGameObject(obj);
	}
#pragma endregion

#pragma region Directional Light
	{
		shared_ptr<GameObject> light = make_shared<GameObject>();
		light->AddComponent(make_shared<Transform>());
		light->GetTransform()->SetLocalPosition(Vec3(0, 1000, 500));
		light->AddComponent(make_shared<Light>());
		light->GetLight()->SetLightDirection(Vec3(0, -1, 0.f));
		light->GetLight()->SetLightType(LIGHT_TYPE::DIRECTIONAL_LIGHT);
		light->GetLight()->SetDiffuse(Vec3(1.f, 1.f, 1.f));
		light->GetLight()->SetAmbient(Vec3(0.1f, 0.1f, 0.1f));
		light->GetLight()->SetSpecular(Vec3(0.1f, 0.1f, 0.1f));

		scene->AddGameObject(light);
	}
#pragma endregion

	//#pragma region Tessellation Test
	//	{
	//		shared_ptr<GameObject> gameObject = make_shared<GameObject>();
	//		gameObject->AddComponent(make_shared<Transform>());
	//		gameObject->GetTransform()->SetLocalPosition(Vec3(0, 0, 300));
	//		gameObject->GetTransform()->SetLocalScale(Vec3(100, 100, 100));
	//		gameObject->GetTransform()->SetLocalRotation(Vec3(0, 0, 0));
	//
	//		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
	//		{
	//			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
	//			meshRenderer->SetMesh(mesh);
	//			meshRenderer->SetMaterial(GET_SINGLE(Resources)->Get<Material>(L"Tessellation"));
	//		}
	//		gameObject->AddComponent(meshRenderer);
	//
	//		scene->AddGameObject(gameObject);
	//	}
	//#pragma endregion

		//
	//#pragma region Point Light
	//	{
	//		shared_ptr<GameObject> light = make_shared<GameObject>();
	//		light->AddComponent(make_shared<Transform>());
	//		light->GetTransform()->SetLocalPosition(Vec3(0.f, 100.f, 150.f));
	//		light->AddComponent(make_shared<Light>());
	//		//light->GetLight()->SetLightDirection(Vec3(-1.f, -1.f, 0));
	//		light->GetLight()->SetLightType(LIGHT_TYPE::POINT_LIGHT);
	//		light->GetLight()->SetDiffuse(Vec3(0.0f, 0.5f, 0.0f));
	//		light->GetLight()->SetAmbient(Vec3(0.0f, 0.3f, 0.0f));
	//		light->GetLight()->SetSpecular(Vec3(0.0f, 0.3f, 0.0f));
	//		light->GetLight()->SetLightRange(200.f);
	//
	//		scene->AddGameObject(light);
	//	}
	//#pragma endregion
	//
	//#pragma region Spot Light
	//	{
	//		shared_ptr<GameObject> light = make_shared<GameObject>();
	//		light->AddComponent(make_shared<Transform>());
	//		light->GetTransform()->SetLocalPosition(Vec3(75.f, 0.f, 150.f));
	//		light->AddComponent(make_shared<Light>());
	//		light->GetLight()->SetLightDirection(Vec3(-1.f, 0, 0));
	//		light->GetLight()->SetLightType(LIGHT_TYPE::SPOT_LIGHT);
	//		light->GetLight()->SetDiffuse(Vec3(0.0f, 0.f, 0.5f));
	//		light->GetLight()->SetAmbient(Vec3(0.0f, 0.0f, 0.1f));
	//		light->GetLight()->SetSpecular(Vec3(0.0f, 0.0f, 0.1f));
	//		light->GetLight()->SetLightRange(200.f);
	//		light->GetLight()->SetLightAngle(3.14f / 2);
	//
	//		scene->AddGameObject(light);
	//	}
	//#pragma endregion
	//


	return scene;
}