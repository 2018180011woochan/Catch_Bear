#pragma once

#include "Display.h"

class CAnimationManager
{
public:
	CAnimationManager() { m_Display = CDisplay(); }
	~CAnimationManager() { }

public:
	void Display_AllAnimations(FbxScene** ppfbxScenes, int nSeparatedAnimations);

	void Display_AllAnimations(FbxScene* pfbxScene);
	void DisplayAnimation(FbxAnimStack* pfbxAnimStack, FbxNode* pfbxNode, int nTabIndents);

	int Get_AnimationLayerCurveNodesCnt(FbxAnimLayer* pfbxAnimationLayer, FbxNode* pfbxNode);
	int Get_AnimationCurvesCnt(FbxAnimLayer* pfbxAnimationLayer, FbxNode* pfbxNode);

	void Display_CurveAnimation(FbxAnimLayer* pfbxAnimationLayer, FbxNode* pfbxNode, int* pnCurveNode, int nTabIndents);
	void Display_CurveChannels(FbxAnimLayer* pfbxAnimationLayer, FbxNode* pfbxNode, int* pnCurveNode, int nTabIndents);
	void Display_CurveKeys(char* pHeader, FbxAnimCurve* pfbxAnimationCurve, char* pSuffix, int nTabIndents, bool bRotationAngle);

public:
	static CAnimationManager* Get_Instance()
	{
		if (nullptr == m_pInstance)	m_pInstance = new CAnimationManager;
		return m_pInstance;
	}
	static void Destroy_Instance()
	{
		if (m_pInstance) {
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}

private:
	static CAnimationManager* m_pInstance;
	CDisplay	m_Display;
};

