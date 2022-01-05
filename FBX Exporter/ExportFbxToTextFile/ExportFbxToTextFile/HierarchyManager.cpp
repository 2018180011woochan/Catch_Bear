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
    // pfbxScene신의 루트노드
    FbxNode* pfbxRootNode = pfbxScene->GetRootNode();

    int nFrame = 0;
    // pfbxRootNode->GetChildCount(): 루트노드가 갖고있는 child들에 대해 출력
    for (int i = 0; i < pfbxRootNode->GetChildCount(); i++, nFrame++)
    {
        Display_ChildHierarchy(&nFrame, pfbxRootNode->GetChild(i), 1);
    }
}

void CHierarchyManager::Display_ChildHierarchy(int* pnFrame, FbxNode* pfbxNode, int nTabIndents)
{
	// 1. 프레임의 이름
	m_Display.DisplayIntString("<Frame>: ", *pnFrame, m_Display.ReplaceBlank(pfbxNode->GetName(), '_'), "\n", nTabIndents);

	// EvaluateLocalTransform로컬 변환행렬을 가져다가
	FbxAMatrix fbxmtxLocalTransform = pfbxNode->EvaluateLocalTransform(FBXSDK_TIME_INFINITE);

	// 원래 노드가 갖고있는 SRT 정보들(현재 애니메이션 반영X)
	FbxDouble3 T = pfbxNode->LclTranslation.Get();
	FbxDouble3 R = pfbxNode->LclRotation.Get();
	FbxDouble3 S = pfbxNode->LclScaling.Get();

	// 2. 변환행렬로 출력
	m_Display.WriteTransform("<Transform>: ", fbxmtxLocalTransform, S, R, T, "\n", nTabIndents + 1);

	// 노드의 어트리뷰트
	FbxNodeAttribute* pfbxNodeAttribute = pfbxNode->GetNodeAttribute();
	if (pfbxNodeAttribute) {
		FbxNodeAttribute::EType nAttributeType = pfbxNodeAttribute->GetAttributeType();

		// 노드의 어트리뷰트: 메쉬
		if (nAttributeType == FbxNodeAttribute::eMesh)
		{
			// 메쉬: 스킨메쉬
			FbxMesh* pfbxMesh = (FbxMesh*)pfbxNode->GetNodeAttribute();

			// 스킨 디포머의 개수
			int nSkinDeformers = pfbxMesh->GetDeformerCount(FbxDeformer::eSkin);
			if (nSkinDeformers > 0)
			{
				// 3-1. 스킨 디포메이션 정보들 출력
				m_Display.DisplayString("<SkinDeformations>: ", "", "\n", nTabIndents + 1);
				Display_SkinDeformations(pfbxMesh, nTabIndents + 2);
				m_Display.DisplayString("</SkinDeformations>", "", "\n", nTabIndents + 1);
			}
		}
	}
}

