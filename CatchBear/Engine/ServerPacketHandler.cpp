#include "pch.h"
#include "ServerPacketHandler.h"
#include "SceneManager.h"
#include "Input.h"
#include "Scene.h"
#include "GameObject.h"
#include "Player.h"
#include "Transform.h"
#include "Timer.h"
//#include "Session.h"
#include "ServerSession.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

// ���� ������ �۾���

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : Log
	return false;
}

static uint64 enterPlayerIndex = 0;
bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& pkt)
{
	if (pkt.success() == false)
		return true;

	//SceneManager::GetInstance()->MakePlayer(pkt.playerid());

	// ������ playerID�� �������ش�
	mysession->SetPlayerID(pkt.playerid());
	uint64 pid = mysession->GetPlayerID();

	shared_ptr<GameObject>	_player = make_shared<GameObject>();

	// ���� ������ �÷��̾ ã�´�
	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
	const vector<shared_ptr<GameObject>>& gameObjects = scene->GetGameObjects();
	_player->SetPlayerID(pkt.playerid());

	//for (auto& gameObject : gameObjects)
	//{
	//	if (gameObject->GetName() == L"Player" && gameObject->GetPlayerID() == pkt.playerid())
	//	{
	//		_player = gameObject;
	//		break;
	//	}
	//}

	//uint64 enterPlayernum = pkt.enterplayer();

	//if (enterPlayerIndex != enterPlayernum)
	//{
	//	if (enterPlayernum == 1)	// 2�� ����
	//	{
	//		if (_player->GetPlayerID() == 0)
	//			SceneManager::GetInstance()->MakePlayer(1);		// 0�� �÷��̾��϶� 1�� �÷��̾� �����
	//		if (_player->GetPlayerID() == 1)
	//			SceneManager::GetInstance()->MakePlayer(0);		// 1�� �÷��̾��϶� 0�� �÷��̾� �����
	//	}
	//	if (enterPlayernum == 2)	// 3�� ����
	//	{
	//		if (_player->GetPlayerID() == 0)
	//			SceneManager::GetInstance()->MakePlayer(2);		// 0�� �÷��̾��϶� 2�� �÷��̾� �����
	//		if (_player->GetPlayerID() == 1)
	//			SceneManager::GetInstance()->MakePlayer(2);		// 1�� �÷��̾��϶� 2�� �÷��̾� �����
	//		if (_player->GetPlayerID() == 2) {
	//			SceneManager::GetInstance()->MakePlayer(0);		// 2�� �÷��̾��϶� 0,1�� �÷��̾� �����
	//			SceneManager::GetInstance()->MakePlayer(1);
	//		}
	//	}
	//	enterPlayerIndex = enterPlayernum;
	//}

	Protocol::C_ENTER_GAME enterGamePkt;
	enterGamePkt.set_playerid(mysession->GetPlayerID());
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(enterGamePkt);
	session->Send(sendBuffer);

	//// ���� UI ��ư ������ �κ�â ����
	//Protocol::C_ENTER_LOBBY enterLobbyPkt;
	//enterLobbyPkt.set_playerid(GPlayer.playerId);
	//auto sendBuffer = ServerPacketHandler::MakeSendBuffer(enterLobbyPkt);
	//session->Send(sendBuffer);

	return true;
}

