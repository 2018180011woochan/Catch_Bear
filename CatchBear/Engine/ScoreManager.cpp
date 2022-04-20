#include "pch.h"
#include "ScoreManager.h"
#include "Timer.h"
#include "Player.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "ScoreUIManager.h"

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
	// 1�ʿ� 1���� �ö�
	// ������ ã���� 30�� ����
	// ������ ���� ã�Ƶ� ���� ���� ����

	auto& scene = GET_SINGLE(SceneManager)->GetActiveScene();
	_myPlayer = scene->GetGameObject(L"Player1");

	//if (_myPlayer->GetIsTagger()) return;		// ���� �׽�Ʈ�Ҷ� �÷��̾� �Ѹ� ����(�°� ������) ����, ���� �Ѿߵ�!

	_scoreTime += DELTA_TIME;

	if (_scoreTime >= 1.f)
	{
		static_pointer_cast<Player>(_myPlayer->GetScript(0))->AddPlayerScore(1);
		GET_SINGLE(ScoreUIManager)->SetPlayerScoreTexture(0, static_pointer_cast<Player>(_myPlayer->GetScript(0))->GetPlayerScore());
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
				< static_pointer_cast<Player>(p2->GetScript(0))->GetPlayerScore();
		});

	_bRanked = true;
}
