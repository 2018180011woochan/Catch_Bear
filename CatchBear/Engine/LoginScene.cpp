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
	// 순회만 한다
	// shared_ptr: 어떤 물체를 복사하게 되면 내부적으로 RC가 1이 증가된다
	// shared_ptr이라고 해도 &로 받게 되면 그 RC가 1 증가시키지 않는다
	// gameObject를 복사해서 다른 곳으로 넘기지 않을거니까 
	// 복사하기보다는 &로 들고 있으면 성능적으로 더 좋은 향상을 얻을 수 있다
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
