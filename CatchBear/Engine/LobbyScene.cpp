#include "pch.h"
#include "LobbyScene.h"
#include "Engine.h"
#include "SoundManager.h"
#include "GameObject.h"
#include "Input.h"
#include "SceneManager.h"
#include "ServerPacketHandler.h"
#include "ServerSession.h"
#include "ReadyButton.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "Texture.h"
#include "Resources.h"

LobbyScene::LobbyScene()
{
}

LobbyScene::~LobbyScene()
{
}

void LobbyScene::Awake()
{
	GET_SINGLE(SoundManager)->StopSound(SoundManager::CHANNELID::BGM);
	GET_SINGLE(SoundManager)->PlayBGM(L"lobbyScene.wav");
}

void LobbyScene::Start()
{
	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		gameObject->Start();
	}
}

void LobbyScene::Update()
{
	//GEngine->_isIPAddrEnter = true;
	// �� �κ� ��� �÷��̾ �غ�Ǹ� �Ѿ�� �ٲܰԿ�~ (����)
	if (INPUT->GetButtonDown(KEY_TYPE::ENTER)) {
		Protocol::C_ENTER_LOBBY enterLobbyPkt;
		enterLobbyPkt.set_playerid(mysession->GetPlayerID());
		enterLobbyPkt.set_playernum(GEngine->GetPlayerNum());
		enterLobbyPkt.set_isplayerready(true);
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(enterLobbyPkt);
		mysession->Send(sendBuffer);

		Protocol::C_LOBBY_STATE LobbyStatePkt;
		LobbyStatePkt.set_isready(true);
		LobbyStatePkt.set_playerid(mysession->GetPlayerID());
		sendBuffer = ServerPacketHandler::MakeSendBuffer(LobbyStatePkt);
		mysession->Send(sendBuffer);
		//GET_SINGLE(SceneManager)->LoadScene(SCENE_ID::STAGE);
	}

	CheckIsPlayerEnter();
	CheckIsPlayerReady();
}

void LobbyScene::LateUpdate()
{
	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		gameObject->LateUpdate();
	}
}

void LobbyScene::FinalUpdate()
{
	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		gameObject->FinalUpdate();
	}
}

void LobbyScene::CheckIsPlayerEnter()
{
	for (int i = 0; i < GEngine->GetPlayerNum(); ++i)
	{
		if (GET_SINGLE(SceneManager)->GetEnterLobby(i))
		{
			wstring objName = L"playerInfo" + s2ws(to_string(i+1));
			shared_ptr<GameObject> playerInfo = GET_SINGLE(SceneManager)->GetActiveScene()->GetGameObject(objName);
			shared_ptr<GameObject> startB = GET_SINGLE(SceneManager)->GetActiveScene()->GetGameObject(objName + L"_start");

			playerInfo->_isRender = true;
			startB->_isRender = true;
		}
	}
}

void LobbyScene::CheckIsPlayerReady()
{
	for (int i = 0; i < GEngine->GetPlayerNum(); ++i)
	{
		wstring objName = L"playerInfo" + s2ws(to_string(i + 1)) + L"_start";
		shared_ptr<GameObject> playerInfo = GET_SINGLE(SceneManager)->GetActiveScene()->GetGameObject(objName);
		shared_ptr<Texture> clickedTex = GET_SINGLE(Resources)->Get<Texture>(L"lobby_start_click");
		shared_ptr<Texture> normalTex = GET_SINGLE(Resources)->Get<Texture>(L"lobby_start");

		if (GET_SINGLE(SceneManager)->GetPlayerReady(i))
		{
			playerInfo->GetMeshRenderer()->GetMaterial()->SetTexture(0, clickedTex);
		}
		else
		{
			playerInfo->GetMeshRenderer()->GetMaterial()->SetTexture(0, normalTex);
		}
	}
}
