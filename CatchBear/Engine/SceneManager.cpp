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

shared_ptr<Scene> scene = make_shared<Scene>();

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

void SceneManager::MakePlayer(uint64 _playerID)
{
	{
		shared_ptr<GameObject> obj = make_shared<GameObject>();
		obj->SetName(L"Player");
		obj->AddComponent(make_shared<Transform>());
		obj->AddComponent(make_shared<Player>());
		obj->GetTransform()->SetLocalScale(Vec3(50.f, 50.f, 50.f));
		obj->GetTransform()->SetLocalPosition(Vec3(200.f + (_playerID * 100.f), 0.f, 500.f));
		obj->SetStatic(false);
		obj->SetCheckFrustum(false);	// �ø� �������� �ø����� �ʵ��� �����ص�
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadCubeMesh();
			meshRenderer->SetMesh(sphereMesh);
		}
		{
			shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"GameObject");
			meshRenderer->SetMaterial(material->Clone());
		}

		// �÷��̾�ID ����
		obj->SetPlayerID(_playerID);

		obj->AddComponent(meshRenderer);
		scene->AddGameObject(obj);
		scene->AddPlayers(_playerID, obj);
	}
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

#pragma region Camera
	{
		shared_ptr<GameObject> camera = make_shared<GameObject>();
		camera->SetName(L"Main_Camera");
		camera->AddComponent(make_shared<Transform>());
		camera->AddComponent(make_shared<Camera>()); // Near=1, Far=1000, FOV=45��
		camera->AddComponent(make_shared<CameraScript>());
		camera->GetTransform()->SetLocalPosition(Vec3(0.0f, 0.f, 0.f));
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

#pragma region �׽�Ʈ�� Object
	{
		//shared_ptr<GameObject> obj = make_shared<GameObject>();
		//obj->SetName(L"Wall");
		//obj->AddComponent(make_shared<Transform>());
		//obj->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
		//obj->GetTransform()->SetLocalPosition(Vec3(0, 0.f, 700.f));
		//obj->SetStatic(false);
		//shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		//{
		//	shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadSphereMesh();
		//	meshRenderer->SetMesh(sphereMesh);
		//}
		//{
		//	shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"GameObject");
		//	meshRenderer->SetMaterial(material->Clone());
		//}
		//obj->AddComponent(meshRenderer);
		//scene->AddGameObject(obj);
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
	//LoadMapFile(scene);
	//LoadMap(scene);
#pragma endregion

#pragma region Item
	// present1
	// Resources::LoadFBX()
	// fbx������� �� ���̳ʸ� ������ �ε� & �ε��� ������ �������� MeshData ��ü �����ؼ� ����
	shared_ptr<MeshData> meshPresent1 = GET_SINGLE(Resources)->LoadFBX(L"present1.bin");

	for (int i = 0; i < 2; ++i)
	{
		vector<shared_ptr<GameObject>>	objectsPresent1 = meshPresent1->Instantiate();

		for (auto& gameObject : objectsPresent1)
		{
			gameObject->SetName(L"Present1");
			gameObject->SetCheckFrustum(false);
			gameObject->GetTransform()->SetLocalPosition(Vec3(5.324442f, -2.475603f, -3 + i * 5));
			gameObject->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));
			gameObject->GetMeshRenderer()->GetMaterial()->SetInt(0, 0);
			scene->AddGameObject(gameObject);
		}
	}

	// present4
	shared_ptr<MeshData> meshPresent4 = GET_SINGLE(Resources)->LoadFBX(L"present4.bin");

	vector<shared_ptr<GameObject>>	objectsPresent4 = meshPresent4->Instantiate();

	for (auto& gameObject : objectsPresent4)
	{
		gameObject->SetName(L"Present4");
		gameObject->SetCheckFrustum(false);
		gameObject->GetTransform()->SetLocalPosition(Vec3(5.324442f, -2.475603f, 3));
		gameObject->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));
		gameObject->GetMeshRenderer()->GetMaterial()->SetInt(0, 0);
		scene->AddGameObject(gameObject);
	}


#pragma endregion

#pragma region TestPlayer
	{
		shared_ptr<CharacterData> CharacData = GET_SINGLE(Resources)->LoadCharacter(L"EvilbearL2.bin");
		
		vector<shared_ptr<GameObject>>	gameObjects = CharacData->Instantiate();

		for (auto& gameObject : gameObjects)
		{
			gameObject->SetName(L"Player");
			gameObject->GetTransform()->SetLocalPosition(Vec3(5.324442f, -2.475603f, -1.120835f));
			gameObject->GetTransform()->SetLocalScale(Vec3(5.f, 5.f, 5.f));
			gameObject->GetTransform()->SetLocalRotation(Vec3(0.f, 0.f, 0.f));
			gameObject->AddComponent(make_shared<Player>());
			gameObject->GetAnimationController()->SetTrackAnimationSet(0, 0);
			gameObject->SetStatic(false);
			gameObject->SetCheckFrustum(false);
			scene->AddGameObject(gameObject);
		}
	}
