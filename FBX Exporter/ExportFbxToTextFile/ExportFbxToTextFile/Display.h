#pragma once
#include <fbxsdk.h>

extern void WriteStringToFile(char* pszBuffer);

class CDisplay
{
public:
	CDisplay() { }
	~CDisplay() { }

public:
	char* ReplaceBlank(const char* pValue, const char chReplace);

	void DisplayMetaDataConnections(FbxObject* pfbxNode);
	void DisplayString(char* pstrValue);
	void DisplayString(const char* pstrValue);
	void DisplayString(char* pstrHeader, const char* pstrValue, char* pstrSuffix = "", int nTabIndents = 0);
	void DisplayString(char* pstrHeader, char* pstrValue1, char* pstrValue2, char* pstrValue3, char* pstrSuffix, int nTabIndents);
	void DisplayIntString(char* pstrHeader, int iValue, const char* pstrValue, char* pstrSuffix = "", int nTabIndents = 0);
	void DisplayString(char* pstrHeader, char* pstrValue, char* pstrSuffix = "", int nTabIndents = 0);
	void DisplayIntString(char* pstrHeader, int iValue, char* pstrValue, char* pstrSuffix, int nTabIndents = 0);
	void DisplayIntString(char* pstrHeader, int iValue, const char* pstrValue1, const char* pstrValue2, char* pstrSuffix, int nTabIndents);
	void DisplayIntString(char* pstrHeader, int value1, int value2, const char* const pstrValue, char* pstrSuffix, int nTabIndents = 0);
	void DisplayIntString(char* pstrHeader, int value1, int value2, int value3, char* pString, char* pstrSuffix, int nTabIndents = 0);
	void DisplayBool(bool value);
	void DisplayBool(char* pstrHeader, bool value, char* pstrSuffix = "", int nTabIndents = 0);
	void DisplayInt(int value);
	void DisplayInt(char* pstrHeader, int value, char* pstrSuffix = "", int nTabIndents = 0);
	void DisplayInt(char* pstrHeader, int value1, int value2, char* pstrSuffix = "", int nTabIndents = 0);
	void DisplayIntFloat(char* pstrHeader, int value1, int value2, float value3, char* pstrSuffix = "", int nTabIndents = 0);
	void DisplayInt(char* pstrHeader, int value1, int value2, int value3, char* pstrSuffix = "", int nTabIndents = 0);
	void DisplayInt(char* pstrHeader, int value1, int value2, int value3, int value4, char* pstrSuffix = "", int nTabIndents = 0);
	void DisplayInt(int value1, int value2, int value3, int value4);
	void DisplayFloat(float value1, float value2, float value3, float value4);
	void DisplayFloat(double value);
	void DisplayFloat(double value1, double value2);
	void DisplayFloat(double value1, double value2, double value3);
	void DisplayFloat(double value1, double value2, double value3, double value4);
	void DisplayFloat(float value);
	void DisplayFloat(float value1, float value2);
	void DisplayDouble(char* pstrHeader, double value, char* pstrSuffix = "", int nTabIndents = 0);
	void DisplayFloat(char* pstrHeader, float value, char* pstrSuffix = "", int nTabIndents = 0);
	void DisplayFloat(char* pstrHeader, float value1, float value2, char* pstrSuffix = "", int nTabIndents = 0);
	void Display2DVector(FbxVector2 value);
	void DisplayUV2DVector(FbxVector2 value);
	void Display2DVector(FbxDouble2 value);
	void Display2DVector(char* pstrHeader, FbxVector2 value, char* pstrSuffix = "", int nTabIndents = 0);
	void Display3DVector(char* pstrHeader, FbxDouble3 value);
	void Display3DVector(FbxVector4 value);
	void Display3DVector(char* pstrHeader, FbxVector4 value, char* pstrSuffix = "", int nTabIndents = 0);
	void DisplayColor(FbxPropertyT<FbxDouble3> value);
	void DisplayColor(char* pstrHeader, FbxPropertyT<FbxDouble3> value, char* pstrSuffix, int nTabIndents = 0);
	void DisplayColor(FbxColor value);
	void DisplayColor(char* pstrHeader, FbxColor value, char* pstrSuffix = "", int nTabIndents = 0);
	void Display4DVector(FbxVector4 value);
	void Display4DVector(char* pstrHeader, FbxVector4 value, char* pstrSuffix = "", int nTabIndents = 0);
	void Display2DVector(char* pstrHeader, FbxDouble2 value, char* pstrSuffix, int nTabIndents);
	void Display3DVector(char* pstrHeader, FbxDouble3 value, char* pstrSuffix, int nTabIndents);
	void Display4DVector(char* pstrHeader, FbxDouble4 value, char* pstrSuffix, int nTabIndents);
	void DisplayMatrix(char* pstrHeader, FbxDouble4x4 value, char* pstrSuffix, int nTabIndents);
	void DisplayMatrix(FbxAMatrix value);
	void DisplayMatrix(FbxMatrix value);
	void DisplayMatrix(char* pstrHeader, FbxAMatrix value, char* pstrSuffix = "", int nTabIndents = 0);
	void WriteTransform(char* pstrHeader, FbxAMatrix value, FbxDouble3 S, FbxDouble3 R, FbxDouble3 T, char* pstrSuffix, int nTabIndents);

public:
	static CDisplay* Get_Instance()
	{
		if (nullptr == m_pInstance)	m_pInstance = new CDisplay;
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
	static CDisplay* m_pInstance;
	FbxString gfbxString;
};

