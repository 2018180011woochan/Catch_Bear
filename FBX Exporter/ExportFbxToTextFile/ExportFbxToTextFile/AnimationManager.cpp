#include "AnimationManager.h"

void CAnimationManager::Display_AllAnimations(FbxScene** ppfbxScenes, int nSeparatedAnimations)
{
	int nAnimationStacks = 0;
	int* pnAnimationStacks = new int[nSeparatedAnimations];

	for (int i = 0; i < nSeparatedAnimations; i++)
	{
		pnAnimationStacks[i] = ppfbxScenes[i]->GetSrcObjectCount<FbxAnimStack>();
		nAnimationStacks += pnAnimationStacks[i];
	}

	// 현재 애니메이션 스택의 개수
	m_Display.DisplayInt("<AnimationSets>: ", nAnimationStacks, "\n", 1);

	// 각각의 애니메이션 스택에 따라 애니메이션 정보 출력
	for (int i = 0, k = 0; i < nSeparatedAnimations; i++)
	{
		for (int j = 0; j < pnAnimationStacks[i]; j++, k++)
		{
			FbxAnimStack* pfbxAnimStack = ppfbxScenes[i]->GetSrcObject<FbxAnimStack>(j);
			m_Display.DisplayIntString("<AnimationSet>: ", k, m_Display.ReplaceBlank(pfbxAnimStack->GetName(), '_'), " ", 2);
			FbxTime fbxTimeStart = pfbxAnimStack->LocalStart;
			FbxTime fbxTimeStop = pfbxAnimStack->LocalStop;
			m_Display.DisplayFloat("", (float)fbxTimeStart.GetSecondDouble(), (float)fbxTimeStop.GetSecondDouble(), "\n");

			DisplayAnimation(pfbxAnimStack, ppfbxScenes[i]->GetRootNode(), 3);
			m_Display.DisplayString("</AnimationSet>", "", "\n", 2);
		}
	}

	m_Display.DisplayString("</AnimationSets>", "", "\n", 1);

	delete[] pnAnimationStacks;
}

void CAnimationManager::Display_AllAnimations(FbxScene* pfbxScene)
{
	int nAnimationStacks = pfbxScene->GetSrcObjectCount<FbxAnimStack>();
	m_Display.DisplayInt("<AnimationSets>: ", nAnimationStacks, "\n", 1);

	for (int i = 0; i < nAnimationStacks; i++)
	{
		FbxAnimStack* pfbxAnimStack = pfbxScene->GetSrcObject<FbxAnimStack>(i);
		m_Display.DisplayIntString("<AnimationSet>: ", i, m_Display.ReplaceBlank(pfbxAnimStack->GetName(), '_'), " ", 2);
		FbxTime fbxTimeStart = pfbxAnimStack->LocalStart;
		FbxTime fbxTimeStop = pfbxAnimStack->LocalStop;
		m_Display.DisplayFloat("", (float)fbxTimeStart.GetSecondDouble(), (float)fbxTimeStop.GetSecondDouble(), "\n");

		DisplayAnimation(pfbxAnimStack, pfbxScene->GetRootNode(), 3);
		m_Display.DisplayString("</AnimationSet>", "", "\n", 2);
	}

	m_Display.DisplayString("</AnimationSets>", "", "\n", 1);
}

// pfbxNode노드에 있는 pfbxAnimStack애니메이션 스택 정보들을 출력하려면
void CAnimationManager::DisplayAnimation(FbxAnimStack* pfbxAnimStack, FbxNode* pfbxNode, int nTabIndents)
{
	// 스택에 있는 레이어를 가져와서
	int nAnimationLayers = pfbxAnimStack->GetMemberCount<FbxAnimLayer>();
	m_Display.DisplayInt("<AnimationLayers>: ", nAnimationLayers, "\n", nTabIndents);

	// 애니메이션 레이어에 있는 애니메이션 커브를 출력하면 된다
	for (int i = 0; i < nAnimationLayers; i++)
	{
		FbxAnimLayer* pfbxAnimationLayer = pfbxAnimStack->GetMember<FbxAnimLayer>(i);
		int nLayerCurveNodes = Get_AnimationLayerCurveNodesCnt(pfbxAnimationLayer, pfbxNode);

		// nAnimationLayers애니메이션 레이어의 개수만큼 애니메이션 레이어를 출력한다
		int nCurveNode = 0;
		m_Display.DisplayIntFloat("<AnimationLayer>: ", i, nLayerCurveNodes, float(pfbxAnimationLayer->Weight) / 100.0f, "\n", nTabIndents + 1);
		Display_CurveAnimation(pfbxAnimationLayer, pfbxNode, &nCurveNode, nTabIndents + 2);
		m_Display.DisplayString("</AnimationLayer>", "", "\n", nTabIndents + 1);
	}

	m_Display.DisplayString("</AnimationLayers>", "", "\n", nTabIndents);
}