#pragma endregion

#pragma region Terrain
	{
		//shared_ptr<GameObject> obj = make_shared<GameObject>();
		//obj->AddComponent(make_shared<Transform>());
		//obj->GetTransform()->SetLocalScale(Vec3(1000.f, 1000.f, 50.f));
		//obj->GetTransform()->SetLocalPosition(Vec3(-500.f, -50.f, 0.f));
		//obj->SetStatic(true);
		//shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		//{
		//	shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadTerrainMesh();
		//	meshRenderer->SetMesh(mesh);
		//}
		//{
		//	shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"Terrain");
		//	material->SetInt(0, 0);
		//	meshRenderer->SetMaterial(material);
		//}
		//obj->AddComponent(meshRenderer);
		//scene->AddGameObject(obj);
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
		light->GetTransform()->SetLocalPosition(Vec3(0, 1000, 0));
		light->AddComponent(make_shared<Light>());
		light->GetLight()->SetLightDirection(Vec3(0, -1, 0.f));
		light->GetLight()->SetLightType(LIGHT_TYPE::DIRECTIONAL_LIGHT);
		light->GetLight()->SetDiffuse(Vec3(1.f, 1.f, 1.f));
		light->GetLight()->SetAmbient(Vec3(0.1f, 0.1f, 0.1f));
		light->GetLight()->SetSpecular(Vec3(0.1f, 0.1f, 0.1f));

		scene->AddGameObject(light);
	}
#pragma endregion

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

void SceneManager::LoadMapFile(shared_ptr<Scene> scene)
{
	FILE* pFile;
	char pStrTocken[64] = { '\0' };
	UINT	nReads = 0;
	wstring strpath = L"..\\Resources\\Binary\\Objects.bin";

	fopen_s(&pFile, ws2s(strpath).c_str(), "rb");
	if (pFile == NULL)
	{
		return;
	}
	rewind(pFile);

	int i = 0;


	for (; ;)
	{
		ReadStringFromFileForCharac(pFile, pStrTocken);

		for (; ;)
		{
			if (!strcmp(pStrTocken, "<ObjectName>:"))
			{
				Vec3 scale, rotate, trans;
				++i;

				shared_ptr<MeshData> meshData;
				shared_ptr<GameObject> obj;
				ReadStringFromFileForCharac(pFile, pStrTocken);
				wstring name = s2ws(pStrTocken);
				_objectName.push_back(name);
				if (!strcmp(pStrTocken, "Stump_01"))
				{
					fclose(pFile);
					return;
				}

				meshData = GET_SINGLE(Resources)->Get<MeshData>(name + L".bin");
				

				ReadStringFromFileForCharac(pFile, pStrTocken);
				if (!strcmp(pStrTocken, "<Transform>:"))
				{
					nReads = (UINT)::fread(&trans, sizeof(Vec3), 1, pFile);
					_transform.push_back(trans);
					//obj->GetTransform()->SetLocalPosition(trans);
				}

				ReadStringFromFileForCharac(pFile, pStrTocken);
				if (!strcmp(pStrTocken, "<Rotation>:"))
				{
					nReads = (UINT)::fread(&rotate, sizeof(Vec3), 1, pFile);
					_rotate.push_back(rotate);
					//obj->GetTransform()->SetLocalRotation(rotate);
				}

				ReadStringFromFileForCharac(pFile, pStrTocken);
				if (!strcmp(pStrTocken, "<Scale>:"))
				{
					nReads = (UINT)::fread(&scale, sizeof(Vec3), 1, pFile);
					scale = Vec3(10.f, 10.f, 10.f);
					_scale.push_back(scale);
					//obj->GetTransform()->SetLocalScale(scale);
					//scene->AddGameObject(obj);
					break;
				}
			}
		}
	}
}

void SceneManager::LoadMap(shared_ptr<Scene> scene)
{
	for (int i = 0; i < 4; ++i)
	{
		vector<shared_ptr<GameObject>> object = GET_SINGLE(Resources)->LoadFBX(_objectName[i] + L".bin")->Instantiate();
		for (auto& gameObj : object)
		{
			gameObj->SetName(_objectName[i] + L"1");
			gameObj->SetCheckFrustum(false);
			gameObj->GetTransform()->SetLocalPosition(_transform[i]);
			gameObj->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));
			gameObj->GetTransform()->SetLocalRotation(_rotate[i]);
			scene->AddGameObject(gameObj);
		}
	}
}
