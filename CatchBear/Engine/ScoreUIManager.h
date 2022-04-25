#pragma once
#include "Item.h"
class GameObject;

class ScoreUIManager
{
	DECLARE_SINGLE(ScoreUIManager);

public:
	// nPlayer��°�� ������ nScore�� �ؽ�ó�� �����Ѵ�
	void SetPlayerScoreTexture(int nPlayer, int nScore);

private:
	shared_ptr<GameObject> _scoreUI1;
	shared_ptr<GameObject> _scoreUI2;
	shared_ptr<GameObject> _scoreUI3;
};