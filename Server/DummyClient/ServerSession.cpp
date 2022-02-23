#include "pch.h"
#include "ServerSession.h"
#include "ServerPacketHandler.h"
#include "Player.h"

static bool isConnected = false;

void ServerSession::OnConnected()
{
	Protocol::C_LOGIN pkt;

	// ������ ����Ǹ� �÷��̾� �г��� ������
	
	if (!isConnected) {
		string sNickName;
		cout << "�÷��̾� �г����� �Է��ϼ���: ";
		cin >> sNickName;
		pkt.set_nickname(sNickName);
		GPlayer.name = sNickName;

		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
		Send(sendBuffer);
		isConnected = true;
	}
}

void ServerSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketSessionRef session = GetPacketSessionRef();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	// TODO : packetId �뿪 üũ
	ServerPacketHandler::HandlePacket(session, buffer, len);
}

void ServerSession::OnSend(int32 len)
{
	//cout << "OnSend Len = " << len << endl;
}

void ServerSession::OnDisconnected()
{
	//cout << "Disconnected" << endl;
}
