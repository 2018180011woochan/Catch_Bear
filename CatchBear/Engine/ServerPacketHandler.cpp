#include "pch.h"
#include "ServerPacketHandler.h"
#include "SceneManager.h"
#include "ItemSlotManager.h"
#include "Input.h"
#include "Scene.h"
#include "GameObject.h"
#include "Player.h"
#include "Transform.h"
#include "Timer.h"
#include "TagMark.h"
#include "ServerSession.h"
#include "Engine.h"

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

#include "Resources.h"
#include "MeshRenderer.h"

#include "FontDevice.h"

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

	// 세션의 playerID를 저장해준다
	mysession->SetPlayerID(pkt.playerid());

	// 현재 씬에서 플레이어를 찾는다
	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
	const vector<shared_ptr<GameObject>>& gameObjects = scene->GetGameObjects();

	cout << "Player " << mysession->GetPlayerID() << " 로그인 성공\n";

	Protocol::C_ENTER_LOBBY enterLobbyPkt;
	enterLobbyPkt.set_playerid(mysession->GetPlayerID());
	enterLobbyPkt.set_playernum(scene->GetEnterPlayerNum());
	enterLobbyPkt.set_isplayerready(false);
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(enterLobbyPkt);
	session->Send(sendBuffer);


	return true;
}

bool Handle_S_ENTER_LOBBY(PacketSessionRef& session, Protocol::S_ENTER_LOBBY& pkt)
{
	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();

	GET_SINGLE(SceneManager)->SetEnterLobby(int(pkt.playerid()), true);
	//cout << "Player " << mysession->GetPlayerID() << " 로비 입장\n";
#pragma region test
	// 만약 모든 플레이어가 준비됐다면 C_ENTER_GAME 패킷 보냄
	if (pkt.isallplayersready())
	{	
		GEngine->SetIsAllPlayerReady();
		GET_SINGLE(SceneManager)->LoadScene(SCENE_ID::STAGE);

		Protocol::C_ENTER_GAME enterGamePkt;
		enterGamePkt.set_playerid(mysession->GetPlayerID());
		enterGamePkt.set_playernum(GEngine->GetPlayerNum());
		//enterGamePkt.set_playernum(scene->GetEnterPlayerNum());
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(enterGamePkt);
		session->Send(sendBuffer);

	}
	
	return true;
}

bool Handle_S_LOBBY_STATE(PacketSessionRef& session, Protocol::S_LOBBY_STATE& pkt)
{
	GET_SINGLE(SceneManager)->SetPlayerReady(int(pkt.playerid()), pkt.isready());
	GET_SINGLE(SceneManager)->SetPlayerType(int(pkt.playerid()), (PLAYER_TYPE)pkt.playertype());

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

		scene->SetCurTime(0.f);

		if (_player == nullptr) 
		{
			cout << "술래 플레이어 정해지지 않음\n";
			return true;
		}
		_player->SetIsTagger(true);	

		static_pointer_cast<Player>(_player->GetScript(0))->SetPlayerScore(0);
		cout << "모든 플레이어 접속 완료!\n";

		scene->_FinalPlayerEnter = true;
		//cout << "술래는 " << pkt.taggerplayerid() << "번 플레이어입니다!" << endl;
	}
	else
		cout << "플레이어 접속 대기중.. \n";
	return true;
}

bool Handle_S_CHAT(PacketSessionRef& session, Protocol::S_CHAT& pkt)
{
	uint64 playerId = pkt.playerid();

	std::cout << "ID: " << playerId << ") " << pkt.msg() << endl;
	
	//wstring message_w;
	//message_w.assign(pkt.msg().begin(), pkt.msg().end());

	wstring sendID = L"";
	if (mysession)
		sendID = to_wstring((int)pkt.playerid()) + L" : ";

	wstring wstrText = s2ws(pkt.msg());
	GEngine->GetFontDevice()->Server_UpdateFont(sendID + wstrText);
	GEngine->GetFontDevice()->PushFont(wstrText);

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

	// 본인의 플레이어가 아닐때만 이동, 애니메이션 동기화 시켜줌
	if (_player->GetPlayerID() != mysession->GetPlayerID())
	{
		_player->GetTransform()->SetLocalPosition(pos);
		_player->GetTransform()->SetLocalRotation(rot);
		static_pointer_cast<TagMark>(scene->GetTagMarks(_player->GetPlayerID())->GetScript(0))->SetPosition(pos);
	}

	return true;
}

