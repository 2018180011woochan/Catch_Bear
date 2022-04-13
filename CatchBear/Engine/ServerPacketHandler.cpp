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
#include "DashRestState.h"
#include "SlowRestState.h"
#include "SlowState.h"
#include "StunState.h"
#include "JumpState.h"

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

	Protocol::C_ENTER_GAME enterGamePkt;
	enterGamePkt.set_playerid(mysession->GetPlayerID());
	enterGamePkt.set_playernum(scene->GetEnterPlayerNum());
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
	shared_ptr<GameObject>	_player = make_shared<GameObject>();

	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();

	if (pkt.success() != true)
		return false;

	if (pkt.isallplayersready()) {
		mysession->SetAllPlayerEnter();
		_player = scene->GetPlayer(pkt.taggerplayerid());
		_player->SetIsTagger(true);	
		cout << "��� �÷��̾� ���� �Ϸ�!\n";
	}
	else
		cout << "�÷��̾� ���� �����.. \n";
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

	PlayerState* state;

	// ������ �÷��̾ �ƴҶ��� �̵�, �ִϸ��̼� ����ȭ ������
	if (_player->GetPlayerID() != mysession->GetPlayerID())
	{
		_player->GetTransform()->SetLocalRotation(rot);
		_player->GetTransform()->SetLocalPosition(pos);
		
		switch (pkt.state())
		{
		case Protocol::IDLE:
			if (_player->_state->curState != STATE::IDLE)
			{
				if (_player->_state->curState != STATE::STUN)
				{
					state = new IdleState;
					delete _player->_state;
					_player->_state = state;
					_player->_state->Enter(*_player);
					_player->_state->curState = STATE::IDLE;
				}
			}
			break;
		case Protocol::WALK:
			if (_player->_state->curState != STATE::WALK)
			{
				if (_player->_state->curState != STATE::DASH)
				{
					state = new MoveState;
					delete _player->_state;
					_player->_state = state;
					_player->_state->Enter(*_player);
					_player->_state->curState = STATE::WALK;
				}
			}
			break;
		case Protocol::JUMP:
			if (_player->_state->curState != STATE::JUMP)
			{
				state = new JumpState;
				delete _player->_state;
				_player->_state = state;
				_player->_state->Enter(*_player);
				_player->_state->curState = STATE::JUMP;
			}
			break;
		case Protocol::ATTACK:
			if (static_pointer_cast<Player>(_player->GetScript(0))->_state->curState != STATE::ATTACK)
			{
				state = new AttackState;
				delete static_pointer_cast<Player>(_player->GetScript(0))->_state;
				static_pointer_cast<Player>(_player->GetScript(0))->_state = state;
				static_pointer_cast<Player>(_player->GetScript(0))->_state->Enter(*_player);
				static_pointer_cast<Player>(_player->GetScript(0))->_state->curState = STATE::ATTACK;
			}
			break;
		case Protocol::STUN:
			if (static_pointer_cast<Player>(_player->GetScript(0))->_state->curState != STATE::STUN)
			{
				state = new StunState;
				delete static_pointer_cast<Player>(_player->GetScript(0))->_state;
				static_pointer_cast<Player>(_player->GetScript(0))->_state = state;
				static_pointer_cast<Player>(_player->GetScript(0))->_state->Enter(*_player);
				static_pointer_cast<Player>(_player->GetScript(0))->_state->curState = STATE::STUN;
			}
			break;
		case Protocol::DASH:
			if (_player->_state->curState != STATE::DASH)
			{
				_player->_state->End(*_player);
				delete _player->_state;
				state = new DashState;
				_player->_state = state;
				_player->_state->Enter(*_player);
				_player->_state->curState = STATE::DASH;
			}
			break;
		case Protocol::DASHREST:
			if (_player->_state->curState != STATE::DASH_REST)
			{
				_player->_state->End(*_player);
				delete _player->_state;
				state = new DashRestState;
				_player->_state = state;
				_player->_state->Enter(*_player);
				_player->_state->curState = STATE::DASH_REST;
			}
			break;
		default:
			break;
		}
	}

	return true;
}

bool Handle_S_USE_DEBUFITEM(PacketSessionRef& session, Protocol::S_USE_DEBUFITEM& pkt)
{
	shared_ptr<GameObject>	_player = make_shared<GameObject>();
	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
	_player = scene->GetPlayer(mysession->GetPlayerID());

	switch (pkt.itemtype())
	{
	case Protocol::DEBUF_BLIND:
		static_pointer_cast<Player>(_player->GetScript(0))->SetCurItem(Player::ITEM::BLIND, true);
		break;
	case Protocol::DEBUF_SPEEDDOWN:
		static_pointer_cast<Player>(_player->GetScript(0))->SetCurItem(Player::ITEM::SPEED_DOWN, true);
		break;
	default:
		break;
	}

	return true;
}

bool Handle_S_USE_STUN(PacketSessionRef& session, Protocol::S_USE_STUN& pkt)
{
	shared_ptr<GameObject>	_player = make_shared<GameObject>();
	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
	//_player = scene->GetPlayer(mysession->GetPlayerID());

	PlayerState* state;
	for (int i = 0; i < scene->GetEnterPlayerNum(); ++i)
	{
		_player = scene->GetPlayer(i);

		if (_player->GetPlayerID() == pkt.fromplayerid())
			continue;

		static_pointer_cast<Player>(_player->GetScript(0))->SetCurItem(Player::ITEM::STUN, true);
		if (static_pointer_cast<Player>(_player->GetScript(0))->_state->curState != STATE::STUN)
		{
			state = new StunState;
			delete static_pointer_cast<Player>(_player->GetScript(0))->_state;
			static_pointer_cast<Player>(_player->GetScript(0))->_state = state;
			static_pointer_cast<Player>(_player->GetScript(0))->_state->Enter(*_player);
			static_pointer_cast<Player>(_player->GetScript(0))->_state->curState = STATE::STUN;
			static_pointer_cast<Player>(_player->GetScript(0))->SetPlayerStunned(true);
		}
		
	}
	return true;
}

bool Handle_S_COLLIDPLAYERTOPLAYER(PacketSessionRef& session, Protocol::S_COLLIDPLAYERTOPLAYER& pkt)
{
	shared_ptr<GameObject>	_player = make_shared<GameObject>();
	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
	PlayerState* state;
	// ���� �������ٸ� �����ƴϰ�
	_player = scene->GetPlayer(pkt.fromplayerid());
	_player->SetIsTagger(false);

	// �����ƴѵ� �ε����ٸ� ������
	_player = scene->GetPlayer(pkt.toplayerid());
	_player->SetIsTagger(true);
	// ���Ӱ� ������ �ƴٸ� ����
	//static_pointer_cast<Player>(_player->GetScript(0))->SetCurItem(Player::ITEM::STUN, true);
	if (static_pointer_cast<Player>(_player->GetScript(0))->_state->curState != STATE::STUN)
	{
		state = new StunState;
		delete static_pointer_cast<Player>(_player->GetScript(0))->_state;
		static_pointer_cast<Player>(_player->GetScript(0))->_state = state;
		static_pointer_cast<Player>(_player->GetScript(0))->_state->Enter(*_player);
		static_pointer_cast<Player>(_player->GetScript(0))->_state->curState = STATE::STUN;
		static_pointer_cast<Player>(_player->GetScript(0))->SetPlayerStunned(true);
	}
	return true;
}



