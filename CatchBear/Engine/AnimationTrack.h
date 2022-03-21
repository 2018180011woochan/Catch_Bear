#pragma once

class AnimationTrack
{
public:
	AnimationTrack() { }
	~AnimationTrack() { }

public:
	BOOL 							_bEnable = true;
	float 							_speed = 1.0f;
	float 							_position = 0.0f;
	float 							_weight = 1.0f;

	int 							_nAnimationSet = 0;

public:
	void SetAnimationSet(int nAnimationSet) { _nAnimationSet = nAnimationSet; }

	void SetEnable(bool bEnable) { _bEnable = bEnable; }
	void SetSpeed(float fSpeed) { _speed = fSpeed; }
	void SetWeight(float fWeight) { _weight = fWeight; }
	void SetPosition(float fPosition) { _position = fPosition; }
};