bool Handle_S_USE_DEBUFITEM(PacketSessionRef& session, Protocol::S_USE_DEBUFITEM& pkt)
{
	shared_ptr<GameObject>	_player = make_shared<GameObject>();
	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
	_player = scene->GetPlayer(mysession->GetPlayerID());

	if (static_pointer_cast<Player>(_player->GetScript(0))->GetCurItem(Player::ITEM::SHIELD))
	{
		//static_pointer_cast<Player>(_player->GetScript(0))->SetCurItem(Player::ITEM::SHIELD, false);
		GET_SINGLE(ItemSlotManager)->UseShieldItem();
		//static_pointer_cast<Player>(_player->GetScript(0))->SetSheildTime(0.f);
		return true;
	}
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

		if (_player == nullptr)
			return true;
		
		if (_player->GetPlayerID() == pkt.fromplayerid())
			continue;

		if (static_pointer_cast<Player>(_player->GetScript(0))->GetCurItem(Player::ITEM::SHIELD))
		{
			//static_pointer_cast<Player>(_player->GetScript(0))->SetCurItem(Player::ITEM::SHIELD, false);
			GET_SINGLE(ItemSlotManager)->UseShieldItem();
			//static_pointer_cast<Player>(_player->GetScript(0))->SetSheildTime(0.f);

			_player->_state->End(*_player);
			state = new IdleState;
			delete _player->_state;
			_player->_state = state;
			_player->_state->Enter(*_player);
			_player->_state->curState = STATE::IDLE;

			return true;
		}
		else
		{
			static_pointer_cast<Player>(_player->GetScript(0))->SetCurItem(Player::ITEM::STUN, true);
			if (static_pointer_cast<Player>(_player->GetScript(0))->_state->curState != STATE::STUN)
			{			
				state = new StunState;
				delete static_pointer_cast<Player>(_player->GetScript(0))->_state;
				static_pointer_cast<Player>(_player->GetScript(0))->_state = state;
				static_pointer_cast<Player>(_player->GetScript(0))->_state->Enter(*_player);
				_player->_state->curState = STATE::STUN;
				static_pointer_cast<Player>(_player->GetScript(0))->_state->curState = STATE::STUN;
				static_pointer_cast<Player>(_player->GetScript(0))->SetPlayerStunned(true);
				//static_pointer_cast<Player>(_player->GetScript(0))->SetCurState(STATE::STUN);
			}
		}
	}

	return true;
}

bool Handle_S_COLLIDPLAYERTOPLAYER(PacketSessionRef& session, Protocol::S_COLLIDPLAYERTOPLAYER& pkt)
{
	shared_ptr<GameObject>	_player = make_shared<GameObject>();
	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
	PlayerState* state;
	// 원래 술래였다면 술래아니게
	_player = scene->GetPlayer(pkt.fromplayerid());
	_player->SetIsTagger(!_player->GetIsTagger());	// false

	// 술래아닌데 부딪혔다면 술래로
	_player = scene->GetPlayer(pkt.toplayerid());
	_player->SetIsTagger(!_player->GetIsTagger());	// true
	// 새롭게 술래가 됐다면 스턴
	if (static_pointer_cast<Player>(_player->GetScript(0))->_state->curState != STATE::STUN)
	{
		state = new StunState;
		delete static_pointer_cast<Player>(_player->GetScript(0))->_state;
		static_pointer_cast<Player>(_player->GetScript(0))->_state = state;
		static_pointer_cast<Player>(_player->GetScript(0))->_state->Enter(*_player);
		static_pointer_cast<Player>(_player->GetScript(0))->_state->curState = STATE::STUN;
		static_pointer_cast<Player>(_player->GetScript(0))->SetPlayerStunned(true);
		static_pointer_cast<Player>(_player->GetScript(0))->SetCurState(STATE::STUN);
	}
	return true;
}

