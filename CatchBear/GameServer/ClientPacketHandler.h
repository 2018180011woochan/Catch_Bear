#pragma once
#include "Protocol.pb.h"

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : uint16
{
	PKT_C_LOGIN = 1000,
	PKT_S_LOGIN = 1001,
	PKT_C_ENTER_LOBBY = 1002,
	PKT_S_ENTER_LOBBY = 1003,
	PKT_C_ENTER_GAME = 1004,
	PKT_S_ENTER_GAME = 1005,
	PKT_C_CHAT = 1006,
	PKT_S_CHAT = 1007,
	PKT_C_MOVE = 1008,
	PKT_S_MOVE = 1009,
	PKT_C_USE_DEBUFITEM = 1010,
	PKT_S_USE_DEBUFITEM = 1011,
	PKT_C_USE_STUN = 1012,
	PKT_S_USE_STUN = 1013,
	PKT_C_COLLIDPLAYERTOPLAYER = 1014,
	PKT_S_COLLIDPLAYERTOPLAYER = 1015,
	PKT_C_PLAYERINFO = 1016,
	PKT_S_PLAYERINFO = 1017,
	PKT_C_STATE = 1018,
	PKT_S_STATE = 1019,
};

// Custom Handlers
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);
bool Handle_C_LOGIN(PacketSessionRef& session, Protocol::C_LOGIN& pkt);
bool Handle_C_ENTER_LOBBY(PacketSessionRef& session, Protocol::C_ENTER_LOBBY& pkt);
bool Handle_C_ENTER_GAME(PacketSessionRef& session, Protocol::C_ENTER_GAME& pkt);
bool Handle_C_CHAT(PacketSessionRef& session, Protocol::C_CHAT& pkt);
bool Handle_C_MOVE(PacketSessionRef& session, Protocol::C_MOVE& pkt);
bool Handle_C_USE_DEBUFITEM(PacketSessionRef& session, Protocol::C_USE_DEBUFITEM& pkt);
bool Handle_C_USE_STUN(PacketSessionRef& session, Protocol::C_USE_STUN& pkt);
bool Handle_C_COLLIDPLAYERTOPLAYER(PacketSessionRef& session, Protocol::C_COLLIDPLAYERTOPLAYER& pkt);
bool Handle_C_PLAYERINFO(PacketSessionRef& session, Protocol::C_PLAYERINFO& pkt);
bool Handle_C_STATE(PacketSessionRef& session, Protocol::C_STATE& pkt);

class ClientPacketHandler
{
public:
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;
		GPacketHandler[PKT_C_LOGIN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_LOGIN>(Handle_C_LOGIN, session, buffer, len); };
		GPacketHandler[PKT_C_ENTER_LOBBY] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_ENTER_LOBBY>(Handle_C_ENTER_LOBBY, session, buffer, len); };
		GPacketHandler[PKT_C_ENTER_GAME] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_ENTER_GAME>(Handle_C_ENTER_GAME, session, buffer, len); };
		GPacketHandler[PKT_C_CHAT] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_CHAT>(Handle_C_CHAT, session, buffer, len); };
		GPacketHandler[PKT_C_MOVE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_MOVE>(Handle_C_MOVE, session, buffer, len); };
		GPacketHandler[PKT_C_USE_DEBUFITEM] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_USE_DEBUFITEM>(Handle_C_USE_DEBUFITEM, session, buffer, len); };
		GPacketHandler[PKT_C_USE_STUN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_USE_STUN>(Handle_C_USE_STUN, session, buffer, len); };
		GPacketHandler[PKT_C_COLLIDPLAYERTOPLAYER] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_COLLIDPLAYERTOPLAYER>(Handle_C_COLLIDPLAYERTOPLAYER, session, buffer, len); };
		GPacketHandler[PKT_C_PLAYERINFO] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_PLAYERINFO>(Handle_C_PLAYERINFO, session, buffer, len); };
		GPacketHandler[PKT_C_STATE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_STATE>(Handle_C_STATE, session, buffer, len); };
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}
	static SendBufferRef MakeSendBuffer(Protocol::S_LOGIN& pkt) { return MakeSendBuffer(pkt, PKT_S_LOGIN); }
	static SendBufferRef MakeSendBuffer(Protocol::S_ENTER_LOBBY& pkt) { return MakeSendBuffer(pkt, PKT_S_ENTER_LOBBY); }
	static SendBufferRef MakeSendBuffer(Protocol::S_ENTER_GAME& pkt) { return MakeSendBuffer(pkt, PKT_S_ENTER_GAME); }
	static SendBufferRef MakeSendBuffer(Protocol::S_CHAT& pkt) { return MakeSendBuffer(pkt, PKT_S_CHAT); }
	static SendBufferRef MakeSendBuffer(Protocol::S_MOVE& pkt) { return MakeSendBuffer(pkt, PKT_S_MOVE); }
	static SendBufferRef MakeSendBuffer(Protocol::S_USE_DEBUFITEM& pkt) { return MakeSendBuffer(pkt, PKT_S_USE_DEBUFITEM); }
	static SendBufferRef MakeSendBuffer(Protocol::S_USE_STUN& pkt) { return MakeSendBuffer(pkt, PKT_S_USE_STUN); }
	static SendBufferRef MakeSendBuffer(Protocol::S_COLLIDPLAYERTOPLAYER& pkt) { return MakeSendBuffer(pkt, PKT_S_COLLIDPLAYERTOPLAYER); }
	static SendBufferRef MakeSendBuffer(Protocol::S_PLAYERINFO& pkt) { return MakeSendBuffer(pkt, PKT_S_PLAYERINFO); }
	static SendBufferRef MakeSendBuffer(Protocol::S_STATE& pkt) { return MakeSendBuffer(pkt, PKT_S_STATE); }

private:
	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
			return false;

		return func(session, pkt);
	}

	template<typename T>
	static SendBufferRef MakeSendBuffer(T& pkt, uint16 pktId)
	{
		const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(PacketHeader);

		SendBufferRef sendBuffer = GSendBufferManager->Open(packetSize);
		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = pktId;
		ASSERT_CRASH(pkt.SerializeToArray(&header[1], dataSize));
		sendBuffer->Close(packetSize);

		return sendBuffer;
	}
};