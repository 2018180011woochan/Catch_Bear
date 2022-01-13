// ExportFbxToTextFile.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include "SdkManager.h"
#include "HierarchyManager.h"
#include "AnimationManager.h"

FILE* pFile = NULL;
char pszBuffer[256];

void WriteStringToFile(char* pszString) { ::fwrite(pszString, sizeof(char), strlen(pszString), pFile); }

int main(int argc, char** argv)
{
	// FbxSdk 생성/초기화
	FbxManager* pfbxSdkManager = NULL;

	CSdkManager::Get_Instance()->InitializeSdkObjects(pfbxSdkManager);

	// 읽어들일 fbx 파일 이름
	FbxString pfbxstrModelFilePath("Bush_01.fbx");
	// 출력할 txt 파일 이름
	char* pszWriteFileName = "Bush_01.txt";

	// FBX Scene 생성&로드
	FbxScene* pfbxModelScene = FbxScene::Create(pfbxSdkManager, pfbxstrModelFilePath.Buffer());
	bool bResult = CSdkManager::Get_Instance()->LoadScene(pfbxSdkManager, pfbxModelScene, pfbxstrModelFilePath.Buffer());

	// FbxGeometryConverter: GeometryNode 속성( FbxMesh, FbxNurbs 및 FbxPatch ) 변환하는 기능 제공 
	// 씬 내에서 삼각형화 할 수 있는 모든 노드를 삼각형화 시킨다.
	FbxGeometryConverter fbxGeomConverter(pfbxSdkManager);
	fbxGeomConverter.Triangulate(pfbxModelScene, true);

	// FbxSystemUnit: 특정 장면 내에서 사용되는 측정 단위를 설명
	FbxSystemUnit fbxSceneSystemUnit = pfbxModelScene->GetGlobalSettings().GetSystemUnit();
	if (fbxSceneSystemUnit.GetScaleFactor() != 1.0) FbxSystemUnit::cm.ConvertScene(pfbxModelScene);

	// 파일 입출력
	::fopen_s(&pFile, pszWriteFileName, "wt");

	// 2. 출력할 파일에 필요한 정보를 쓴다
	WriteStringToFile("<Hierarchy>\n");
	CHierarchyManager::Get_Instance()->Display_AllHierarchy(pfbxModelScene);
	WriteStringToFile("</Hierarchy>\n");

	WriteStringToFile("<Animation>\n");
#ifdef _WITH_SEPARATED_ANIMATIONS
	DisplayAnimation(ppfbxAnimationScenes, nSeparatedAnimations);
#else
	//CAnimationManager::Get_Instance()->Display_AllAnimations(pfbxModelScene);
#endif
	WriteStringToFile("</Animation>\n");

	::fclose(pFile);

	CSdkManager::Get_Instance()->DestroySdkObjects(pfbxSdkManager, bResult);

	return(0);
}

