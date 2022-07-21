#include "pch.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ClientPacketHandler.h"
#include "Player.h"
#include "GameSession.h"
#include "Timer.h"
#include "InGame.h"
#include "Lobby.h"


PacketHandlerFunc GPacketHandler[UINT16_MAX];
std::mutex m;
int CurPlayerNum = 2;

// ���� ������ �۾���
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : Log
	return false;
}

bool Handle_C_LOGIN(PacketSessionRef& session, Protocol::C_LOGIN& pkt)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	//
	//	// TODO : Validation üũ
	//	
	Protocol::S_LOGIN loginPkt;

	loginPkt.set_success(true);

	static Atomic<uint64> idGenerator = 0;
	{
		// PlayerId�� ���� ������
		loginPkt.set_playerid(idGenerator);

		PlayerRef playerRef = MakeShared<Player>();
		playerRef->playerId = idGenerator++;
		playerRef->ownerSession = gameSession;
		GInGame.Enter(playerRef);
		cout << "�÷��̾�ID " << playerRef->playerId << " �α��� �Ϸ�!" << endl;
		//loginPkt.set_enterplayer(GInGame.GetEnterPlayerNum() - 1);
		gameSession->_player = playerRef;

		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(loginPkt);
		session->Send(sendBuffer);
		//GInGame.Broadcast(sendBuffer);
		//GInGame.mySend(sendBuffer);
	}

#pragma region �ϴܳ��߿�
//	// gameSession�� nickName ���Ϳ� ����.
//	gameSession->_nickNames.push_back(pkt.nickname());
//	cout << "�г���: " << pkt.nickname() << " ���Ӽ���!" << endl;
//
//	// GameSession�� �÷��� ������ ���� (�޸�)
//	// ID �߱� (DB ���̵� �ƴϰ�, �ΰ��� ���̵�)
//	static Atomic<uint64> idGenerator = 0;
//	{
//		// PlayerId�� ���� ������
//		loginPkt.set_playerid(idGenerator);
//
//		auto player = loginPkt.add_players();
//		player->set_nickname(pkt.nickname());
//		player->set_playertype(Protocol::PLAYER_TYPE_BEAR1);
//
//		PlayerRef playerRef = MakeShared<Player>();
//		playerRef->playerId = idGenerator++;
//		playerRef->nickname = player->nickname();
//		playerRef->type = player->playertype();
//		playerRef->ownerSession = gameSession;
//
//		gameSession->_player = playerRef;
//	}

//#pragma endregion addPlayer
//
//	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(loginPkt);
//	session->Send(sendBuffer);
#pragma endregion
	return true;
}

bool Handle_C_ENTER_LOBBY(PacketSessionRef& session, Protocol::C_ENTER_LOBBY& pkt)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	Protocol::S_ENTER_LOBBY enterLobbyPkt;

	if (GLobby.isFirstEnterLobby(pkt.playerid()))
	{
		PlayerRef player = gameSession->_player;
		player->playerId = pkt.playerid();
		GLobby.Enter(player);
		cout << "�÷��̾�ID " << pkt.playerid() << " �κ� ó�� ���ӿϷ�!" << endl;

		enterLobbyPkt.set_playerid(player->playerId);
		enterLobbyPkt.set_isallplayersready(false);
		

		for (int i = 0; i < CurPlayerNum; ++i)		// ĳġ����� 3�ΰ����̴ϱ� ���� �˻��Ѵ�
		{
			if (!GLobby.isFirstEnterLobby(i))
			{
				enterLobbyPkt.set_playerid(i);
				auto sendBuffer = ClientPacketHandler::MakeSendBuffer(enterLobbyPkt);
				GLobby.Broadcast(sendBuffer);
			}
		}

		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(enterLobbyPkt);		
		GLobby.Broadcast(sendBuffer);
		//session->Send(sendBuffer);
		//GLobby.Broadcast(sendBuffer);
	}
	else
	{
		if (pkt.isplayerready())
		{
			cout << "�÷��̾� " << pkt.playerid() << " �غ�Ϸ�\n";
			GLobby.SetPlayerReady(pkt.playerid(), true);
		}

		// ���� ��� �÷��̾ �غ�ƴٸ�
		if (GLobby.isAllPlayerReady())
		{
			cout << "��� �÷��̾ �غ��\n";
			enterLobbyPkt.set_isallplayersready(true);
			auto sendBuffer = ClientPacketHandler::MakeSendBuffer(enterLobbyPkt);
			GLobby.Broadcast(sendBuffer);
		}
		// �Ѹ��� �÷��̾�� ���������ʾ�����
		else
		{
			cout << "��� �÷��̾ �غ���� ����\n";
			enterLobbyPkt.set_isallplayersready(false);
			auto sendBuffer = ClientPacketHandler::MakeSendBuffer(enterLobbyPkt);
			session->Send(sendBuffer);
		}
	}
	return true;
}

