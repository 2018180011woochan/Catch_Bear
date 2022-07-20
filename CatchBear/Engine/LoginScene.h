#pragma once
#include "Scene.h"

class LoginScene : public Scene
{
public:
	LoginScene();
	virtual ~LoginScene();

public:
	// Scene을(를) 통해 상속됨
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void FinalUpdate() override;
};

