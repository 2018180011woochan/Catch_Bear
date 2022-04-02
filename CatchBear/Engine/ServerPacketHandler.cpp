#include "pch.h"
#include "ServerPacketHandler.h"
#include "SceneManager.h"
#include "Input.h"
#include "Scene.h"
#include "GameObject.h"
#include "Player.h"
#include "Transform.h"
#include "Timer.h"
#include "ServerSession.h"

#include "PlayerState.h"
#include "IdleState.h"
#include "MoveState.h"
#include "AttackState.h"
#include "DashState.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

// 직접 컨텐츠 작업자

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

	// 세션의 playerID를 저장해준다
	mysession->SetPlayerID(pkt.playerid());
	uint64 pid = mysession->GetPlayerID();

	shared_ptr<GameObject>	_player = make_shared<GameObject>();

	// 현재 씬에서 플레이어를 찾는다
	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
	const vector<shared_ptr<GameObject>>& gameObjects = scene->GetGameObjects();
	_player->SetPlayerID(pkt.playerid());

	Protocol::C_ENTER_GAME enterGamePkt;
	enterGamePkt.set_playerid(mysession->GetPlayerID());
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(enterGamePkt);
	session->Send(sendBuffer);

	//// 입장 UI 버튼 누르면 로비창 입장
	//Protocol::C_ENTER_LOBBY enterLobbyPkt;
	//enterLobbyPkt.set_playerid(GPlayer.playerId);
	//auto sendBuffer = ServerPacketHandler::MakeSendBuffer(enterLobbyPkt);
	//session->Send(sendBuffer);

	return true;
}

bool Handle_S_ENTER_LOBBY(PacketSessionRef& session, Protocol::S_ENTER_LOBBY& pkt)
{
#pragma region test
	//// 만약 모든 플레이어가 준비됐다면 C_ENTER_GAME 패킷 보냄
	//if (pkt.isallplayersready())
	//{
	//	Protocol::C_ENTER_GAME enterGamePkt;
	//	enterGamePkt.set_playerid(GPlayer.playerId);
	//	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(enterGamePkt);
	//	session->Send(sendBuffer);
	//}
	//// 준비 안됐다면 여기로
	//else
	//{
	//	string sChat;
	//	// 채팅하고싶으면 컨트롤 키 누르기 (일단 임시)
	//	cout << "채팅하고싶으면 LControl키 누르세요 / 레디하고 싶으면 스페이스를 누르세요 " << endl;
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

	//		cout << GPlayer.playerId << "번 플레이어 준비 완료!" << endl;
	//		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(enterLobbyPkt);
	//		session->Send(sendBuffer);
	//	}
	//}
#pragma endregion test
	//// 만약 모든 플레이어가 준비됐다면 C_ENTER_GAME 패킷 보냄
	//if (pkt.isallplayersready())
	//{
	//	Protocol::C_ENTER_GAME enterGamePkt;
	//	enterGamePkt.set_playerid(GPlayer.playerId);
	//	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(enterGamePkt);
	//	session->Send(sendBuffer);
	//}
	//// 준비 안됐다면 여기로
	//else
	//{
	//	Protocol::C_ENTER_LOBBY LobbyPkt;
	//	
	//	if (!GPlayer.bisPlayerReady)
	//	{
	//		// 일단은 ready를 입력하면 준비됐다구..실제 게임에선 ui를 클릭해서 ready되도록 수정할거임.
	//		string chat;
	//		cin >> chat;
	//		if (chat == "ready")
	//		{
	//			LobbyPkt.set_playerid(GPlayer.playerId);
	//			LobbyPkt.set_isplayerready(true);
	//			GPlayer.bisPlayerReady = true;
	//			cout << "ID " << GPlayer.playerId << "준비완료" << endl;
	//		}

	//		// ready 한게 아니라면 채팅으로 간주함 실제 게임에선 채팅창 UI를 클릭하면 그때 cin을 받아 채팅 내용을 입력받음
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

	Vec3 rot;
	rot.y = pkt.yrot();

	_player->GetTransform()->SetLocalPosition(pos);
	_player->GetTransform()->SetLocalRotation(rot);

	PlayerState* state;
	switch (pkt.state())
	{
	case Protocol::IDLE:
		if (!_player->_state->curState == STATE::IDLE)
		{
			state = new IdleState;
			delete _player->_state;
			_player->_state = state;
			_player->_state->Enter(*_player);
			_player->_state->curState = STATE::IDLE;
		}
		break;
	case Protocol::WALK:
		if (!_player->_state->curState == STATE::WALK)
		{
			state = new MoveState;
			delete _player->_state;
			_player->_state = state;
			_player->_state->Enter(*_player);
			_player->_state->curState = STATE::WALK;
		}
		if (pkt.iskeydown() == false)
			_player->_state->End(*_player);
		break;
	default:
		break;
	}

	return true;
}