#include "pch.h"
#include "StageScene.h"
#include "Engine.h"
#include "SoundManager.h"
#include "GameObject.h"
#include "Timer.h"
#include "Resources.h"
#include "Input.h"
#include "CollidManager.h"
#include "ItemManager.h"
#include "ScoreManager.h"
#include "ServerSession.h"
#include "ServerPacketHandler.h"
#include "TagMark.h"
#include "TimerUI.h"
#include "MeshRenderer.h"
#include "Player.h"
#include "ScoreUI.h"
#include "Transform.h"
#include "ItemSlotUI.h"
#include "ItemWindow.h"

StageScene::StageScene()
{
}

StageScene::~StageScene()
{
}

void StageScene::Awake()
{
	//GET_SINGLE(SoundManager)->StopAll();
	//GET_SINGLE(SoundManager)->PlayBGM(L"Stage.mp3");
}

void StageScene::Start()
{
	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		gameObject->Start();
	}
}

void StageScene::Update()
{
	CheckTagger();

	if (_isStart)
	{
		_toStartTime += DELTA_TIME;
		if (_toStartTime >= 7.f)
		{
			CheckMouse();
			GET_SINGLE(CollidManager)->Update();

			if (!_isFinish)
			{
				SetTimer();
				GET_SINGLE(ItemManager)->Update();
				GET_SINGLE(ScoreManager)->Update();
				GET_SINGLE(CollidManager)->Update();
			}

			for (const shared_ptr<GameObject>& gameObject : _gameObjects)
			{
				gameObject->Update();
			}

		}
	}
}

void StageScene::LateUpdate()
{
	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		gameObject->LateUpdate();
	}
}

void StageScene::FinalUpdate()
{
	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		gameObject->FinalUpdate();
	}
}

void StageScene::SetTimer()
{
	Protocol::C_PLAYERINFO pkt;

	if (_FinalPlayerEnter)
	{
		_curTime = 0;
		_FinalPlayerEnter = false;
	}

	shared_ptr<GameObject> mTimer = GetGameObject(L"minuteTimer");
	shared_ptr<GameObject> tTimer = GetGameObject(L"tenSecond");
	shared_ptr<GameObject> oTimer = GetGameObject(L"oneSecond");
	shared_ptr<Texture> textureMinute, textureTenSec, textureOneSec;

	_curTime += DELTA_TIME;

	float time = 180.0f - _curTime;
	if (_curTime >= 180.0f)
		int k = 0;

	// minuteTimer
	int minute = (int)(_curTime / 60.f);
	if (minute == 0)
		textureMinute = GET_SINGLE(Resources)->Load<Texture>(L"timer2", L"..\\Resources\\Texture\\timer\\timer2.png");
	else if (minute == 1)
		textureMinute = GET_SINGLE(Resources)->Load<Texture>(L"timer1", L"..\\Resources\\Texture\\timer\\timer1.png");
	else if (minute == 2)
		textureMinute = GET_SINGLE(Resources)->Load<Texture>(L"timer0", L"..\\Resources\\Texture\\timer\\timer0.png");
	else
		textureMinute = GET_SINGLE(Resources)->Load<Texture>(L"timer0", L"..\\Resources\\Texture\\timer\\timer0.png");
	mTimer->GetMeshRenderer()->GetMaterial()->SetTexture(0, textureMinute);

	// secondTimer
	int second = (int)(time) % 60;
	int ten = second / 10;
	int one = second % 10;
	if (minute == 0 && second == 0)
	{
		ten = 5; one = 9;
	}
	if (minute == 2 && second < 1)
	{
		// 3분 다 지났으면 랭킹 정하기
		SetFinalRanking();
		_isFinish = true;
	}
	wstring texTenName = L"timer" + s2ws(to_string(ten));
	wstring texOneName = L"timer" + s2ws(to_string(one));

	textureTenSec = GET_SINGLE(Resources)->Load<Texture>(texTenName, L"..\\Resources\\Texture\\timer\\" + texTenName + L".png");
	textureOneSec = GET_SINGLE(Resources)->Load<Texture>(texOneName, L"..\\Resources\\Texture\\timer\\" + texOneName + L".png");
	tTimer->GetMeshRenderer()->GetMaterial()->SetTexture(0, textureTenSec);
	oTimer->GetMeshRenderer()->GetMaterial()->SetTexture(0, textureOneSec);

	uint64 myscore = static_pointer_cast<Player>(_players[mysession->GetPlayerID()]->GetScript(0))->GetPlayerScore();
	pkt.set_score(myscore);
	if (mysession->GetPlayerID() == (g_EnterPlayerCnt - 1))
		pkt.set_timer(_curTime);
	pkt.set_playerid(mysession->GetPlayerID());
	if (gPacketControl % 60 == 1)
	{
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
		mysession->Send(sendBuffer);
	}
}