void CHierarchyManager::Display_SkinDeformations(FbxMesh* pfbxMesh, int nTabIndents)
{
	// 메쉬의 스킨 디포머 반환
	FbxSkin* pfbxSkinDeformer = (FbxSkin*)pfbxMesh->GetDeformer(0, FbxDeformer::eSkin);
	// 스킨 디포머가 갖고있는 클러스터 개수
	int nClusters = pfbxSkinDeformer->GetClusterCount();

	// 이 스킨에 영향을 주는 뼈들의 이름
	m_Display.DisplayInt("<BoneNames>: ", nClusters, " ", nTabIndents);

	// 클러스터들의 정보 출력
	for (int j = 0; j < nClusters; j++)
	{
		// 클러스터에 대해 스킨값(스킨 데이터)을 가져옴
		FbxCluster* pfbxCluster = pfbxSkinDeformer->GetCluster(j);

		// 제어점에 영향을 주는 노드(뼈대)
		FbxNode* pfbxClusterLinkNode = pfbxCluster->GetLink();
		m_Display.DisplayString("", m_Display.ReplaceBlank(pfbxClusterLinkNode->GetName(), '_'), " ");
	}
	WriteStringToFile("\n");

	m_Display.DisplayInt("<BoneOffsets>: ", nClusters, " ", nTabIndents);

	// bone offset행렬을 만들기 위해 메쉬에서 노드로 변환하기 위한 행렬을 가져온다
	// - GetNode(): pfbxMesh이 설정된 노드를 반환
	FbxAMatrix fbxmtxGeometryOffset = GetGeometricOffsetTransform(pfbxMesh->GetNode());
	for (int j = 0; j < nClusters; j++)
	{
		FbxCluster* pfbxCluster = pfbxSkinDeformer->GetCluster(j);

		FbxAMatrix fbxmtxBindPoseMeshToRoot; //Cluster Transform
		pfbxCluster->GetTransformMatrix(fbxmtxBindPoseMeshToRoot);		// 부모공간으로의 월드행렬
		FbxAMatrix fbxmtxBindPoseBoneToRoot; //Cluster Link Transform
		pfbxCluster->GetTransformLinkMatrix(fbxmtxBindPoseBoneToRoot);	// 뼈공간의 월드행렬

		// fbxmtxVertextToLinkNode: 오프셋 변환 행렬
		FbxAMatrix fbxmtxVertextToLinkNode = fbxmtxBindPoseBoneToRoot.Inverse() * fbxmtxBindPoseMeshToRoot * fbxmtxGeometryOffset;

		m_Display.DisplayMatrix(fbxmtxVertextToLinkNode);
	}
	WriteStringToFile("\n");

	// 제어점의 개수
	int nControlPoints = pfbxMesh->GetControlPointsCount();

	// pnBonesPerVertex: 정점마다 영향을 주는 뼈들의 배열
	int* pnBonesPerVertex = new int[nControlPoints];
	::memset(pnBonesPerVertex, 0, nControlPoints * sizeof(int));

	for (int j = 0; j < nClusters; j++)
	{
		FbxCluster* pfbxCluster = pfbxSkinDeformer->GetCluster(j);

		int nControlPointIndices = pfbxCluster->GetControlPointIndicesCount();
		int* pnControlPointIndices = pfbxCluster->GetControlPointIndices();
		// 정점에 영향을 주는 뼈들의 개수를 배열에 채워 넣는다
		// -> 하나의 정점에 대해 영향을 주는 뼈가 몇 개가 있는지 알 수 있다
		for (int k = 0; k < nControlPointIndices; k++) pnBonesPerVertex[pnControlPointIndices[k]] += 1;
	}
	//int nMaxBonesPerVertex = 0;
	//for (int i = 0; i < nControlPoints; i++) if (pnBonesPerVertex[i] > nMaxBonesPerVertex) nMaxBonesPerVertex = pnBonesPerVertex[i];

	// 많이 복잡하다!
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	// 클러스터에 있는 정보들을 정점에 영향을 주는 뼈들의 리스트&가중치를 얻어내기 위해
	// 모든 클러스터들을 방문하면서 정점의 인덱스가 몇 번 나타나는지 확인해서 
	// 메모리를 할당하고 거기에 정점마다 영향을 주는 뼈의 번호들&가중치를 써놓을 수 있도록 처리한다
	///////////////////////////////////////////////////////////////////////////////////////////////////////

	// 뼈에 영향을 주는 개수만큼 뼈들의 ID와 가중치의 배열을 만든다
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

	// 클러스터에 있는 모든 제어점들에 대해 영향을 주는 뼈들의 정보/가중치를 정점마다 쓴다
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

	// 뼈의 가중치들을 정렬한다
	// : 하나의 정점에 영향을 주는 뼈가 많으면 그것을 가중치가 큰 순서대로 정렬하고
	//	 그것을 4개까지만 허용하도록 만들겠다
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

	// 스키닝 뼈들의 인덱스&가중치를 정점들마다 출력
	m_Display.DisplayInt("<BoneIndices>: ", nControlPoints, " ", nTabIndents);
	for (int i = 0; i < nControlPoints; i++)
	{
		// 뼈의 인덱스&가중치는 최대 4개까지만 만들었다(물론 다 저장해도 된다, 보통 4개까지만 쓴다)
		m_Display.DisplayInt(pnSkinningIndices[i][0], pnSkinningIndices[i][1], pnSkinningIndices[i][2], pnSkinningIndices[i][3]);
	}
	WriteStringToFile("\n");

	m_Display.DisplayInt("<BoneWeights>: ", nControlPoints, " ", nTabIndents);
	for (int i = 0; i < nControlPoints; i++)
	{
		m_Display.DisplayFloat(pfSkinningWeights[i][0], pfSkinningWeights[i][1], pfSkinningWeights[i][2], pfSkinningWeights[i][3]);
	}
	WriteStringToFile("\n");

	// 뼈들의 인덱스&가중치를 정점 버퍼로 만들어서 쉐이더로 넘기면 된다!!

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


