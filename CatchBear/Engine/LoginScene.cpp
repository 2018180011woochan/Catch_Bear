#include "pch.h"
#include "LoginScene.h"
#include "Engine.h"
#include "SoundManager.h"
#include "GameObject.h"
#include "Input.h"
#include "SceneManager.h"
#include "Engine.h"

LoginScene::LoginScene()
{
}

LoginScene::~LoginScene()
{
}

void LoginScene::Awake()
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

	//GET_SINGLE(SoundManager)->PlayBGM(L"logo.mp3");
}

void LoginScene::Start()
{
	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		gameObject->Start();
	}
}

void LoginScene::Update()
{
	if (INPUT->GetButtonDown(KEY_TYPE::ENTER)) {
		GET_SINGLE(SceneManager)->LoadScene(SCENE_ID::STAGE);
		GEngine->_isIPAddrEnter = true;
	}
}

void LoginScene::LateUpdate()
{
	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		gameObject->LateUpdate();
	}
}

void LoginScene::FinalUpdate()
{
	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		gameObject->FinalUpdate();
	}
}
