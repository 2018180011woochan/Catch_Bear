#pragma once
#include "SdkManager.h"
#include <fbxsdk.h>

class CHierarchyManager
{
public:
	CHierarchyManager() { }
	~CHierarchyManager() { }
	
public:
	void Display_AllHierarchy(FbxScene* pfbxScene);

	// 현재 노드에 대한 정보들을 모두 출력
	void Display_ChildHierarchy(int* pnFrame, FbxNode* pfbxNode, int nTabIndents);
	
	// 메쉬의 스킨 디포메이션 출력
	void Display_SkinDeformations(FbxMesh* pfbxMesh, int nTabIndents);

public:
	FbxAMatrix GetGeometricOffsetTransform(FbxNode* pfbxNode);

public:
	static CHierarchyManager* Get_Instance()
	{
		if (nullptr == m_pInstance)	m_pInstance = new CHierarchyManager;
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
	static CHierarchyManager* m_pInstance;
	CDisplay	m_Display;
};

