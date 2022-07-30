#pragma once
#include "Scene.h"

class StageScene : public Scene
{
public:
	StageScene();
	virtual ~StageScene();

public:
	// Scene을(를) 통해 상속됨
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void FinalUpdate() override;

private:
	void SetTimer();
	void SetFinalRanking();

	void CheckMouse();
	void CheckTagger();

	void InitUI();


public:
	POINT GetMousePos() { return _mousePos; }


private:
	POINT	_mousePos = { 0, 0 };
	RECT	_slotRt[3] = { {100, 620, 200, 710}, {220, 620, 320, 710}, {340, 620, 440, 710} };
	RECT	_endRt = { 470, 80, 680, 130 };
	RECT	_restartRt = { 770, 650, 950, 700 };

	//bool	_isTimerStart = false;
	float	_StartTimer = 0.f;
	bool	_isSpeedTime = false;
};

