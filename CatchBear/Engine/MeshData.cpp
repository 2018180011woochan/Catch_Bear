#include "pch.h"
#include "MeshData.h"
#include "Mesh.h"
#include "Material.h"
#include "Resources.h"
#include "Transform.h"
#include "MeshRenderer.h"

UINT ReadUnsignedIntegerFromFile(FILE* pInFile)
{
	UINT nValue = 0;
	UINT nReads = (UINT)::fread(&nValue, sizeof(UINT), 1, pInFile);
	return(nValue);
}

int ReadIntegerFromFile(FILE* pInFile)
{
	int nValue = 0;
	UINT nReads = (UINT)::fread(&nValue, sizeof(int), 1, pInFile);
	return(nValue);
}

float ReadFloatFromFile(FILE* pInFile)
{
	float fValue = 0;
	UINT nReads = (UINT)::fread(&fValue, sizeof(float), 1, pInFile);
	return(fValue);
}

int ReadStringFromFile(FILE* pInFile, char* pstrToken)
{
	UINT nStrLength = 0;
	UINT nReads = 0;
	nReads = (UINT)::fread(&nStrLength, sizeof(int), 1, pInFile);
	nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
	pstrToken[nStrLength] = '\0';

	return(nStrLength);
}

MeshData::MeshData() : Object(OBJECT_TYPE::MESH_DATA)
{
}

MeshData::~MeshData()
{
}

void MeshData::LoadMeshFromFile(const wstring& path)
{
	FILE* pFile = NULL;

	char* pStr;
	int iLen = WideCharToMultiByte(CP_ACP, 0, path.c_str(), -1, NULL, 0, NULL, NULL);
	pStr = new char[iLen];
	WideCharToMultiByte(CP_ACP, 0, path.c_str(), -1, pStr, iLen, 0, 0);

	fopen_s(&pFile, pStr, "rb");
	rewind(pFile);

	char pStrTocken[64] = { '\0' };
	GameObject* pGameObj = new GameObject();

	for (; ;)
	{
		if (ReadStringFromFile(pFile, pStrTocken))
		{
			if (!strcmp(pStrTocken, "<Hierarchy>"))
			{
				for (; ;)
				{
					ReadStringFromFile(pFile, pStrTocken);
					if (!strcmp(pStrTocken, "<Frame>:"))
					{
						pGameObj = LoadFrameHierarchyFromFile(NULL, pFile);
					}
					else if (!strcmp(pStrTocken, "</Hierarchy>"))
					{

					}
				}
			}
		}
	}
}

GameObject* MeshData::LoadFrameHierarchyFromFile(GameObject* parent, FILE* pFile)
{
	char pStrTocken[64] = { '\0' };
	UINT	nReads = 0;

	int		nFrame = ReadIntegerFromFile(pFile);
	GameObject* pGameObj = new GameObject();
	// ������ �̸����� �ٲ�� ��!
	ReadStringFromFile(pFile, pStrTocken);

	for (; ;)
	{
		ReadStringFromFile(pFile, pStrTocken);

		if (!strcmp(pStrTocken, "<Transform>:"))
		{
			Vec3	xmf3Scale, xmf3Rotate, xmf3Trans;
			Matrix	xmf4x4ToParent;

			nReads = (UINT)::fread(&xmf4x4ToParent, sizeof(XMFLOAT4X4), 1, pFile);

			nReads = (UINT)::fread(&xmf3Scale, sizeof(XMFLOAT3), 1, pFile);
			nReads = (UINT)::fread(&xmf3Rotate, sizeof(XMFLOAT3), 1, pFile);
			nReads = (UINT)::fread(&xmf3Trans, sizeof(XMFLOAT3), 1, pFile);
			
			//pGameObj->GetTransform()->SetLocalToParent(xmf4x4ToParent);
			//pGameObj->GetTransform()->SetLocalScale(xmf3Scale);
			//pGameObj->GetTransform()->SetLocalRotation(xmf3Rotate);
			//pGameObj->GetTransform()->SetLocalPosition(xmf3Trans);
		}

		else if (!strcmp(pStrTocken, "<Mesh>:"))
		{
			LoadMeshInfoFromFile(pFile);
		}
		
		else if (!strcmp(pStrTocken, "<Materials>:"))
		{
			LoadMaterialInfoFromFile(pFile);
		}
	}
}

