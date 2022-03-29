#pragma once

// 1�ʿ� 1���� �ö�
// ������ ã���� 30�� ���� (�ӽð�)

class ScoreManager
{
	DECLARE_SINGLE(ScoreManager);


public:
	void Update();

public:
	int GetScore() { return _score; }
	void SetScore(int score) { _score += score; }

private:
	void AddScore();

private:
	float	_scoreTime = 0.f;
	int		_score = 0;

	array<int, 3> _playerScore;
};

