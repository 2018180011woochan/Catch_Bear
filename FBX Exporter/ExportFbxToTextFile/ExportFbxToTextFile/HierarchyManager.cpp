#include <fbxsdk.h>
#include "HierarchyManager.h"
#include "Display.h"

CHierarchyManager* CHierarchyManager::m_pInstance = nullptr;

FbxAMatrix CHierarchyManager::GetGeometricOffsetTransform(FbxNode* pfbxNode)
{
	// �������� ��ȯ: ����� ������ȯ�� ���� �� ����� �Ӽ��� �����
	const FbxVector4 T = pfbxNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 R = pfbxNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 S = pfbxNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return(FbxAMatrix(T, R, S));
}

void CHierarchyManager::Display_AllHierarchy(FbxScene* pfbxScene)
{
    // pfbxScene���� ��Ʈ���
    FbxNode* pfbxRootNode = pfbxScene->GetRootNode();

    int nFrame = 0;
    // pfbxRootNode->GetChildCount(): ��Ʈ��尡 �����ִ� child�鿡 ���� ���
    for (int i = 0; i < pfbxRootNode->GetChildCount(); i++, nFrame++)
    {
        Display_ChildHierarchy(&nFrame, pfbxRootNode->GetChild(i), 1);
    }
}

void CHierarchyManager::Display_ChildHierarchy(int* pnFrame, FbxNode* pfbxNode, int nTabIndents)
{
	// 1. �������� �̸�
	m_Display.DisplayIntString("<Frame>: ", *pnFrame, m_Display.ReplaceBlank(pfbxNode->GetName(), '_'), "\n", nTabIndents);

	// EvaluateLocalTransform���� ��ȯ����� �����ٰ�
	FbxAMatrix fbxmtxLocalTransform = pfbxNode->EvaluateLocalTransform(FBXSDK_TIME_INFINITE);

	// ���� ��尡 �����ִ� SRT ������(���� �ִϸ��̼� �ݿ�X)
	FbxDouble3 T = pfbxNode->LclTranslation.Get();
	FbxDouble3 R = pfbxNode->LclRotation.Get();
	FbxDouble3 S = pfbxNode->LclScaling.Get();

	// 2. ��ȯ��ķ� ���
	m_Display.WriteTransform("<Transform>: ", fbxmtxLocalTransform, S, R, T, "\n", nTabIndents + 1);

	// ����� ��Ʈ����Ʈ
	FbxNodeAttribute* pfbxNodeAttribute = pfbxNode->GetNodeAttribute();
	if (pfbxNodeAttribute) {
		FbxNodeAttribute::EType nAttributeType = pfbxNodeAttribute->GetAttributeType();

		// ����� ��Ʈ����Ʈ: �޽�
		if (nAttributeType == FbxNodeAttribute::eMesh)
		{
			// �޽�: ��Ų�޽�
			FbxMesh* pfbxMesh = (FbxMesh*)pfbxNode->GetNodeAttribute();

			// ��Ų �������� ����
			int nSkinDeformers = pfbxMesh->GetDeformerCount(FbxDeformer::eSkin);
			if (nSkinDeformers > 0)
			{
				// 3-1. ��Ų �������̼� ������ ���
				m_Display.DisplayString("<SkinDeformations>: ", "", "\n", nTabIndents + 1);
				Display_SkinDeformations(pfbxMesh, nTabIndents + 2);
				m_Display.DisplayString("</SkinDeformations>", "", "\n", nTabIndents + 1);
			}

			// 3. �޽� ���
			m_Display.DisplayString("<Mesh>: ", m_Display.ReplaceBlank(pfbxMesh->GetName(), '_'), "\n", nTabIndents + 1);
			DisplayMesh(pfbxMesh, nTabIndents + 2);
			// 4. ���� ���
			//DisplayString("</Mesh>", "", "\n", nTabIndents + 1);

			//DisplayMaterials(pfbxMesh, nTabIndents + 1); //DisplayTexture(pfbxMesh, nTabIndents + 1);
		}
	}

	////////////////////////////////////////////////////
	// �ڽĳ�� ���
	////////////////////////////////////////////////////

	// ���� ��尡 ���� �ִ� child ����
	int nChilds = pfbxNode->GetChildCount();
	m_Display.DisplayInt("<Children>: ", nChilds, "\n", nTabIndents + 1);

	for (int i = 0; i < nChilds; i++)
	{
		// ��� �����ӿ� ���� ���� ����� �� �������� ��ȣ�� ����(�� ��° ���������� �� �� �ֵ���)
		(*pnFrame)++;
		// ������ child ���� ���
		Display_ChildHierarchy(pnFrame, pfbxNode->GetChild(i), nTabIndents + 1);
	}

	m_Display.DisplayString("</Frame>", "", "\n", nTabIndents);
}

