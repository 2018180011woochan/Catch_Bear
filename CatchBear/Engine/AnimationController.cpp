#include "pch.h"
#include "AnimationController.h"
#include "Timer.h"
#include "Resources.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "StructuredBuffer.h"
#include "AnimationTrack.h"
#include "AnimationSet.h"
#include "Transform.h"
#include "Engine.h"

AnimationController::AnimationController() : Component(COMPONENT_TYPE::ANIMATOR)
{
}

AnimationController::~AnimationController()
{
}

void AnimationController::SetModelInfo(shared_ptr<AnimationModelInfo> model, SkinningInfo skinInfo)
{
	_nAnimationTracks = 1;
	_animTracks = new AnimationTrack[_nAnimationTracks];

	_animSets = model->_allAnimationSets;

	// _vecAnimatedFrame: 애니메이션에 사용되는 뼈들
	_animatedTrans.resize(model->_vecAnimatedFrame.size());
	
	matToParent.resize(model->_vecAnimatedFrame.size());

	for (size_t i = 0; i < model->_vecAnimatedFrame.size(); ++i)
	{
		shared_ptr<Transform>	trans = make_shared<Transform>();
		trans = model->_vecAnimatedFrame[i];
		_animatedTrans[i] = trans;

		matToParent[i] = trans->_matWorld;
	}

	// Bone Offset 행렬
	SkinningInfo	skInfo = skinInfo;
	const int32 boneCnt = static_cast<int32>(skInfo.boneOffsets.size());

	offsetMat.resize(boneCnt);

	for (int32 i = 0; i < boneCnt; ++i)	offsetMat[i] = skInfo.boneOffsets[i];
}

void AnimationController::PushData()
{
	BoneOffsetParams	offset = {};

	for (int i = 0; i < 72; ++i)
	{
		offset.matOfset[i] = offsetMat[i];
		XMStoreFloat4x4(&offset.matOfset[i], XMMatrixTranspose(XMLoadFloat4x4(&offset.matOfset[i])));
	}
	CONST_BUFFER(CONSTANT_BUFFER_TYPE::BONE_OFFSET)->PushGraphicsData(&offset, sizeof(offset));


	AnimatedBoneParams	matrix = {};

	for (int i = 0; i < 72; ++i)
		matrix.matBoneTrans[i] = matToParent[i];

	CONST_BUFFER(CONSTANT_BUFFER_TYPE::ANIMATED_BONE_TRANS)->PushGraphicsData(&matrix, sizeof(matrix));
}

void AnimationController::AdvanceTime(float fElapsedTime)
{
	_time += fElapsedTime;

	if (_animTracks)
	{
		for (int i = 0; i < _nAnimationTracks; i++) _animTracks[i]._position += (fElapsedTime * _animTracks[i]._speed);

		for (int j = 0; j < _animatedTrans.size(); j++)
		{
			XMFLOAT4X4 xmf4x4Transform = Zero();

			for (int k = 0; k < _nAnimationTracks; k++)
			{
				if (_animTracks[k]._bEnable)
				{
					AnimationSet* pAnimationSet = _animSets->_animationSet[_animTracks[k]._nAnimationSet];
					pAnimationSet->SetPosition(_animTracks[k]._position);

					XMFLOAT4X4 xmf4x4TrackTransform = pAnimationSet->GetSRT(j);
					_curFrame = pAnimationSet->_curFrame;
					//printf("frame: %d\n", _curFrame);
					xmf4x4Transform = Add(xmf4x4Transform, Scale(xmf4x4TrackTransform, _animTracks[k]._weight));
				}
			}
			_animatedTrans[j]->_matToParent = xmf4x4Transform;
			GetGameObject()->GetTransform()->FindTransform(_animatedTrans[j]->_name)->_matToParent = xmf4x4Transform;
		}

	}
}

void AnimationController::SetWorldMatrix()
{
	for (size_t i = 0; i < _animatedTrans.size(); ++i)
	{
		matToParent[i] = GetGameObject()->GetTransform()->FindTransform(_animatedTrans[i]->_name)->_matWorld;
		XMStoreFloat4x4(&matToParent[i], XMMatrixTranspose(XMLoadFloat4x4(&matToParent[i])));
	}
}

bool AnimationController::IsAnimationFinish(int nTrack)
{
	AnimationSet*	 pAnimationSet = _animSets->_animationSet[_animTracks[nTrack]._nAnimationSet];
	
	return pAnimationSet->IsAnimationFinish();
}

void AnimationController::SetAnimationPosition(int nTrack, float fPosition)
{
	_animSets->_animationSet[_animTracks[nTrack]._nAnimationSet]->SetPosition(fPosition);
}


void AnimationController::FinalUpdate()
{
	//_updateTime += DELTA_TIME;
}

void AnimationController::SetTrackAnimationSet(int nAnimationTrack, int nAnimationSet)
{
	if (_animTracks)
	{
		_animTracks[nAnimationTrack]._position = 0.f;
		_animTracks[nAnimationTrack]._nAnimationSet = nAnimationSet;
	}
}

void AnimationController::SetTrackEnable(int nAnimationTrack, bool bEnable)
{
	if (_animTracks) _animTracks[nAnimationTrack].SetEnable(bEnable);
}

void AnimationController::SetTrackPosition(int nAnimationTrack, float fPosition)
{
	if (_animTracks) _animTracks[nAnimationTrack].SetPosition(fPosition);
}

void AnimationController::SetTrackSpeed(int nAnimationTrack, float fSpeed)
{
	if (_animTracks) _animTracks[nAnimationTrack].SetSpeed(fSpeed);
}

void AnimationController::SetTrackWeight(int nAnimationTrack, float fWeight)
{
	if (_animTracks) _animTracks[nAnimationTrack].SetWeight(fWeight);
}
