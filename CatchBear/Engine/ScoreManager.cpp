#include "pch.h"
#include "ScoreManager.h"
#include "Timer.h"
#include "Player.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "ScoreUIManager.h"
#include "ServerSession.h"

void ScoreManager::Update()
{
	if (GET_SINGLE(SceneManager)->GetActiveScene()->_isStart)
		AddScore();
}

const vector<shared_ptr<GameObject>>& ScoreManager::GetVecRankedPlayers()
{
	if (_bRanked)
		return _vecRankedPlayers;
}

void ScoreManager::AddScore()
{
	// 1초에 1점씩 올라감
	// 보물을 찾으면 30점 얻음
	// 술래는 보물 찾아도 점수 얻지 않음

	auto& scene = GET_SINGLE(SceneManager)->GetActiveScene();

	// 모든 플레이어가 점수가 올라가게 수정함
	_myPlayer = scene->GetPlayer(mysession->GetPlayerID());	

	_scoreTime += DELTA_TIME;

	if (_scoreTime >= 1.f)
	{
		// player1
		shared_ptr<GameObject>	player1 = GET_SINGLE(SceneManager)->GetActiveScene()->GetGameObject(L"Player1");
		GET_SINGLE(ScoreUIManager)->SetPlayerScoreTexture(0, static_pointer_cast<Player>(player1->GetScript(0))->GetPlayerScore());

		// player2
		shared_ptr<GameObject>	player2 = GET_SINGLE(SceneManager)->GetActiveScene()->GetGameObject(L"Player2");
		GET_SINGLE(ScoreUIManager)->SetPlayerScoreTexture(1, static_pointer_cast<Player>(player2->GetScript(0))->GetPlayerScore());

		//player3
		shared_ptr<GameObject>	player3 = GET_SINGLE(SceneManager)->GetActiveScene()->GetGameObject(L"Player3");
		GET_SINGLE(ScoreUIManager)->SetPlayerScoreTexture(2, static_pointer_cast<Player>(player3->GetScript(0))->GetPlayerScore());

		// 내가 테스트할땐 플레이어 한명만 만들어서(걔가 술래됨) 꺼둠, 원래 켜야됨!
		// 술래여도 다른 플레이어들의 점수 올라가는건 보여야 하니까 여기서 리턴 변경
		if (_myPlayer->GetIsTagger()) return;
		static_pointer_cast<Player>(_myPlayer->GetScript(0))->AddPlayerScore(1);

		//printf("Score: %d\n", static_pointer_cast<Player>(_myPlayer->GetScript(0))->GetPlayerScore());
		_scoreTime = 0.f;
	}
}

void ScoreManager::Rank()
{
	auto& scene = GET_SINGLE(SceneManager)->GetActiveScene();
	_vecRankedPlayers = scene->GetVecPlayers();

	sort(_vecRankedPlayers.begin(), _vecRankedPlayers.end(),
		[](shared_ptr<GameObject>& p1, shared_ptr<GameObject>& p2)
		{
			return static_pointer_cast<Player>(p1->GetScript(0))->GetPlayerScore()
				> static_pointer_cast<Player>(p2->GetScript(0))->GetPlayerScore();
		});

	_bRanked = true;
}