bool Handle_S_PLAYERINFO(PacketSessionRef& session, Protocol::S_PLAYERINFO& pkt)
{
	shared_ptr<GameObject>	_player1 = make_shared<GameObject>();
	shared_ptr<GameObject>	_player2 = make_shared<GameObject>();
	shared_ptr<GameObject>	_player3 = make_shared<GameObject>();

	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();

	_player1 = scene->GetPlayer(0);
	_player2 = scene->GetPlayer(1);
	_player3 = scene->GetPlayer(2);

	switch (pkt.playerid())
	{
	case 0:
		static_pointer_cast<Player>(_player1->GetScript(0))->SetPlayerScore(static_cast<int>(pkt.score()));
		if (g_EnterPlayerCnt == 1)
			scene->SetCurTime(pkt.timer());
		break;
	case 1:
		static_pointer_cast<Player>(_player2->GetScript(0))->SetPlayerScore(static_cast<int>(pkt.score()));
		if (g_EnterPlayerCnt == 2)
			scene->SetCurTime(pkt.timer());
		break;
	case 2:
		static_pointer_cast<Player>(_player3->GetScript(0))->SetPlayerScore(static_cast<int>(pkt.score()));
		if (g_EnterPlayerCnt == 3)
			scene->SetCurTime(pkt.timer());
		break;
	}
	

	
	
	return true;
}