bool Handle_S_ENTER_LOBBY(PacketSessionRef& session, Protocol::S_ENTER_LOBBY& pkt)
{
#pragma region test
	//// ���� ��� �÷��̾ �غ�ƴٸ� C_ENTER_GAME ��Ŷ ����
	//if (pkt.isallplayersready())
	//{
	//	Protocol::C_ENTER_GAME enterGamePkt;
	//	enterGamePkt.set_playerid(GPlayer.playerId);
	//	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(enterGamePkt);
	//	session->Send(sendBuffer);
	//}
	//// �غ� �ȵƴٸ� �����
	//else
	//{
	//	string sChat;
	//	// ä���ϰ������ ��Ʈ�� Ű ������ (�ϴ� �ӽ�)
	//	cout << "ä���ϰ������ LControlŰ �������� / �����ϰ� ������ �����̽��� �������� " << endl;
	//	if (CKeyManager::Get_Instance()->Key_Down(VK_LCONTROL))
	//	{
	//		cin >> sChat;
	//		Protocol::C_CHAT chatPkt;
	//		chatPkt.set_msg(sChat);
	//		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(chatPkt);
	//		session->Send(sendBuffer);
	//	}
	//	if (CKeyManager::Get_Instance()->Key_Down(VK_SPACE))
	//	{
	//		Protocol::C_ENTER_LOBBY enterLobbyPkt;
	//		enterLobbyPkt.set_playerid(GPlayer.playerId);
	//		enterLobbyPkt.set_isplayerready(true);

	//		cout << GPlayer.playerId << "�� �÷��̾� �غ� �Ϸ�!" << endl;
	//		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(enterLobbyPkt);
	//		session->Send(sendBuffer);
	//	}
	//}
#pragma endregion test
	//// ���� ��� �÷��̾ �غ�ƴٸ� C_ENTER_GAME ��Ŷ ����
	//if (pkt.isallplayersready())
	//{
	//	Protocol::C_ENTER_GAME enterGamePkt;
	//	enterGamePkt.set_playerid(GPlayer.playerId);
	//	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(enterGamePkt);
	//	session->Send(sendBuffer);
	//}
	//// �غ� �ȵƴٸ� �����
	//else
	//{
	//	Protocol::C_ENTER_LOBBY LobbyPkt;
	//	
	//	if (!GPlayer.bisPlayerReady)
	//	{
	//		// �ϴ��� ready�� �Է��ϸ� �غ�ƴٱ�..���� ���ӿ��� ui�� Ŭ���ؼ� ready�ǵ��� �����Ұ���.
	//		string chat;
	//		cin >> chat;
	//		if (chat == "ready")
	//		{
	//			LobbyPkt.set_playerid(GPlayer.playerId);
	//			LobbyPkt.set_isplayerready(true);
	//			GPlayer.bisPlayerReady = true;
	//			cout << "ID " << GPlayer.playerId << "�غ�Ϸ�" << endl;
	//		}

	//		// ready �Ѱ� �ƴ϶�� ä������ ������ ���� ���ӿ��� ä��â UI�� Ŭ���ϸ� �׶� cin�� �޾� ä�� ������ �Է¹���
	//		else
	//		{
	//			Protocol::C_CHAT chatPkt;
	//			chatPkt.set_playerid(GPlayer.playerId);
	//			chatPkt.set_msg(chat);
	//			auto sendBuffer = ServerPacketHandler::MakeSendBuffer(chatPkt);
	//			session->Send(sendBuffer);
	//		}
	//	}
	//	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(LobbyPkt);
	//	session->Send(sendBuffer);
	//}
	return true;
}

bool Handle_S_ENTER_GAME(PacketSessionRef& session, Protocol::S_ENTER_GAME& pkt)
{
	if (pkt.success() != true)
		return false;
	return true;
}

bool Handle_S_CHAT(PacketSessionRef& session, Protocol::S_CHAT& pkt)
{
	//uint64 playerId = pkt.playerid();

	//std::cout << "ID: " << playerId << ") " << pkt.msg() << endl;

	////std::cout << pkt.msg() << endl;
	return true;
}

bool Handle_S_MOVE(PacketSessionRef& session, Protocol::S_MOVE& pkt)
{
	shared_ptr<GameObject>	_player = make_shared<GameObject>();

	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
	_player = scene->GetPlayer(pkt.playerid());

	Vec3 pos;
	pos.x = pkt.xpos();
	pos.y = pkt.ypos();
	pos.z = pkt.zpos();

	//MovePkt.set_playerid(pkt.playerid());

	_player->GetTransform()->SetLocalPosition(pos);

	return true;
}
