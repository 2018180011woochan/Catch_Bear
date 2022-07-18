#pragma once
#include "Scene.h"

class LobbyScene : public Scene
{
public:
	LobbyScene();
	virtual ~LobbyScene();

public:
	// Scene��(��) ���� ��ӵ�
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void FinalUpdate() override;

private:
	void CheckIsPlayerEnter();
};

