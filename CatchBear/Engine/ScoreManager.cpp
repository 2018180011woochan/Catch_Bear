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
	// 1�ʿ� 1���� �ö�
	// ������ ���� �ö��� ����
	// ������ ã���� 30�� ���� (�ӽð�)

	// �� ���� �÷��̾���� ã�´�
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