int CAnimationManager::Get_AnimationLayerCurveNodesCnt(FbxAnimLayer* pfbxAnimationLayer, FbxNode* pfbxNode)
{
	int nAnimationCurveNodes = 0;
	if (Get_AnimationCurvesCnt(pfbxAnimationLayer, pfbxNode) > 0) nAnimationCurveNodes++;

	for (int i = 0; i < pfbxNode->GetChildCount(); i++)
	{
		nAnimationCurveNodes += Get_AnimationLayerCurveNodesCnt(pfbxAnimationLayer, pfbxNode->GetChild(i));
	}

	return(nAnimationCurveNodes);
}

int CAnimationManager::Get_AnimationCurvesCnt(FbxAnimLayer* pfbxAnimationLayer, FbxNode* pfbxNode)
{
	int nAnimationCurves = 0;

	FbxAnimCurve* pfbxAnimationCurve = NULL;
	if (pfbxAnimationCurve = pfbxNode->LclTranslation.GetCurve(pfbxAnimationLayer, FBXSDK_CURVENODE_COMPONENT_X)) nAnimationCurves++;
	if (pfbxAnimationCurve = pfbxNode->LclTranslation.GetCurve(pfbxAnimationLayer, FBXSDK_CURVENODE_COMPONENT_Y)) nAnimationCurves++;
	if (pfbxAnimationCurve = pfbxNode->LclTranslation.GetCurve(pfbxAnimationLayer, FBXSDK_CURVENODE_COMPONENT_Z)) nAnimationCurves++;
	if (pfbxAnimationCurve = pfbxNode->LclRotation.GetCurve(pfbxAnimationLayer, FBXSDK_CURVENODE_COMPONENT_X)) nAnimationCurves++;
	if (pfbxAnimationCurve = pfbxNode->LclRotation.GetCurve(pfbxAnimationLayer, FBXSDK_CURVENODE_COMPONENT_Y)) nAnimationCurves++;
	if (pfbxAnimationCurve = pfbxNode->LclRotation.GetCurve(pfbxAnimationLayer, FBXSDK_CURVENODE_COMPONENT_Z)) nAnimationCurves++;
	if (pfbxAnimationCurve = pfbxNode->LclScaling.GetCurve(pfbxAnimationLayer, FBXSDK_CURVENODE_COMPONENT_X)) nAnimationCurves++;
	if (pfbxAnimationCurve = pfbxNode->LclScaling.GetCurve(pfbxAnimationLayer, FBXSDK_CURVENODE_COMPONENT_Y)) nAnimationCurves++;
	if (pfbxAnimationCurve = pfbxNode->LclScaling.GetCurve(pfbxAnimationLayer, FBXSDK_CURVENODE_COMPONENT_Z)) nAnimationCurves++;

	return(nAnimationCurves);
}

void CAnimationManager::Display_CurveAnimation(FbxAnimLayer* pfbxAnimationLayer, FbxNode* pfbxNode, int* pnCurveNode, int nTabIndents)
{
	Display_CurveChannels(pfbxAnimationLayer, pfbxNode, pnCurveNode, nTabIndents);

	for (int i = 0; i < pfbxNode->GetChildCount(); i++)
	{
		// 애니메이션 정보를 출력하려면 커브에 있는 노드들을 출력하면 된다
		Display_CurveAnimation(pfbxAnimationLayer, pfbxNode->GetChild(i), pnCurveNode, nTabIndents);
	}
}

