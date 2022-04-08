#pragma once

// 1초에 1점씩 올라감
// 보물을 찾으면 30점 얻음 (임시값)

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
	int		_playerCnt = 0;

	array<int, 3> _playerScore;
};