void MeshData::LoadMeshInfoFromFile(FILE* pFile)
{
	char pStrTocken[64] = { '\0' };
	int	nReads, nVertices, nUVs, nNormals, nTangents, nPolygons, nIndices, nSubMeshes, nAllCnt;

	ReadStringFromFile(pFile, pStrTocken);

	for (; ;)
	{
		ReadStringFromFile(pFile, pStrTocken);

		if (!strcmp(pStrTocken, "<ControlPoints>:"))
		{
			nReads = (UINT)fread(&nVertices, sizeof(int), 1, pFile);
			
			if (nVertices)
			{
				_staticMeshInfo.vertices.resize(nVertices);
				Vec3* pos = new Vec3[nVertices];

				nReads = (UINT)fread(pos, sizeof(Vec3), nVertices, pFile);

				for (int i = 0; i < nVertices; ++i)
					_staticMeshInfo.vertices[i].pos = pos[i];
			}
		}

		else if (!strcmp(pStrTocken, "<UVs>:"))
		{
			nReads = (UINT)fread(&nUVs, sizeof(int), 1, pFile);
			nReads = (UINT)fread(&nAllCnt, sizeof(int), 1, pFile);
			ReadStringFromFile(pFile, pStrTocken);
			nReads = (UINT)fread(&nAllCnt, sizeof(int), 1, pFile);

			if (nUVs)
			{
				Vec2* uv = new Vec2[nUVs];

				nReads = (UINT)fread(uv, sizeof(Vec2), nUVs, pFile);

				for (int i = 0; i < nUVs; ++i)
					_staticMeshInfo.vertices[i].uv = uv[i];
			}
		}

		else if (!strcmp(pStrTocken, "<Normals>:"))
		{
			nReads = (UINT)fread(&nNormals, sizeof(int), 1, pFile);
			nReads = (UINT)fread(&nAllCnt, sizeof(int), 1, pFile);
			ReadStringFromFile(pFile, pStrTocken);
			nReads = (UINT)fread(&nAllCnt, sizeof(int), 1, pFile);

			if (nNormals)
			{
				Vec3* normal = new Vec3[nNormals];

				nReads = (UINT)fread(normal, sizeof(Vec3), nNormals, pFile);

				for (int i = 0; i < nNormals; ++i)
					_staticMeshInfo.vertices[i].normal = normal[i];
			}
		}

		else if (!strcmp(pStrTocken, "<Tangents>:"))
		{
			nReads = (UINT)fread(&nTangents, sizeof(int), 1, pFile);
			nReads = (UINT)fread(&nAllCnt, sizeof(int), 1, pFile);
			ReadStringFromFile(pFile, pStrTocken);
			nReads = (UINT)fread(&nAllCnt, sizeof(int), 1, pFile);

			if (nTangents)
			{
				Vec3* tangent = new Vec3[nTangents];

				nReads = (UINT)fread(tangent, sizeof(Vec3), nTangents, pFile);

				for (int i = 0; i < nTangents; ++i)
					_staticMeshInfo.vertices[i].tangent = tangent[i];
			}
		}

		else if (!strcmp(pStrTocken, "<Polygons>:"))
		{
			nReads = (UINT)fread(&nPolygons, sizeof(int), 1, pFile);

			ReadStringFromFile(pFile, pStrTocken);

			if (!strcmp(pStrTocken, "<SubIndices>:"))
			{
				nIndices = ReadIntegerFromFile(pFile);
				nSubMeshes = ReadIntegerFromFile(pFile);

				_staticMeshInfo.indices.resize(nSubMeshes);

				ReadStringFromFile(pFile, pStrTocken);

				if (!strcmp(pStrTocken, "<SubIndex>:"))
				{
					int nIndex = ReadIntegerFromFile(pFile);
					nIndices = ReadIntegerFromFile(pFile);	// 1470

					// ���� �ε��� �о��
					UINT* subIndices = new UINT[nIndices];
					nReads = (UINT)fread(subIndices, sizeof(UINT), nIndices, pFile);

					for (int i = 0; i < nIndices; ++i)
					{
						_staticMeshInfo.indices[nSubMeshes - 1].push_back(subIndices[i]);
					}
				}
			}

		}
	}
}

void MeshData::LoadMaterialInfoFromFile(FILE* pFile)
{
	char pStrTocken[64] = { '\0' };

}