void CAnimationManager::Display_CurveChannels(FbxAnimLayer* pfbxAnimationLayer, FbxNode* pfbxNode, int* pnCurveNode, int nTabIndents)
{
	if (Get_AnimationCurvesCnt(pfbxAnimationLayer, pfbxNode) > 0)
	{
		// 애니메이션 커브를 출력하려면 
		m_Display.DisplayIntString("<AnimationCurve>: ", (*pnCurveNode)++, m_Display.ReplaceBlank(pfbxNode->GetName(), '_'), "\n", nTabIndents);

		FbxAnimCurve* pfbxAnimationCurve = NULL;

		//FbxAnimCurveNode *pfbxAnimCurveNode = pfbxNode->LclTranslation.GetCurveNode(pfbxAnimationLayer);
		//int nChannels = pfbxAnimCurveNode->GetChannelsCount();
		//int nCurves = pfbxAnimCurveNode->GetCurveCount(0);

		// LclTranslation로컬 변환에 있는 x라는 커브를 가져다가 애니메이션 커브를 출력한다
		pfbxAnimationCurve = pfbxNode->LclTranslation.GetCurve(pfbxAnimationLayer, FBXSDK_CURVENODE_COMPONENT_X); //"X"
		if (pfbxAnimationCurve) Display_CurveKeys("<TX>: ", pfbxAnimationCurve, "\n", nTabIndents + 1, false);

		pfbxAnimationCurve = pfbxNode->LclTranslation.GetCurve(pfbxAnimationLayer, FBXSDK_CURVENODE_COMPONENT_Y);
		if (pfbxAnimationCurve) Display_CurveKeys("<TY>: ", pfbxAnimationCurve, "\n", nTabIndents + 1, false);

		pfbxAnimationCurve = pfbxNode->LclTranslation.GetCurve(pfbxAnimationLayer, FBXSDK_CURVENODE_COMPONENT_Z);
		if (pfbxAnimationCurve) Display_CurveKeys("<TZ>: ", pfbxAnimationCurve, "\n", nTabIndents + 1, false);

		pfbxAnimationCurve = pfbxNode->LclRotation.GetCurve(pfbxAnimationLayer, FBXSDK_CURVENODE_COMPONENT_X);
		if (pfbxAnimationCurve) Display_CurveKeys("<RX>: ", pfbxAnimationCurve, "\n", nTabIndents + 1, true);

		pfbxAnimationCurve = pfbxNode->LclRotation.GetCurve(pfbxAnimationLayer, FBXSDK_CURVENODE_COMPONENT_Y);
		if (pfbxAnimationCurve) Display_CurveKeys("<RY>: ", pfbxAnimationCurve, "\n", nTabIndents + 1, true);

		pfbxAnimationCurve = pfbxNode->LclRotation.GetCurve(pfbxAnimationLayer, FBXSDK_CURVENODE_COMPONENT_Z);
		if (pfbxAnimationCurve) Display_CurveKeys("<RZ>: ", pfbxAnimationCurve, "\n", nTabIndents + 1, true);

		pfbxAnimationCurve = pfbxNode->LclScaling.GetCurve(pfbxAnimationLayer, FBXSDK_CURVENODE_COMPONENT_X);
		if (pfbxAnimationCurve) Display_CurveKeys("<SX>: ", pfbxAnimationCurve, "\n", nTabIndents + 1, false);

		pfbxAnimationCurve = pfbxNode->LclScaling.GetCurve(pfbxAnimationLayer, FBXSDK_CURVENODE_COMPONENT_Y);
		if (pfbxAnimationCurve) Display_CurveKeys("<SY>: ", pfbxAnimationCurve, "\n", nTabIndents + 1, false);

		pfbxAnimationCurve = pfbxNode->LclScaling.GetCurve(pfbxAnimationLayer, FBXSDK_CURVENODE_COMPONENT_Z);
		if (pfbxAnimationCurve) Display_CurveKeys("<SZ>: ", pfbxAnimationCurve, "\n", nTabIndents + 1, false);

		m_Display.DisplayString("</AnimationCurve>", "", "\n", nTabIndents);
	}
}

void CAnimationManager::Display_CurveKeys(char* pHeader, FbxAnimCurve* pfbxAnimationCurve, char* pSuffix, int nTabIndents, bool bRotationAngle)
{
	// 애니메이션 키의 개수
	int nKeys = pfbxAnimationCurve->KeyGetCount();
	m_Display.DisplayInt(pHeader, nKeys, " ", nTabIndents);

	// 애니메이션 키의 개수만큼 시간과 변환의 정보들을 커브의 정보로 출력한다
	for (int i = 0; i < nKeys; i++)
	{
		FbxTime fbxKeyTime = pfbxAnimationCurve->KeyGetTime(i);
		float fkeyTime = (float)fbxKeyTime.GetSecondDouble();
		m_Display.DisplayFloat("", fkeyTime, " ");
	}
	for (int i = 0; i < nKeys; i++)
	{
		float fKeyValue = static_cast<float>(pfbxAnimationCurve->KeyGetValue(i));
		if (bRotationAngle) fKeyValue = (3.1415926f / 180.f) * fKeyValue;
		m_Display.DisplayFloat("", fKeyValue, " ");
	}

	WriteStringToFile("\n");
}
