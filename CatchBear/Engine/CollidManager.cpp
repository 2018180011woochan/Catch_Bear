#include "pch.h"
#include "CollidManager.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "Scene.h"
#include "Transform.h"
#include "ServerSession.h"
#include "ServerPacketHandler.h"
#include "PlayerState.h"

void CollidManager::Update()
{
	ColiisionPlayerToStaticObj();
	CollisionTaggerToPlayer();
	CollisionPlayerToPlayer();
}

void CollidManager::ColiisionPlayerToStaticObj()
{
	auto& gameObjects = GET_SINGLE(SceneManager)->GetActiveScene()->GetVecPlayers();
	shared_ptr<GameObject>	_player = make_shared<GameObject>();

	// �� ���� �÷��̾� ã��
	_player = GET_SINGLE(SceneManager)->GetActiveScene()->GetPlayer(mysession->GetPlayerID());

	auto& staticObjects = GET_SINGLE(SceneManager)->GetActiveScene()->GetStaticObj();

	for (auto mapobj = staticObjects.begin(); mapobj != staticObjects.end(); mapobj++)
	{
		if ((*mapobj)->GetName() == L"Tree_01" || (*mapobj)->GetName() == L"Tree_02" ||
			(*mapobj)->GetName() == L"Tree_03" || (*mapobj)->GetName() == L"Rock_02" ||
			/*(*mapobj)->GetName() == L"Rock_03" ||*/ (*mapobj)->GetName() == L"Rock_04" ||
			(*mapobj)->GetName() == L"wooden_fence_04")
		{
			if ((*mapobj)->GetBoundingBox().Intersects(_player->GetBoundingBox()))
			{
				_player->SetIsAllowPlayerMove(false);
				_player->SetIsCollidObj(true);
				break;
			}
			else {
				_player->SetIsAllowPlayerMove(true);
				_player->SetIsCollidObj(false);
			}
		}
		else
			wstring namee = (*mapobj)->GetName();

	}
	
}

void CollidManager::CollisionTaggerToPlayer()
{
	auto& players = GET_SINGLE(SceneManager)->GetActiveScene()->GetVecPlayers();
	shared_ptr<GameObject>	_tagplayer = make_shared<GameObject>();

	// ���� ã��
	for (int i = 0; i < g_EnterPlayerCnt; ++i)
	{
		// GetIsTagger() == true: ����!
		if (GET_SINGLE(SceneManager)->GetActiveScene()->GetPlayer(i)->GetIsTagger() == true)
			_tagplayer = GET_SINGLE(SceneManager)->GetActiveScene()->GetPlayer(i);
	}

	for (auto& pl = players.begin(); pl != players.end(); pl++)
	{
		// pl: ����X �Ϲ� �÷��̾�O
		if ((*pl)->GetIsTagger() == false)
		{
			if (static_pointer_cast<Player>(_tagplayer->GetScript(0))->_state->curState != STATE::STUN)
			{
				// p1 & tag �浹!
				if ((*pl)->GetBoundingBox().Intersects(_tagplayer->GetBoundingBox()))
				{
					Protocol::C_COLLIDPLAYERTOPLAYER collidPkt;
					collidPkt.set_fromplayerid(_tagplayer->GetPlayerID());
					collidPkt.set_toplayerid((*pl)->GetPlayerID());

					cout << "������ " << _tagplayer->GetPlayerID() << " -> " <<
						(*pl)->GetPlayerID() << " �� �ٲ�\n";

					auto sendBuffer = ServerPacketHandler::MakeSendBuffer(collidPkt);
					mysession->Send(sendBuffer);
					break;
				}

			}
		}

	}
}

void CollidManager::CollisionPlayerToPlayer()
{
	auto& players = GET_SINGLE(SceneManager)->GetActiveScene()->GetVecPlayers();
	shared_ptr<GameObject>	_myplayer = make_shared<GameObject>();
	// ������ �÷��̾� ã�Ƽ�
	_myplayer = GET_SINGLE(SceneManager)->GetActiveScene()->GetPlayer(mysession->GetPlayerID());

	if (!_myplayer->GetIsCollidObj())
	{
		// �ٸ� �÷��̾�� �浹�˻�
		for (auto& pl = players.begin(); pl != players.end(); pl++)
		{
			if ((*pl)->GetPlayerID() == _myplayer->GetPlayerID())
				continue;

			if (_myplayer->GetBoundingBox().Intersects((*pl)->GetBoundingBox()))
			{
				_myplayer->SetIsAllowPlayerMove(false);
				break;
			}
			else
				_myplayer->SetIsAllowPlayerMove(true);
		}
	}
}

