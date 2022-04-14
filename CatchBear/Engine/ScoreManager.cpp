#include "pch.h"
#include "ScoreManager.h"
#include "Timer.h"
#include "Player.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"

void ScoreManager::Update()
{
	AddScore();
}

void ScoreManager::AddScore()
{
	// 1�ʿ� 1���� �ö�
	// ������ ã���� 30�� ����
	// ������ ���� ã�Ƶ� ���� ���� ����

	auto& scene = GET_SINGLE(SceneManager)->GetActiveScene();
	_myPlayer = scene->GetGameObject(L"Player");

	//if (_myPlayer->GetIsTagger()) return;		// ���� �׽�Ʈ�Ҷ� �÷��̾� �Ѹ� ����(�°� ������) ����, ���� �Ѿߵ�!

	_scoreTime += DELTA_TIME;

	if (_scoreTime >= 1.f)
	{
		static_pointer_cast<Player>(_myPlayer->GetScript(0))->AddPlayerScore(1);
		printf("%d\n", static_pointer_cast<Player>(_myPlayer->GetScript(0))->GetPlayerScore());
		_scoreTime = 0.f;
	}
}

void ScoreManager::Rank()
{
}