void CHierarchyManager::Display_SkinDeformations(FbxMesh* pfbxMesh, int nTabIndents)
{
	// �޽��� ��Ų ������ ��ȯ
	FbxSkin* pfbxSkinDeformer = (FbxSkin*)pfbxMesh->GetDeformer(0, FbxDeformer::eSkin);
	// ��Ų �����Ӱ� �����ִ� Ŭ������ ����
	int nClusters = pfbxSkinDeformer->GetClusterCount();

	// �� ��Ų�� ������ �ִ� ������ �̸�
	m_Display.DisplayInt("<BoneNames>: ", nClusters, " ", nTabIndents);

	// Ŭ�����͵��� ���� ���
	for (int j = 0; j < nClusters; j++)
	{
		// Ŭ�����Ϳ� ���� ��Ų��(��Ų ������)�� ������
		FbxCluster* pfbxCluster = pfbxSkinDeformer->GetCluster(j);

		// �������� ������ �ִ� ���(����)
		FbxNode* pfbxClusterLinkNode = pfbxCluster->GetLink();
		m_Display.DisplayString("", m_Display.ReplaceBlank(pfbxClusterLinkNode->GetName(), '_'), " ");
	}
	WriteStringToFile("\n");

	m_Display.DisplayInt("<BoneOffsets>: ", nClusters, " ", nTabIndents);

	// bone offset����� ����� ���� �޽����� ���� ��ȯ�ϱ� ���� ����� �����´�
	// - GetNode(): pfbxMesh�� ������ ��带 ��ȯ
	FbxAMatrix fbxmtxGeometryOffset = GetGeometricOffsetTransform(pfbxMesh->GetNode());
	for (int j = 0; j < nClusters; j++)
	{
		FbxCluster* pfbxCluster = pfbxSkinDeformer->GetCluster(j);

		FbxAMatrix fbxmtxBindPoseMeshToRoot; //Cluster Transform
		pfbxCluster->GetTransformMatrix(fbxmtxBindPoseMeshToRoot);		// �θ���������� �������
		FbxAMatrix fbxmtxBindPoseBoneToRoot; //Cluster Link Transform
		pfbxCluster->GetTransformLinkMatrix(fbxmtxBindPoseBoneToRoot);	// �������� �������

		// fbxmtxVertextToLinkNode: ������ ��ȯ ���
		FbxAMatrix fbxmtxVertextToLinkNode = fbxmtxBindPoseBoneToRoot.Inverse() * fbxmtxBindPoseMeshToRoot * fbxmtxGeometryOffset;

		m_Display.DisplayMatrix(fbxmtxVertextToLinkNode);
	}
	WriteStringToFile("\n");

	// ������(����)�� ����
	int nControlPoints = pfbxMesh->GetControlPointsCount();

	// pnBonesPerVertex: �������� ������ �ִ� ������ �迭
	int* pnBonesPerVertex = new int[nControlPoints];
	::memset(pnBonesPerVertex, 0, nControlPoints * sizeof(int));

	for (int j = 0; j < nClusters; j++)
	{
		FbxCluster* pfbxCluster = pfbxSkinDeformer->GetCluster(j);

		// GetControlPointIndicesCount(): ������ �ε��� �� ����ġ �迭�� ���� ��ȯ
		int nControlPointIndices = pfbxCluster->GetControlPointIndicesCount();
		// GetControlPointIndices(): ������ �ε����� �迭 ��ȯ
		int* pnControlPointIndices = pfbxCluster->GetControlPointIndices();
		// ������ ������ �ִ� ������ ������ �迭�� ä�� �ִ´�
		// -> �ϳ��� ������ ���� ������ �ִ� ���� �� ���� �ִ��� �� �� �ִ�
		for (int k = 0; k < nControlPointIndices; k++) pnBonesPerVertex[pnControlPointIndices[k]] += 1;
	}
	//int nMaxBonesPerVertex = 0;
	//for (int i = 0; i < nControlPoints; i++) if (pnBonesPerVertex[i] > nMaxBonesPerVertex) nMaxBonesPerVertex = pnBonesPerVertex[i];

	// ���� �����ϴ�!
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	// Ŭ�����Ϳ� �ִ� �������� ������ ������ �ִ� ������ ����Ʈ&����ġ�� ���� ����
	// ��� Ŭ�����͵��� �湮�ϸ鼭 ������ �ε����� �� �� ��Ÿ������ Ȯ���ؼ� 
	// �޸𸮸� �Ҵ��ϰ� �ű⿡ �������� ������ �ִ� ���� ��ȣ��&����ġ�� ����� �� �ֵ��� ó���Ѵ�
	///////////////////////////////////////////////////////////////////////////////////////////////////////

	// ���� ������ �ִ� ������ŭ ������ ID�� ����ġ�� �迭�� �����
	int** ppnBoneIDs = new int* [nControlPoints];
	float** ppnBoneWeights = new float* [nControlPoints];
	for (int i = 0; i < nControlPoints; i++)
	{
		ppnBoneIDs[i] = new int[pnBonesPerVertex[i]];
		ppnBoneWeights[i] = new float[pnBonesPerVertex[i]];
		::memset(ppnBoneIDs[i], 0, pnBonesPerVertex[i] * sizeof(int));
		::memset(ppnBoneWeights[i], 0, pnBonesPerVertex[i] * sizeof(float));
	}

	int* pnBones = new int[nControlPoints];
	::memset(pnBones, 0, nControlPoints * sizeof(int));

	// Ŭ�����Ϳ� �ִ� 
	for (int j = 0; j < nClusters; j++)	// BoneID
	{
		FbxCluster* pfbxCluster = pfbxSkinDeformer->GetCluster(j);

		int* pnControlPointIndices = pfbxCluster->GetControlPointIndices();
		double* pfControlPointWeights = pfbxCluster->GetControlPointWeights();
		int nControlPointIndices = pfbxCluster->GetControlPointIndicesCount();

		// ��� �������鿡 ���� ������ �ִ� ������ ����/����ġ�� �������� ����
		for (int k = 0; k < nControlPointIndices; k++)
		{
			int nVertex = pnControlPointIndices[k];
			ppnBoneIDs[nVertex][pnBones[nVertex]] = j;
			ppnBoneWeights[nVertex][pnBones[nVertex]] = (float)pfControlPointWeights[k];
			pnBones[nVertex] += 1;
		}
	}

	// ���� ����ġ���� �����Ѵ�
	// : �ϳ��� ������ ������ �ִ� ���� ������ �װ��� ����ġ�� ū ������� �����ϰ�
	//	 �װ��� 4�������� ����ϵ��� ����ڴ�
	for (int i = 0; i < nControlPoints; i++)
	{
		for (int j = 0; j < pnBonesPerVertex[i] - 1; j++)
		{
			for (int k = j + 1; k < pnBonesPerVertex[i]; k++)
			{
				if (ppnBoneWeights[i][j] < ppnBoneWeights[i][k])
				{
					float fTemp = ppnBoneWeights[i][j];
					ppnBoneWeights[i][j] = ppnBoneWeights[i][k];
					ppnBoneWeights[i][k] = fTemp;
					int nTemp = ppnBoneIDs[i][j];
					ppnBoneIDs[i][j] = ppnBoneIDs[i][k];
					ppnBoneIDs[i][k] = nTemp;
				}
			}
		}
	}

	int(*pnSkinningIndices)[4] = new int[nControlPoints][4];
	float(*pfSkinningWeights)[4] = new float[nControlPoints][4];

	for (int i = 0; i < nControlPoints; i++)
	{
		::memset(pnSkinningIndices[i], 0, 4 * sizeof(int));
		::memset(pfSkinningWeights[i], 0, 4 * sizeof(float));

		for (int j = 0; j < pnBonesPerVertex[i]; j++)
		{
			if (j < 4)
			{
				pnSkinningIndices[i][j] = ppnBoneIDs[i][j];
				pfSkinningWeights[i][j] = ppnBoneWeights[i][j];
			}
		}
	}

	for (int i = 0; i < nControlPoints; i++)
	{
		float nSumOfBoneWeights = 0.0f;
		for (int j = 0; j < 4; j++) nSumOfBoneWeights += pfSkinningWeights[i][j];
		for (int j = 0; j < 4; j++) pfSkinningWeights[i][j] /= nSumOfBoneWeights;
	}

	// ��Ű�� ������ �ε���&����ġ�� �����鸶�� ���
	m_Display.DisplayInt("<BoneIndices>: ", nControlPoints, " ", nTabIndents);
	for (int i = 0; i < nControlPoints; i++)
	{
		// ���� �ε���&����ġ�� �ִ� 4�������� �������(���� �� �����ص� �ȴ�, ���� 4�������� ����)
		m_Display.DisplayInt(pnSkinningIndices[i][0], pnSkinningIndices[i][1], pnSkinningIndices[i][2], pnSkinningIndices[i][3]);
	}
	WriteStringToFile("\n");

	m_Display.DisplayInt("<BoneWeights>: ", nControlPoints, " ", nTabIndents);
	for (int i = 0; i < nControlPoints; i++)
	{
		m_Display.DisplayFloat(pfSkinningWeights[i][0], pfSkinningWeights[i][1], pfSkinningWeights[i][2], pfSkinningWeights[i][3]);
	}
	WriteStringToFile("\n");

	// ������ �ε���&����ġ�� ���� ���۷� ���� ���̴��� �ѱ�� �ȴ�!!

	// �޸� ����
	for (int i = 0; i < nControlPoints; i++)
	{
		if (ppnBoneIDs[i]) delete[] ppnBoneIDs[i];
		if (ppnBoneWeights[i]) delete[] ppnBoneWeights[i];
	}
	if (ppnBoneIDs) delete[] ppnBoneIDs;
	if (ppnBoneWeights) delete[] ppnBoneWeights;

	if (pnBones) delete[] pnBones;
	if (pnBonesPerVertex) delete[] pnBonesPerVertex;
	if (pnSkinningIndices) delete[] pnSkinningIndices;
	if (pfSkinningWeights) delete[] pfSkinningWeights;
}