bool Handle_C_LOBBY_STATE(PacketSessionRef& session, Protocol::C_LOBBY_STATE& pkt)
{
	Protocol::S_LOBBY_STATE LobbyStatePkt;
	GLobby.SetPlayerReady(pkt.playerid(), pkt.isready());
	GLobby.SetPlayerType(pkt.playerid(), pkt.playertype());

	LobbyStatePkt.set_playerid(pkt.playerid());
	LobbyStatePkt.set_isready(GLobby.GetPlayerReady(pkt.playerid()));
	LobbyStatePkt.set_playertype(GLobby.GetPlayerType(pkt.playerid()));
	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(LobbyStatePkt);
	GLobby.Broadcast(sendBuffer);

	for (int i = 0; i < CurPlayerNum; ++i)
	{
		if (GLobby.GetPlayerReady(i))		
		{
			LobbyStatePkt.set_playerid(i);
			LobbyStatePkt.set_isready(true);
			LobbyStatePkt.set_playertype(GLobby.GetPlayerType(i));
			auto sendBuffer = ClientPacketHandler::MakeSendBuffer(LobbyStatePkt);
			GLobby.Broadcast(sendBuffer);	
		}		
	}

	// ���� ��� �÷��̾ �غ�ƴٸ�
	if (GLobby.isAllPlayerReady())
	{
		Protocol::S_ENTER_LOBBY enterLobbyPkt;
		cout << "��� �÷��̾ �غ��\n";
		enterLobbyPkt.set_isallplayersready(true);
		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(enterLobbyPkt);
		GLobby.Broadcast(sendBuffer);
	}
	
	return true;
}

bool Handle_C_ENTER_GAME(PacketSessionRef& session, Protocol::C_ENTER_GAME& pkt)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);

	cout << "�÷��̾� " << pkt.playerid() << " �ΰ��� ������\n";

	Protocol::S_ENTER_GAME enterGamePkt;
	enterGamePkt.set_success(true);
	enterGamePkt.set_isallplayersready(false);
	enterGamePkt.set_playerid(pkt.playerid());

	srand((unsigned int)time(NULL));
	// ���� �̱�� ����
	int tagger = rand();

	switch (pkt.playernum())
	{
	case 1:
		if (pkt.playerid() == 0) {		// 1�ο� �����϶�
			enterGamePkt.set_isallplayersready(true);
			enterGamePkt.set_taggerplayerid(0);
		}
		break;
	case 2:
		if (pkt.playerid() == 1) {		// 2�ο� �����϶�
			enterGamePkt.set_isallplayersready(true);
			enterGamePkt.set_taggerplayerid(tagger % 2);
		}
		break;
	case 3:
		if (pkt.playerid() == 2) {		// 3�ο� �����϶� (���������δ� �̰Ÿ� ���� �ɵ�)
			enterGamePkt.set_isallplayersready(true);
			enterGamePkt.set_taggerplayerid(tagger % 3);
		}
		break;
	}

	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(enterGamePkt);
	//session->Send(sendBuffer);
	GInGame.Broadcast(sendBuffer);
	return true;
}

