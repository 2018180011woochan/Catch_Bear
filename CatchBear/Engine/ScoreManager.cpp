#include "pch.h"
#include "ScoreManager.h"
#include "Timer.h"

void ScoreManager::Update()
{
	AddScore();
}

void ScoreManager::AddScore()
{
	// 1�ʿ� 1���� �ö�
	// ������ ã���� 30�� ���� (�ӽð�)

	_scoreTime += DELTA_TIME;

	if (_scoreTime >= 1.f)
	{
		_score += 1;
		_scoreTime = 0.f;
	}
}
