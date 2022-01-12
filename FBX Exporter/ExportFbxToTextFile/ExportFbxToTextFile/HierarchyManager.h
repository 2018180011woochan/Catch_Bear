#pragma once
#include "SdkManager.h"
#include "Display.h"
#include <fbxsdk.h>

class CHierarchyManager
{
public:
	CHierarchyManager() { m_Display = CDisplay(); }
	~CHierarchyManager() { }
	
public:
	void Display_AllHierarchy(FbxScene* pfbxScene);

	// 현재 노드에 대한 정보들을 모두 출력
	void Display_ChildHierarchy(int* pnFrame, FbxNode* pfbxNode, int nTabIndents);
	
	// 메쉬의 스킨 디포메이션 출력
	void Display_SkinDeformations(FbxMesh* pfbxMesh, int nTabIndents);

	// 메쉬 정보: 정점, uv, normal, tangent, binormal, indics
	void DisplayMesh(FbxMesh* pfbxMesh, int nTabIndents);
	void DisplayControlPoints(FbxMesh* pfbxMesh, int nControlPoints, int nTabIndents);
	void DisplayControlPointUVs(FbxMesh* pfbxMesh, int nControlPoints, int nTabIndents);
	void DisplayControlPointNormals(FbxMesh* pfbxMesh, int nControlPoints, int nTabIndents);
	void DisplayControlPointTangents(FbxMesh* pfbxMesh, int nControlPoints, int nTabIndents);
	void DisplayControlPointBiTangents(FbxMesh* pfbxMesh, int nControlPoints, int nTabIndents);
	void DisplayPolygons(FbxMesh* pfbxMesh, int nPolygons, int nTabIndents);
	void DisplayPolygonVertexIndices(FbxMesh* pfbxMesh, int nPolygons, int nTabIndents);

	// 메쉬 재질
	void DisplayMaterials(FbxMesh* pfbxMesh, int nTabIndents);
	void FindAndDisplayTextureInfoByProperty(FbxSurfaceMaterial* pfbxMaterial, FbxProperty* pfbxProperty, int nTabIndents);
	void DisplayTextureInfo(FbxTexture* pfbxTexture, int nBlendMode);

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

