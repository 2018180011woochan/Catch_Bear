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
	PKT_C_PLUSTIME = 1020,
	PKT_S_PLUSTIME = 1021,
};

// Custom Handlers
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);
bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& pkt);
bool Handle_S_ENTER_LOBBY(PacketSessionRef& session, Protocol::S_ENTER_LOBBY& pkt);
bool Handle_S_ENTER_GAME(PacketSessionRef& session, Protocol::S_ENTER_GAME& pkt);
bool Handle_S_CHAT(PacketSessionRef& session, Protocol::S_CHAT& pkt);
bool Handle_S_MOVE(PacketSessionRef& session, Protocol::S_MOVE& pkt);
bool Handle_S_USE_DEBUFITEM(PacketSessionRef& session, Protocol::S_USE_DEBUFITEM& pkt);
bool Handle_S_USE_STUN(PacketSessionRef& session, Protocol::S_USE_STUN& pkt);
bool Handle_S_COLLIDPLAYERTOPLAYER(PacketSessionRef& session, Protocol::S_COLLIDPLAYERTOPLAYER& pkt);
bool Handle_S_PLAYERINFO(PacketSessionRef& session, Protocol::S_PLAYERINFO& pkt);
bool Handle_S_STATE(PacketSessionRef& session, Protocol::S_STATE& pkt);
bool Handle_S_PLUSTIME(PacketSessionRef& session, Protocol::S_PLUSTIME& pkt);

class ServerPacketHandler
{
public:
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;
		GPacketHandler[PKT_S_LOGIN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_LOGIN>(Handle_S_LOGIN, session, buffer, len); };
		GPacketHandler[PKT_S_ENTER_LOBBY] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_ENTER_LOBBY>(Handle_S_ENTER_LOBBY, session, buffer, len); };
		GPacketHandler[PKT_S_ENTER_GAME] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_ENTER_GAME>(Handle_S_ENTER_GAME, session, buffer, len); };
		GPacketHandler[PKT_S_CHAT] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_CHAT>(Handle_S_CHAT, session, buffer, len); };
		GPacketHandler[PKT_S_MOVE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_MOVE>(Handle_S_MOVE, session, buffer, len); };
		GPacketHandler[PKT_S_USE_DEBUFITEM] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_USE_DEBUFITEM>(Handle_S_USE_DEBUFITEM, session, buffer, len); };
		GPacketHandler[PKT_S_USE_STUN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_USE_STUN>(Handle_S_USE_STUN, session, buffer, len); };
		GPacketHandler[PKT_S_COLLIDPLAYERTOPLAYER] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_COLLIDPLAYERTOPLAYER>(Handle_S_COLLIDPLAYERTOPLAYER, session, buffer, len); };
		GPacketHandler[PKT_S_PLAYERINFO] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_PLAYERINFO>(Handle_S_PLAYERINFO, session, buffer, len); };
		GPacketHandler[PKT_S_STATE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_STATE>(Handle_S_STATE, session, buffer, len); };
		GPacketHandler[PKT_S_PLUSTIME] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_PLUSTIME>(Handle_S_PLUSTIME, session, buffer, len); };
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}
	static SendBufferRef MakeSendBuffer(Protocol::C_LOGIN& pkt) { return MakeSendBuffer(pkt, PKT_C_LOGIN); }
	static SendBufferRef MakeSendBuffer(Protocol::C_ENTER_LOBBY& pkt) { return MakeSendBuffer(pkt, PKT_C_ENTER_LOBBY); }
	static SendBufferRef MakeSendBuffer(Protocol::C_ENTER_GAME& pkt) { return MakeSendBuffer(pkt, PKT_C_ENTER_GAME); }
	static SendBufferRef MakeSendBuffer(Protocol::C_CHAT& pkt) { return MakeSendBuffer(pkt, PKT_C_CHAT); }
	static SendBufferRef MakeSendBuffer(Protocol::C_MOVE& pkt) { return MakeSendBuffer(pkt, PKT_C_MOVE); }
	static SendBufferRef MakeSendBuffer(Protocol::C_USE_DEBUFITEM& pkt) { return MakeSendBuffer(pkt, PKT_C_USE_DEBUFITEM); }
	static SendBufferRef MakeSendBuffer(Protocol::C_USE_STUN& pkt) { return MakeSendBuffer(pkt, PKT_C_USE_STUN); }
	static SendBufferRef MakeSendBuffer(Protocol::C_COLLIDPLAYERTOPLAYER& pkt) { return MakeSendBuffer(pkt, PKT_C_COLLIDPLAYERTOPLAYER); }
	static SendBufferRef MakeSendBuffer(Protocol::C_PLAYERINFO& pkt) { return MakeSendBuffer(pkt, PKT_C_PLAYERINFO); }
	static SendBufferRef MakeSendBuffer(Protocol::C_STATE& pkt) { return MakeSendBuffer(pkt, PKT_C_STATE); }
	static SendBufferRef MakeSendBuffer(Protocol::C_PLUSTIME& pkt) { return MakeSendBuffer(pkt, PKT_C_PLUSTIME); }

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