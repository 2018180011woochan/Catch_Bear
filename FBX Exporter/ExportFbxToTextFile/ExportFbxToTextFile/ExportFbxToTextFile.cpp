// ExportFbxToTextFile.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include "SdkManager.h"

FILE* pFile = NULL;
char pszBuffer[256];

void WriteStringToFile(char* pszString) { ::fwrite(pszString, sizeof(char), strlen(pszString), pFile); }

int main(int argc, char** argv)
{
	// FbxSdk 생성/초기화
	FbxManager* pfbxSdkManager = NULL;

	CSdkManager::Get_Instance()->InitializeSdkObjects(pfbxSdkManager);

	// 읽어들일 fbx 파일 이름
	FbxString pfbxstrModelFilePath("EvilbearK.fbx");
	// 출력할 txt 파일 이름
	char* pszWriteFileName = "EvilbearK.txt";

	// FBX Scene 생성&로드
	FbxScene* pfbxModelScene = FbxScene::Create(pfbxSdkManager, pfbxstrModelFilePath.Buffer());
	bool bResult = CSdkManager::Get_Instance()->LoadScene(pfbxSdkManager, pfbxModelScene, pfbxstrModelFilePath.Buffer());

	return(0);
}

