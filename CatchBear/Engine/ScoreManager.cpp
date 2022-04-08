#include "pch.h"
#include "ScoreManager.h"
#include "Timer.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Player.h"

void ScoreManager::Update()
{
	AddScore();
}

void ScoreManager::AddScore()
{
	// 1초에 1점씩 올라감
	// 술래는 점수 올라가지 않음
	// 보물을 찾으면 30점 얻음 (임시값)

	// 씬 안의 플레이어들을 찾는다
	auto& gameObjects = GET_SINGLE(SceneManager)->GetActiveScene()->GetGameObjects();

	for (auto& gameObject : gameObjects)
	{

	}

	_scoreTime += DELTA_TIME;

	if (_scoreTime >= 1.f)
	{
		_score += 1;
		_scoreTime = 0.f;
	}
}