void CHierarchyManager::DisplayMesh(FbxMesh* pfbxMesh, int nTabIndents)
{
	int nControlPoints = pfbxMesh->GetControlPointsCount();
	if (nControlPoints > 0)
	{
		DisplayControlPoints(pfbxMesh, nControlPoints, nTabIndents);

		DisplayControlPointUVs(pfbxMesh, nControlPoints, nTabIndents); //UVs Per Vertex

		DisplayControlPointNormals(pfbxMesh, nControlPoints, nTabIndents);
		DisplayControlPointTangents(pfbxMesh, nControlPoints, nTabIndents);
		DisplayControlPointBiTangents(pfbxMesh, nControlPoints, nTabIndents);
	}

	int nPolygons = pfbxMesh->GetPolygonCount();
	if (nPolygons > 0) DisplayPolygons(pfbxMesh, nPolygons, nTabIndents);
}

void CHierarchyManager::DisplayControlPoints(FbxMesh* pfbxMesh, int nControlPoints, int nTabIndents)
{
	m_Display.DisplayInt("<ControlPoints>: ", nControlPoints, " ", nTabIndents);

	// GetControlPoints(): ������(����) �迭 ������ ��ȯ
	FbxVector4* pfbxvControlPoints = pfbxMesh->GetControlPoints();
	for (int i = 0; i < nControlPoints; i++) m_Display.Display3DVector(pfbxvControlPoints[i]);
	WriteStringToFile("\n");
}

