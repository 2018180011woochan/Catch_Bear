#include "pch.h"
#include "AnimationSet.h"
#include "Timer.h"

AnimationSet::AnimationSet(float length, int framePerSec, int nKeyFrames, int nSkinnings, wstring name)
{
	_length = length;
	_framePerSec = framePerSec;
	_nKeyFrames = nKeyFrames;
	_name = name;
	
	_keyFrameTimes.resize(nKeyFrames);
	_keyFrameTrans.resize(nKeyFrames);
	for (int i = 0; i < nKeyFrames; ++i)	_keyFrameTrans[i].resize(nSkinnings);
}

AnimationSet::~AnimationSet()
{
	if (!_keyFrameTrans.empty()) _keyFrameTrans.clear();
	for (int j = 0; j < _nKeyFrames; j++) if (!_keyFrameTrans[j].empty()) _keyFrameTrans.clear();
	if (!_keyFrameTimes.empty()) _keyFrameTimes.clear();
}

void AnimationSet::SetPosition(float fTrackPosition)
{
	_position = fTrackPosition;
	_position = fmod(fTrackPosition, _keyFrameTimes[_nKeyFrames -1]);
}

XMFLOAT4X4 AnimationSet::GetSRT(int nBone)
{
	Matrix	trans = Identity();

	for (int i = 0; i < (_nKeyFrames - 1); i++)
	{
		if ((_keyFrameTimes[i] <= _position) && (_position <= _keyFrameTimes[i + 1]))
		{
			float t = (_position - _keyFrameTimes[i]) / (_keyFrameTimes[i + 1] - _keyFrameTimes[i]);
			trans = Interpolate(_keyFrameTrans[i][nBone], _keyFrameTrans[i + 1][nBone], t);
			_curFrame = i;
			break;
		}
	}

	return trans;
}

bool AnimationSet::IsAnimationFinish()
{
	if (_position >= _keyFrameTimes[_nKeyFrames - 1])
	{
		_position = 0.f;
		return true;
	}
	return false;
}
