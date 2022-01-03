#pragma once

#include <fbxsdk.h>

class CSdkManager
{
public:
	CSdkManager() { }
	~CSdkManager() { }

public:
	void InitializeSdkObjects(FbxManager*& pManager);
	void DestroySdkObjects(FbxManager* pManager, bool pExitStatus);

public:
	bool LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename);

public:
	static CSdkManager* Get_Instance()
	{
		if (nullptr == m_pInstance)	m_pInstance = new CSdkManager;
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
	static CSdkManager* m_pInstance;
};