bool Handle_S_STATE(PacketSessionRef& session, Protocol::S_STATE& pkt)
{
	shared_ptr<GameObject>	_player = make_shared<GameObject>();

	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();

	_player = scene->GetPlayer(pkt.playerid());
	
	if (_player == nullptr)
		return true;

	PlayerState* state;

	switch (pkt.state())
	{
	case Protocol::IDLE:
		if (static_pointer_cast<Player>(_player->GetScript(0))->_state->curState == STATE::STUN)
			break;
		if (_player->_state->curState != STATE::IDLE)
		{
			if (_player->_state->curState != STATE::STUN)
			{
				//static_pointer_cast<Player>(_player->GetScript(0))->_state->End(*_player);
				_player->_state->End(*_player);
				state = new IdleState;
				delete _player->_state;
				_player->_state = state;
				_player->_state->Enter(*_player);
				_player->_state->curState = STATE::IDLE;

				static_pointer_cast<Player>(_player->GetScript(0))->_state->curState = STATE::IDLE;
			}
		}
		break;
	case Protocol::WALK:
		if (_player->_state->curState != STATE::WALK)
		{
			if (_player->_state->curState != STATE::DASH)
			{
				if (_player->_state->curState != STATE::SLOW)
				{
					//static_pointer_cast<Player>(_player->GetScript(0))->_state->End(*_player);
					_player->_state->End(*_player);
					state = new MoveState;
					delete _player->_state;
					_player->_state = state;
					_player->_state->Enter(*_player);
					_player->_state->curState = STATE::WALK;

					static_pointer_cast<Player>(_player->GetScript(0))->_state->curState = STATE::WALK;
				}
			}
		}
		break;
	case Protocol::JUMP:
		if (static_pointer_cast<Player>(_player->GetScript(0))->_state->curState != STATE::JUMP)
		{
			state = new JumpState;
			delete static_pointer_cast<Player>(_player->GetScript(0))->_state;
			static_pointer_cast<Player>(_player->GetScript(0))->_state = state;
			static_pointer_cast<Player>(_player->GetScript(0))->_state->Enter(*_player);
			static_pointer_cast<Player>(_player->GetScript(0))->_state->curState = STATE::JUMP;
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
		if (static_pointer_cast<Player>(_player->GetScript(0))->GetCurItem(Player::ITEM::SHIELD))
			break;
		if (static_pointer_cast<Player>(_player->GetScript(0))->_state->curState != STATE::STUN)
		{
			state = new StunState;
			delete static_pointer_cast<Player>(_player->GetScript(0))->_state;
			static_pointer_cast<Player>(_player->GetScript(0))->_state = state;
			static_pointer_cast<Player>(_player->GetScript(0))->_state->Enter(*_player);
			static_pointer_cast<Player>(_player->GetScript(0))->_state->curState = STATE::STUN;
			static_pointer_cast<Player>(_player->GetScript(0))->SetCurState(STATE::STUN);
		}
		break;
	case Protocol::DASH:
		/*if (static_pointer_cast<Player>(_player->GetScript(0))->_state->curState != STATE::DASH)
		{
			static_pointer_cast<Player>(_player->GetScript(0))->_state->End(*_player);
			delete static_pointer_cast<Player>(_player->GetScript(0))->_state;
			state = new DashState;
			static_pointer_cast<Player>(_player->GetScript(0))->_state = state;
			static_pointer_cast<Player>(_player->GetScript(0))->_state->Enter(*_player);
			static_pointer_cast<Player>(_player->GetScript(0))->_state->curState = STATE::DASH;
		}*/
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
		// 프로토콜 오류로 일단 안쓰는 enum으로 테스트
		if (_player->_state->curState != STATE::DASH_REST)
		{
			static_pointer_cast<Player>(_player->GetScript(0))->_state->End(*_player);
			delete static_pointer_cast<Player>(_player->GetScript(0))->_state;

			state = new IdleState;
			static_pointer_cast<Player>(_player->GetScript(0))->_state = state;
			static_pointer_cast<Player>(_player->GetScript(0))->_state->Enter(*_player);
			static_pointer_cast<Player>(_player->GetScript(0))->_state->curState = STATE::IDLE;

			/*_player->_state->End(*_player);
			delete _player->_state;
			state = new IdleState;
			_player->_state = state;
			_player->_state->Enter(*_player);
			_player->_state->curState = STATE::IDLE;*/
		}
		/*if (_player->_state->curState != STATE::DASH_REST)
		{
			_player->_state->End(*_player);
			delete _player->_state;
			state = new DashRestState;
			_player->_state = state;
			_player->_state->Enter(*_player);
			_player->_state->curState = STATE::DASH_REST;
		}*/
		break;
	case Protocol::SLOW:
		if (_player->_state->curState != STATE::SLOW)
		{
			_player->_state->End(*_player);
			delete _player->_state;
			state = new SlowState;
			_player->_state = state;
			_player->_state->Enter(*_player);
			_player->_state->curState = STATE::SLOW;
		}
		break;
	default:
		break;
	}
	return true;
}

bool Handle_S_PLUSTIME(PacketSessionRef& session, Protocol::S_PLUSTIME& pkt)
{
	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
	scene->AddCurTime(-30.f);
	return true;
}

bool Handle_S_STUNEND(PacketSessionRef& session, Protocol::S_STUNEND& pkt)
{
	shared_ptr<GameObject>	_player = make_shared<GameObject>();
	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();

	_player = scene->GetPlayer(pkt.playerid());

	if (_player == nullptr)
		return true;

	PlayerState* state;
	static_pointer_cast<Player>(_player->GetScript(0))->_state->End(*_player);
	delete static_pointer_cast<Player>(_player->GetScript(0))->_state;

	state = new IdleState;
	static_pointer_cast<Player>(_player->GetScript(0))->_state = state;
	static_pointer_cast<Player>(_player->GetScript(0))->_state->Enter(*_player);
	static_pointer_cast<Player>(_player->GetScript(0))->_state->curState = STATE::IDLE;

	return true;
}

bool Handle_S_USE_SHIELD(PacketSessionRef& session, Protocol::S_USE_SHIELD& pkt)
{
	shared_ptr<GameObject>	_player = make_shared<GameObject>();
	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();

	_player = scene->GetPlayer(pkt.playerid());

	if (_player == nullptr)
		return true;
	if (mysession->GetPlayerID() == pkt.playerid())
		return true;
	
	static_pointer_cast<Player>(_player->GetScript(0))->SetCurItem(Player::ITEM::SHIELD, true);
	static_pointer_cast<Player>(_player->GetScript(0))->SetShieldEffectPlayerIndex((int)pkt.playerid());
	//static_pointer_cast<Player>(_player->GetScript(0))->Server_Item_Shield();
	return true;
}

bool Handle_S_RESTART(PacketSessionRef& session, Protocol::S_RESTART& pkt)
{
	GET_SINGLE(SceneManager)->LoadScene(SCENE_ID::LOBBY);
	return true;
}

bool Handle_S_USE_SILENCE(PacketSessionRef& session, Protocol::S_USE_SILENCE& pkt)
{
	shared_ptr<GameObject>	_player = make_shared<GameObject>();
	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
	_player = scene->GetPlayer(mysession->GetPlayerID());

	if (static_pointer_cast<Player>(_player->GetScript(0))->GetCurItem(Player::ITEM::SILENCE))
		return true;
	static_pointer_cast<Player>(_player->GetScript(0))->SetCurItem(Player::ITEM::SILENCE, true);


	return true;
}


