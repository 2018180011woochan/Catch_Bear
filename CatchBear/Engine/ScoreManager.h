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
	const vector<shared_ptr<GameObject>>& GetVecRankedPlayers();

private:
	void AddScore();

private:
	float	_scoreTime = 0.f;
	bool	_bRanked = false;
	bool	_bTaggerReset = false;

	shared_ptr<GameObject>	_myPlayer;

	// ���� ������ �ſ��� ��Ƶ� �÷��̾ �����ͼ� ���� �ű�
	vector<shared_ptr<GameObject>>	_vecRankedPlayers;

	array<int, 3> _playerScore;	// 0�� �ε����� ������ �� ����
};

