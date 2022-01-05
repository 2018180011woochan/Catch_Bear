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

	// ���� ��忡 ���� �������� ��� ���
	void Display_ChildHierarchy(int* pnFrame, FbxNode* pfbxNode, int nTabIndents);
	
	// �޽��� ��Ų �������̼� ���
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

