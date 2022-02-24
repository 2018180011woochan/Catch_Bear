#include "pch.h"
#include "ServerPacketHandler.h"
#include "Player.h"
#include "KeyManager.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

// ���� ������ �۾���

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : Log
	return false;
}

bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& pkt)
{
	if (pkt.success() == false)
		return true;

	GPlayer.playerId = pkt.playerid();
	cout << "�� �÷��̾��� �г����� " << GPlayer.name << "�̸� ";
	cout << "�� Ŭ���̾�Ʈ�� ID�� " << GPlayer.playerId << "�Դϴ�!" << endl;
	
	if (pkt.players().size() == 0)
	{
		// ĳ���� ����â
	}
	
	// ���� UI ��ư ������ �κ�â ����
	Protocol::C_ENTER_LOBBY enterLobbyPkt;
	enterLobbyPkt.set_playerid(GPlayer.playerId);
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(enterLobbyPkt);
	session->Send(sendBuffer);

	return true;
}

bool Handle_S_ENTER_LOBBY(PacketSessionRef& session, Protocol::S_ENTER_LOBBY& pkt)
{
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

	return true;
}

bool Handle_S_ENTER_GAME(PacketSessionRef& session, Protocol::S_ENTER_GAME& pkt)
{
	// TODO
	return true;
}

bool Handle_S_CHAT(PacketSessionRef& session, Protocol::S_CHAT& pkt)
{
	std::cout << pkt.msg() << endl;
	return true;
}
