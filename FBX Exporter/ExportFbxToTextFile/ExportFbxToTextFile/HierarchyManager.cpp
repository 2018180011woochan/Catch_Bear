#include <fbxsdk.h>
#include "HierarchyManager.h"
#include "Display.h"

CHierarchyManager* CHierarchyManager::m_pInstance = nullptr;

FbxAMatrix CHierarchyManager::GetGeometricOffsetTransform(FbxNode* pfbxNode)
{
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
		}
	}
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

	// �������� ����
	int nControlPoints = pfbxMesh->GetControlPointsCount();

	// pnBonesPerVertex: �������� ������ �ִ� ������ �迭
	int* pnBonesPerVertex = new int[nControlPoints];
	::memset(pnBonesPerVertex, 0, nControlPoints * sizeof(int));

	for (int j = 0; j < nClusters; j++)
	{
		FbxCluster* pfbxCluster = pfbxSkinDeformer->GetCluster(j);

		int nControlPointIndices = pfbxCluster->GetControlPointIndicesCount();
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

	// Ŭ�����Ϳ� �ִ� ��� �������鿡 ���� ������ �ִ� ������ ����/����ġ�� �������� ����
	for (int j = 0; j < nClusters; j++)
	{
		FbxCluster* pfbxCluster = pfbxSkinDeformer->GetCluster(j);

		int* pnControlPointIndices = pfbxCluster->GetControlPointIndices();
		double* pfControlPointWeights = pfbxCluster->GetControlPointWeights();
		int nControlPointIndices = pfbxCluster->GetControlPointIndicesCount();

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