bool Handle_C_CHAT(PacketSessionRef& session, Protocol::C_CHAT& pkt)
{
	//uint64 playerId = pkt.playerid();

	//std::cout << "ID: " << playerId << ") " << pkt.msg() << endl;

	//Protocol::S_CHAT chatPkt;
	//chatPkt.set_playerid(playerId);
	//chatPkt.set_msg(pkt.msg());
	//auto sendBuffer = ClientPacketHandler::MakeSendBuffer(chatPkt);

	//GLobby.Broadcast(sendBuffer); // WRITE_LOCK

	return true;
}

bool Handle_C_MOVE(PacketSessionRef& session, Protocol::C_MOVE& pkt)
{
	Protocol::S_MOVE movePkt;

	movePkt.set_playerid(pkt.playerid());
	movePkt.set_xpos(pkt.xpos());
	movePkt.set_ypos(pkt.ypos());
	movePkt.set_zpos(pkt.zpos());
	movePkt.set_yrot(pkt.yrot());

	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(movePkt);

	GInGame.Broadcast(sendBuffer);
	return true;
}

bool Handle_C_USE_DEBUFITEM(PacketSessionRef& session, Protocol::C_USE_DEBUFITEM& pkt)
{
	Protocol::S_USE_DEBUFITEM s_pkt;
	s_pkt.set_itemtype(pkt.itemtype());
	s_pkt.set_fromplayerid(pkt.fromplayerid());
	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(s_pkt);
	//GInGame.Broadcast(sendBuffer);
	GInGame.ExceptBroadcast(pkt.fromplayerid(), sendBuffer);

	return true;
}

bool Handle_C_USE_STUN(PacketSessionRef& session, Protocol::C_USE_STUN& pkt)
{
	Protocol::S_USE_STUN s_pkt;
	s_pkt.set_fromplayerid(pkt.fromplayerid());
	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(s_pkt);
	GInGame.Broadcast(sendBuffer);
	return true;
}

bool Handle_C_COLLIDPLAYERTOPLAYER(PacketSessionRef& session, Protocol::C_COLLIDPLAYERTOPLAYER& pkt)
{
	Protocol::S_COLLIDPLAYERTOPLAYER collidpkt;
	collidpkt.set_fromplayerid(pkt.fromplayerid());
	collidpkt.set_toplayerid(pkt.toplayerid());
	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(collidpkt);
	GInGame.Broadcast(sendBuffer);

	return true;
}

bool Handle_C_PLAYERINFO(PacketSessionRef& session, Protocol::C_PLAYERINFO& pkt)
{
	Protocol::S_PLAYERINFO Playerinfopkt;

	Playerinfopkt.set_score(pkt.score());
	Playerinfopkt.set_timer(pkt.timer());
	Playerinfopkt.set_playerid(pkt.playerid());

	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(Playerinfopkt);
	GInGame.Broadcast(sendBuffer);
	return true;
}

bool Handle_C_STATE(PacketSessionRef& session, Protocol::C_STATE& pkt)
{
	Protocol::S_STATE StatePkt;
	StatePkt.set_playerid(pkt.playerid());
	StatePkt.set_state(pkt.state());

	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(StatePkt);

	GInGame.Broadcast(sendBuffer);
	return true;
}

bool Handle_C_PLUSTIME(PacketSessionRef& session, Protocol::C_PLUSTIME& pkt)
{
	Protocol::S_PLUSTIME timepkt;
	timepkt.set_playerid(pkt.playerid());

	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(timepkt);
	GInGame.OnlySendPlayer(pkt.playerid(),sendBuffer);
	return true;
}

bool Handle_C_STUNEND(PacketSessionRef& session, Protocol::C_STUNEND& pkt)
{
	Protocol::S_STUNEND StatePkt;
	StatePkt.set_playerid(pkt.playerid());

	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(StatePkt);

	GInGame.Broadcast(sendBuffer);
	return true;
}
