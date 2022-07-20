#pragma once

class AnimationSet
{
public:
	AnimationSet(float length, int framePerSec, int nKeyFrames, int nSkinnings, wstring name);
	~AnimationSet();

public:
	void SetPosition(float fTrackPosition);

	XMFLOAT4X4 GetSRT(int nBone);

	bool IsAnimationFinish();

public:
	wstring					_name;

	float					_length;
	int						_framePerSec;
	int						_nKeyFrames;
	
	vector<float>			_keyFrameTimes;
	vector<vector<Matrix>>	_keyFrameTrans;

	float					_position;
	int						_ntype;

	int						_curFrame;
};

