#pragma once

// 1�ʿ� 1���� �ö�
// ������ ã���� 30�� ���� (�ӽð�)

class GameObject;

class ScoreManager
{
	DECLARE_SINGLE(ScoreManager);


public:
	void Update();

public:
	void Rank();

public:
	//const int GetScore() { return _score; }
	//void SetScore(int score) { _score += score; }
	void SetMyPlayer(shared_ptr<GameObject> myPlayer) { _myPlayer = myPlayer; }

private:
	void AddScore();

private:
	float	_scoreTime = 0.f;

	shared_ptr<GameObject>	_myPlayer;
	array<int, 3> _playerScore;	// 0�� �ε����� ������ �� ����
};