void CHierarchyManager::DisplayControlPointUVs(FbxMesh* pfbxMesh, int nControlPoints, int nTabIndents)
{
	FbxVector2* pControlPointUVs = new FbxVector2[nControlPoints];

	// GetElementUVCount(): �޽��� �����ִ� uv �� ����
	int nUVsPerVertex = pfbxMesh->GetElementUVCount(); //UVs Per Polygon's Vertex
	m_Display.DisplayInt("<UVs>: ", nControlPoints, nUVsPerVertex, "\n", nTabIndents);

	if (nUVsPerVertex > 0)
	{
		// GetPolygonCount(): �޽��� ������(�ﰢ��) ���� ��ȯ
		int nPolygons = pfbxMesh->GetPolygonCount();

		for (int k = 0; k < nUVsPerVertex; k++)
		{
			m_Display.DisplayInt("<UV>: ", k, " ", nTabIndents + 1);
			FbxGeometryElementUV* pfbxElementUV = pfbxMesh->GetElementUV(k);

			for (int i = 0; i < nPolygons; i++)	// �ﰢ���� ����
			{
				// �ε��̵� �������� ���� ��
				int nPolygonSize = pfbxMesh->GetPolygonSize(i); //Triangle: 3, Triangulate()
				for (int j = 0; j < nPolygonSize; j++)	// �ﰢ���� �̷�� �ִ� ����
				{
					int nControlPointIndex = pfbxMesh->GetPolygonVertex(i, j);

					// ���εǴ� ����� ���� �����ϴ� ����� �ٸ���!!!
					switch (pfbxElementUV->GetMappingMode())	// ���̾� ���Ұ� �޽��� ǥ�鿡 ���εǴ� ���
					{
					case FbxGeometryElement::eByControlPoint:
					{
						switch (pfbxElementUV->GetReferenceMode())	// ���� ������ ����� ��ġ(��ǥ���� �迭)�� ���� ���� ���
						{
						case FbxGeometryElement::eDirect:
							pControlPointUVs[nControlPointIndex] = pfbxElementUV->GetDirectArray().GetAt(nControlPointIndex);
							break;
						case FbxGeometryElement::eIndexToDirect:
							pControlPointUVs[nControlPointIndex] = pfbxElementUV->GetDirectArray().GetAt(pfbxElementUV->GetIndexArray().GetAt(nControlPointIndex));
							break;
						default:
							break;
						}
						break;
					}
					case FbxGeometryElement::eByPolygonVertex:
					{
						int nTextureUVIndex = pfbxMesh->GetTextureUVIndex(i, j);
						switch (pfbxElementUV->GetReferenceMode())
						{
						case FbxGeometryElement::eDirect:
						case FbxGeometryElement::eIndexToDirect:
							pControlPointUVs[nControlPointIndex] = pfbxElementUV->GetDirectArray().GetAt(nTextureUVIndex);
							break;
						default:
							break;
						}
						break;
					}
					case FbxGeometryElement::eByPolygon:
					case FbxGeometryElement::eAllSame:
					case FbxGeometryElement::eNone:
						break;
					default:
						break;
					}
				}
			}
			for (int i = 0; i < nControlPoints; i++) m_Display.DisplayUV2DVector(pControlPointUVs[i]);
			WriteStringToFile("\n");
		}
	}
}