void StageScene::SetFinalRanking()
{
	shared_ptr<GameObject>	ranking = GetGameObject(L"finalRanking");
	ranking->_isRender = true;

	GET_SINGLE(ScoreManager)->Rank();
	vector<shared_ptr<GameObject>> players = GET_SINGLE(ScoreManager)->GetVecRankedPlayers();
	int scores[3] = {};
	wstring playerName[3] = {};

	for (size_t i = 0; i < players.size(); ++i)
	{
		scores[i] = static_pointer_cast<Player>(players[i]->GetScript(0))->GetPlayerScore();
		playerName[i] = players[i]->GetName();

		wstring scoreUIName1 = playerName[i] + L"Score1";
		wstring scoreUIName2 = playerName[i] + L"Score2";
		wstring scoreUIName3 = playerName[i] + L"Score3";
		wstring scoreIconName = playerName[i] + L"ScoreIcon";

		shared_ptr<GameObject> mScore1 = GetGameObject(scoreUIName1);
		shared_ptr<GameObject> mScore2 = GetGameObject(scoreUIName2);
		shared_ptr<GameObject> mScore3 = GetGameObject(scoreUIName3);
		shared_ptr<GameObject> mIcon = GetGameObject(scoreIconName);

		// 위치, 크기 재설정해야 함

		mIcon->GetTransform()->SetLocalPosition(Vec3(-180.f, 110.f - (i * 180.f), 500.f));
		mIcon->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 50.f));

		mScore1->GetTransform()->SetLocalPosition(Vec3(-100.f, 110.f - (i * 180.f), 500.f));
		mScore1->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 50.f));

		mScore2->GetTransform()->SetLocalPosition(Vec3(0.f, 110.f - (i * 180.f), 500.f));
		mScore2->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 50.f));

		mScore3->GetTransform()->SetLocalPosition(Vec3(100.f, 110.f - (i * 180.f), 500.f));
		mScore3->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 50.f));
	}
}

void StageScene::CheckMouse()
{
	GetCursorPos(&_mousePos);
	ScreenToClient(GET_WINDOW.hwnd, &_mousePos);

	// 1. ItemWnd
	shared_ptr<GameObject>	itemWnd = GetGameObject(L"ItemWindow");

	for (int i = 0; i < 3; ++i)
	{
		shared_ptr<GameObject>	slot = GetGameObject(L"ItemSlot" + s2ws(to_string(i + 1)));
		wstring itemName = static_pointer_cast<ItemSlotUI>(slot->GetScript(0))->GetSettingItemName();

		if (static_pointer_cast<ItemSlotUI>(slot->GetScript(0))->GetIsSettingItem())
		{
			if (PtInRect(&_slotRt[i], _mousePos))
			{
				itemWnd->_isRender = true;

				wstring name = static_pointer_cast<ItemSlotUI>(slot->GetScript(0))->_texName;
				static_pointer_cast<ItemWindow>(itemWnd->GetScript(0))->_itemName = name;
				static_pointer_cast<ItemWindow>(itemWnd->GetScript(0))->SetItemName();

				Vec3 localPos = slot->GetTransform()->GetLocalPosition();
				localPos.y += 110.f;
				itemWnd->GetTransform()->SetLocalPosition(localPos);
				return;
			}
			else itemWnd->_isRender = false;

		}
	}

	// End & Restart
	shared_ptr<GameObject>	finalWnd = GetGameObject(L"finalRanking");

	if (finalWnd->_isRender)
	{
		if (PtInRect(&_endRt, _mousePos))
		{
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"end", L"..\\Resources\\Texture\\end.png");
			finalWnd->GetMeshRenderer()->GetMaterial()->SetTexture(0, texture);
			if (INPUT->GetButton(KEY_TYPE::LBUTTON))
			{
				_isEnd = true;
			}
		}
		else if (PtInRect(&_restartRt, _mousePos))
		{
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"restart", L"..\\Resources\\Texture\\restart.png");
			finalWnd->GetMeshRenderer()->GetMaterial()->SetTexture(0, texture);
		}
		else
		{
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"ranking", L"..\\Resources\\Texture\\ranking.png");
			finalWnd->GetMeshRenderer()->GetMaterial()->SetTexture(0, texture);
		}
	}
}

void StageScene::CheckTagger()
{
	shared_ptr<GameObject> tagMark1 = _TagMarks[0];
	shared_ptr<GameObject> tagMark2 = _TagMarks[1];
	shared_ptr<GameObject> tagMark3 = _TagMarks[2];
	for (const shared_ptr<GameObject>& gameObject : _vecPlayers)
	{
		switch (gameObject->GetPlayerID())
		{
		case 0:		// 1번 플레이어
			if (gameObject->GetIsTagger()) {
				_isStart = true;
				_tagNum = 0;
			}
			break;
		case 1:		// 2번 플레이어
			if (gameObject->GetIsTagger()) {
				_isStart = true;
				_tagNum = 1;
			}
			break;
		case 2:		// 3번 플레이어
			if (gameObject->GetIsTagger()) {
				_isStart = true;
				_tagNum = 2;
			}
			break;
		}
	}
}
