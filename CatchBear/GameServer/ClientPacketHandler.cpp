#include "pch.h"
#include "ClientPacketHandler.h"
//#include "Player.h"
//#include "Lobby.h"
#include "GameSession.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

// ���� ������ �۾���
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : Log
	return false;
}

bool Handle_C_LOGIN(PacketSessionRef& session, Protocol::C_LOGIN& pkt)
{
//	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
//
//	// TODO : Validation üũ
//	
//	Protocol::S_LOGIN loginPkt;
//	// ������ζ�� �����ͺ��̽��� �����Ͽ� ��ȿ�� ���̵����� Ȯ���ϰ� ���� ��Ŷ�� �����߰���?
//	loginPkt.set_success(true);
//
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
//#pragma region addPlayer
//	/*{
//		auto player = loginPkt.add_players();
//		player->set_name(u8"DB�����ܾ���̸�1");
//		player->set_playertype(Protocol::PLAYER_TYPE_KNIGHT);
//
//		PlayerRef playerRef = MakeShared<Player>();
//		playerRef->playerId = idGenerator++;
//		playerRef->name = player->name();
//		playerRef->type = player->playertype();
//		playerRef->ownerSession = gameSession;
//		
//		gameSession->_players.push_back(playerRef);
//	}
//
//	{
//		auto player = loginPkt.add_players();
//		player->set_name(u8"DB�����ܾ���̸�2");
//		player->set_playertype(Protocol::PLAYER_TYPE_MAGE);
//
//		PlayerRef playerRef = MakeShared<Player>();
//		playerRef->playerId = idGenerator++;
//		playerRef->name = player->name();
//		playerRef->type = player->playertype();
//		playerRef->ownerSession = gameSession;
//
//		gameSession->_players.push_back(playerRef);
//	}*/
//#pragma endregion addPlayer
//
//	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(loginPkt);
//	session->Send(sendBuffer);

	return true;
}

bool Handle_C_ENTER_LOBBY(PacketSessionRef& session, Protocol::C_ENTER_LOBBY& pkt)
{
	//GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	//Protocol::S_ENTER_LOBBY enterLobbyPkt;
	//if (GLobby.isFirstEnterLobby(pkt.playerid()))
	//{
	//	PlayerRef player = gameSession->_player;
	//	player->playerId = pkt.playerid();
	//	GLobby.Enter(player);
	//	cout << "�÷��̾�ID " << pkt.playerid() << " �κ� ���ӿϷ�!" << endl;

	//	enterLobbyPkt.set_isallplayersready(false);

	//	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(enterLobbyPkt);
	//	session->Send(sendBuffer);
	//}
	//else
	//{
	//	if (pkt.isplayerready())
	//		GLobby.SetPlayerReady(pkt.playerid());

	//	// ���� ��� �÷��̾ �غ�ƴٸ�
	//	if (GLobby.isAllPlayerReady())					
	//		enterLobbyPkt.set_isallplayersready(true);
	//	// �Ѹ��� �÷��̾�� ���������ʾ�����
	//	else
	//		enterLobbyPkt.set_isallplayersready(false);

	//	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(enterLobbyPkt);
	//	session->Send(sendBuffer);
	//}
	return true;
}

bool Handle_C_ENTER_GAME(PacketSessionRef& session, Protocol::C_ENTER_GAME& pkt)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);

	//cout << "ID " << pkt.playerid() << " �ΰ��� ���� �Ϸ�" << endl;

	////uint64 index = pkt.playerindex();
	////// TODO : Validation

	//PlayerRef player = gameSession->_player; // READ_ONLY?
	////GLobby.Enter(player); // WRITE_LOCK

	Protocol::S_ENTER_GAME enterGamePkt;
	enterGamePkt.set_success(true);
	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(enterGamePkt);
	// ���߿��� �÷��̾�� ���ʼ��� ���ؼ� �ű⼭ Send�ϴ°ɷ� �����ؾ��ҵ�
	//player->ownerSession->Send(sendBuffer);
	session->Send(sendBuffer);

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

// �浹�� ���� AABB�˻�
bool CheckAABB(float AX, float AZ, float BX, float BZ, float AWidth, float BWidth, float ADepth, float BDepth);

bool Handle_C_MOVE(PacketSessionRef& session, Protocol::C_MOVE& pkt)
{
	// �Ŀ��� static������Ʈ�� ��ġ �ҷ��ͼ� �÷��̾�� �浹üũ��
	// �ϴ��� �׽�Ʈ������ object��ġ
	float staticObjX = 0.f;
	float staticObjZ = 700.f;
	float staticObjWidth = 100.f;
	float staticObjDepth = 100.f;

	if (pkt.movedir() == 0)
		cout << "������ �̵�" << endl;
	if (pkt.movedir() == 1)
		cout << "������ �̵�" << endl;
	if (pkt.movedir() == 2)
		cout << "�������� �̵�" << endl;
	if (pkt.movedir() == 3)
		cout << "���������� �̵�" << endl;

	cout << "x: " << pkt.xpos() << " / z: " << pkt.zpos() << endl;

	Protocol::S_MOVE movePkt;

	// ���⼭ �浹üũ�ϰ� ������ �̵� �����ϸ� �̵����ɿ��θ� ������
	bool isCollid = CheckAABB(pkt.xpos(), pkt.zpos(), staticObjX, staticObjZ, 50.f, staticObjWidth, 50.f, staticObjDepth);

	// �̵� ����
	if (isCollid) {
		movePkt.set_success(false);
		cout << "�浹��!" << endl;
	}
	else
		movePkt.set_success(true);

	movePkt.set_movedir(pkt.movedir());

	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(movePkt);
	session->Send(sendBuffer);
	return true;
}

bool CheckAABB(float AX, float AZ, float BX, float BZ, float AWidth, float BWidth, float ADepth, float BDepth)
{
	float ALeft = AX - (AWidth / 2);
	float BLeft = BX - (BWidth / 2);
	float AFront = AZ - (ADepth / 2);
	float BFront = BZ - (BDepth / 2);

	if (BLeft + BWidth > ALeft && ALeft + AWidth > BLeft
		&& BFront + BDepth > AFront && AFront + ADepth > BFront)
	{
		return true;
	}
	return false;
}