void CHierarchyManager::DisplayControlPointNormals(FbxMesh* pfbxMesh, int nControlPoints, int nTabIndents)
{
	FbxVector4* pControlPointNormals = new FbxVector4[nControlPoints];

	// �븻 ����
	int nNormalsPerVertex = pfbxMesh->GetElementNormalCount();
	m_Display.DisplayInt("<Normals>: ", nControlPoints, nNormalsPerVertex, "\n", nTabIndents);

	if (nNormalsPerVertex > 0)
	{
		// �޽��� �ﰢ�� ����
		int nPolygons = pfbxMesh->GetPolygonCount();

		for (int k = 0; k < nNormalsPerVertex; k++)
		{
			m_Display.DisplayInt("<Normal>: ", k, " ", nTabIndents + 1);

			// ������ �븻
			FbxGeometryElementNormal* pfbxElementNormal = pfbxMesh->GetElementNormal(k);
			if (pfbxElementNormal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
			{
				if (pfbxElementNormal->GetReferenceMode() == FbxGeometryElement::eDirect)
				{
					for (int i = 0; i < nControlPoints; i++) pControlPointNormals[i] = pfbxElementNormal->GetDirectArray().GetAt(i);
				}
			}
			else if (pfbxElementNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
			{
				for (int i = 0, nVertexID = 0; i < nPolygons; i++)
				{
					int nPolygonSize = pfbxMesh->GetPolygonSize(i);
					for (int j = 0; j < nPolygonSize; j++, nVertexID++)
					{
						int nControlPointIndex = pfbxMesh->GetPolygonVertex(i, j);
						switch (pfbxElementNormal->GetReferenceMode())
						{
						case FbxGeometryElement::eDirect:
							pControlPointNormals[nControlPointIndex] = pfbxElementNormal->GetDirectArray().GetAt(nVertexID);
							break;
						case FbxGeometryElement::eIndexToDirect:
							pControlPointNormals[nControlPointIndex] = pfbxElementNormal->GetDirectArray().GetAt(pfbxElementNormal->GetIndexArray().GetAt(nVertexID));
							break;
						default:
							break;
						}
					}
				}
			}

			for (int i = 0; i < nControlPoints; i++) m_Display.Display3DVector(pControlPointNormals[i]);
			WriteStringToFile("\n");
		}
	}
}

void CHierarchyManager::DisplayControlPointTangents(FbxMesh* pfbxMesh, int nControlPoints, int nTabIndents)
{
	FbxVector4* pControlPointTangents = new FbxVector4[nControlPoints];

	// ź��Ʈ ���� ����
	int nTangentsPerVertex = pfbxMesh->GetElementTangentCount();
	m_Display.DisplayInt("<Tangents>: ", nControlPoints, nTangentsPerVertex, "\n", nTabIndents);

	if (nTangentsPerVertex > 0)
	{
		// �޽��� �ﰢ�� ����
		int nPolygons = pfbxMesh->GetPolygonCount();

		for (int k = 0; k < nTangentsPerVertex; k++)
		{
			m_Display.DisplayInt("<Tangent>: ", k, " ", nTabIndents + 1);

			// ������ ź��Ʈ ����
			FbxGeometryElementTangent* pfbxElementTangent = pfbxMesh->GetElementTangent(k);
			if (pfbxElementTangent->GetMappingMode() == FbxGeometryElement::eByControlPoint)
			{
				if (pfbxElementTangent->GetReferenceMode() == FbxGeometryElement::eDirect)
				{
					for (int i = 0; i < nControlPoints; i++) pControlPointTangents[i] = pfbxElementTangent->GetDirectArray().GetAt(i);
				}
			}
			else if (pfbxElementTangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
			{
				for (int i = 0, nVertexID = 0; i < nPolygons; i++)
				{
					int nPolygonSize = pfbxMesh->GetPolygonSize(i);
					for (int j = 0; j < nPolygonSize; j++, nVertexID++)
					{
						int nControlPointIndex = pfbxMesh->GetPolygonVertex(i, j);
						switch (pfbxElementTangent->GetReferenceMode())
						{
						case FbxGeometryElement::eDirect:
							pControlPointTangents[nControlPointIndex] = pfbxElementTangent->GetDirectArray().GetAt(nVertexID);
							break;
						case FbxGeometryElement::eIndexToDirect:
							pControlPointTangents[nControlPointIndex] = pfbxElementTangent->GetDirectArray().GetAt(pfbxElementTangent->GetIndexArray().GetAt(nVertexID));
							break;
						default:
							break;
						}
					}
				}
			}

			for (int i = 0; i < nControlPoints; i++) m_Display.Display3DVector(pControlPointTangents[i]);
			WriteStringToFile("\n");
		}
	}
}

void CHierarchyManager::DisplayControlPointBiTangents(FbxMesh* pfbxMesh, int nControlPoints, int nTabIndents)
{
	FbxVector4* pControlPointBiTangents = new FbxVector4[nControlPoints];

	// BiNormal ����
	int nBiTangentsPerVertex = pfbxMesh->GetElementBinormalCount();
	m_Display.DisplayInt("<BiTangents>: ", nControlPoints, nBiTangentsPerVertex, "\n", nTabIndents);

	if (nBiTangentsPerVertex > 0)
	{
		// �޽��� �ﰢ�� ����
		int nPolygons = pfbxMesh->GetPolygonCount();

		for (int k = 0; k < nBiTangentsPerVertex; k++)
		{
			m_Display.DisplayInt("<BiTangent>: ", k, " ", nTabIndents + 1);

			// BiNormal 
			FbxGeometryElementBinormal* pfbxElementBiTangent = pfbxMesh->GetElementBinormal(k);
			if (pfbxElementBiTangent->GetMappingMode() == FbxGeometryElement::eByControlPoint)
			{
				if (pfbxElementBiTangent->GetReferenceMode() == FbxGeometryElement::eDirect)
				{
					for (int i = 0; i < nControlPoints; i++) pControlPointBiTangents[i] = pfbxElementBiTangent->GetDirectArray().GetAt(i);
				}
			}
			else if (pfbxElementBiTangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
			{
				for (int i = 0, nVertexID = 0; i < nPolygons; i++)
				{
					int nPolygonSize = pfbxMesh->GetPolygonSize(i);
					for (int j = 0; j < nPolygonSize; j++, nVertexID++)
					{
						int nControlPointIndex = pfbxMesh->GetPolygonVertex(i, j);
						switch (pfbxElementBiTangent->GetReferenceMode())
						{
						case FbxGeometryElement::eDirect:
							pControlPointBiTangents[nControlPointIndex] = pfbxElementBiTangent->GetDirectArray().GetAt(nVertexID);
							break;
						case FbxGeometryElement::eIndexToDirect:
							pControlPointBiTangents[nControlPointIndex] = pfbxElementBiTangent->GetDirectArray().GetAt(pfbxElementBiTangent->GetIndexArray().GetAt(nVertexID));
							break;
						default:
							break;
						}
					}
				}
			}

			for (int i = 0; i < nControlPoints; i++) m_Display.Display3DVector(pControlPointBiTangents[i]);
			WriteStringToFile("\n");
		}
	}
}

void CHierarchyManager::DisplayPolygons(FbxMesh* pfbxMesh, int nPolygons, int nTabIndents)
{
	m_Display.DisplayInt("<Polygons>: ", nPolygons, "\n", nTabIndents);

	DisplayPolygonVertexIndices(pfbxMesh, nPolygons, nTabIndents + 1);

	//DisplayPolygonVertexColors(pfbxMesh, nPolygons, nTabIndents + 1);
	//DisplayPolygonVertexUVs(pfbxMesh, nPolygons, nTabIndents + 1); //UVs Per Polygon
	//DisplayPolygonVertexNormals(pfbxMesh, nPolygons, nTabIndents + 1);
	//DisplayPolygonVertexTangents(pfbxMesh, nPolygons, nTabIndents + 1);
	//DisplayPolygonVertexBinormals(pfbxMesh, nPolygons, nTabIndents + 1);

	m_Display.DisplayString("</Polygons>", "", "\n", nTabIndents);
}

void CHierarchyManager::DisplayPolygonVertexIndices(FbxMesh* pfbxMesh, int nPolygons, int nTabIndents)
{
	// 1. ����
	int nPolygonIndices = nPolygons * 3; //Triangle: 3, Triangulate(), nIndices = nPolygons * 3

	int* pnPolygonIndices = new int[nPolygonIndices];
	for (int i = 0, k = 0; i < nPolygons; i++)
	{
		for (int j = 0; j < 3; j++) pnPolygonIndices[k++] = pfbxMesh->GetPolygonVertex(i, j);
	}

	FbxNode* pfbxNode = pfbxMesh->GetNode();
	int nMaterials = pfbxNode->GetMaterialCount();
	m_Display.DisplayInt("<SubIndices>: ", nPolygonIndices, nMaterials, "\n", nTabIndents);

	if (nMaterials > 1)
	{
		// �޽��� ���� ��� ����
		int nElementMaterials = pfbxMesh->GetElementMaterialCount();
		for (int i = 0; i < nElementMaterials; i++)
		{
			// ����, ���� ����
			FbxGeometryElementMaterial* pfbxElementMaterial = pfbxMesh->GetElementMaterial(i);
			FbxGeometryElement::EReferenceMode nReferenceMode = pfbxElementMaterial->GetReferenceMode();
			switch (nReferenceMode)
			{
			case FbxGeometryElement::eDirect:
			{
				m_Display.DisplayInt("<SubIndex>: ", 0, nPolygonIndices, "", nTabIndents + 1);
				for (int i = 0; i < nPolygonIndices; i++) m_Display.DisplayInt(pnPolygonIndices[i]);
				WriteStringToFile("\n");
				break;
			}
			case FbxGeometryElement::eIndex:
			case FbxGeometryElement::eIndexToDirect:
			{
				int* pnSubIndices = new int[nMaterials];
				memset(pnSubIndices, 0, sizeof(int) * nMaterials);

				int nSubIndices = pfbxElementMaterial->GetIndexArray().GetCount();
				for (int j = 0; j < nSubIndices; j++) pnSubIndices[pfbxElementMaterial->GetIndexArray().GetAt(j)]++;

				int** ppnSubIndices = new int* [nMaterials];
				for (int k = 0; k < nMaterials; k++)
				{
					pnSubIndices[k] *= 3;
					ppnSubIndices[k] = new int[pnSubIndices[k]];
				}

				int* pnToAppends = new int[nMaterials];
				memset(pnToAppends, 0, sizeof(int) * nMaterials);
				for (int j = 0, k = 0; j < nSubIndices; j++)
				{
					int nMaterial = pfbxElementMaterial->GetIndexArray().GetAt(j);
					for (int i = 0; i < 3; i++) ppnSubIndices[nMaterial][pnToAppends[nMaterial]++] = pnPolygonIndices[k++];
				}

				for (int k = 0; k < nMaterials; k++)
				{
					m_Display.DisplayInt("<SubIndex>: ", k, pnSubIndices[k], "", nTabIndents + 1);
					for (int j = 0; j < pnSubIndices[k]; j++) m_Display.DisplayInt(ppnSubIndices[k][j]);
					WriteStringToFile("\n");
				}

				if (pnSubIndices) delete[] pnSubIndices;
				for (int k = 0; k < nMaterials; k++) if (ppnSubIndices[k]) delete[] ppnSubIndices[k];
				if (ppnSubIndices) delete[] ppnSubIndices;
				if (pnToAppends) delete[] pnToAppends;

				break;
			}
			}
		}
	}
	else
	{
		m_Display.DisplayInt("<SubIndex>: ", 0, nPolygonIndices, "", nTabIndents + 1);
		for (int i = 0; i < nPolygonIndices; i++) m_Display.DisplayInt(pnPolygonIndices[i]);
		WriteStringToFile("\n");
	}

	if (pnPolygonIndices) delete[] pnPolygonIndices;
}

