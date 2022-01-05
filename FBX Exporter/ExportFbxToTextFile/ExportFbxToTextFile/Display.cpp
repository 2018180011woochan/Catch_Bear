#include "Display.h"
#include "SdkManager.h"
#if defined (FBXSDK_ENV_MAC)
// disable the “format not a string literal and no format arguments?warning since
// the PrintToFile calls made here are all valid calls and there is no secuity risk
#pragma GCC diagnostic ignored "-Wformat-security"
#endif

void CDisplay::DisplayMetaDataConnections(FbxObject* pfbxObject)
{
	int nbMetaData = pfbxObject->GetSrcObjectCount<FbxObjectMetaData>();
	if (nbMetaData > 0)
		DisplayString("    MetaData connections ");

	for (int i = 0; i < nbMetaData; i++)
	{
		FbxObjectMetaData* metaData = pfbxObject->GetSrcObject<FbxObjectMetaData>(i);
		DisplayString("        Name: ", (char*)metaData->GetName());
	}
}

FbxString gfbxString;

char* ReplaceBlank(const char* pstrValue, const char chReplace)
{
	int nLength = (int)strlen(pstrValue);
	if (nLength > 0)
	{
		gfbxString = pstrValue;
		gfbxString.ReplaceAll(' ', chReplace);
	}
	else
	{
		gfbxString = "Null";
	}
	return(gfbxString.Buffer());
}

void CDisplay::DisplayString(char* pstrValue)
{
	FbxString fbxString;

	fbxString = pstrValue;
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::DisplayString(const char* pstrValue)
{
	FbxString fbxString;

	fbxString = pstrValue;
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::DisplayString(char* pstrHeader, const char* pstrValue, char* pstrSuffix, int nTabIndents)
{
	for (int i = 0; i < nTabIndents; i++) WriteStringToFile("\t");

	FbxString fbxString;

	fbxString = pstrHeader;
	fbxString += pstrValue;
	fbxString += pstrSuffix;
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::DisplayIntString(char* pstrHeader, int iValue, const char* pstrValue, char* pstrSuffix, int nTabIndents)
{
	for (int i = 0; i < nTabIndents; i++) WriteStringToFile("\t");

	FbxString fbxString;

	fbxString = pstrHeader;
	fbxString += iValue;
	fbxString += " ";
	fbxString += pstrValue;
	fbxString += pstrSuffix;
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::DisplayIntString(char* pstrHeader, int iValue, const char* pstrValue1, const char* pstrValue2, char* pstrSuffix, int nTabIndents)
{
	for (int i = 0; i < nTabIndents; i++) WriteStringToFile("\t");

	FbxString fbxString;

	fbxString = pstrHeader;
	fbxString += iValue;
	fbxString += " ";
	fbxString += pstrValue1;
	fbxString += " ";
	fbxString += pstrValue2;
	fbxString += pstrSuffix;
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::DisplayString(char* pstrHeader, char* pstrValue, char* pstrSuffix, int nTabIndents)
{
	for (int i = 0; i < nTabIndents; i++) WriteStringToFile("\t");

	FbxString fbxString;

	fbxString = pstrHeader;
	fbxString += pstrValue;
	fbxString += pstrSuffix;
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::DisplayString(char* pstrHeader, char* pstrValue1, char* pstrValue2, char* pstrValue3, char* pstrSuffix, int nTabIndents)
{
	for (int i = 0; i < nTabIndents; i++) WriteStringToFile("\t");

	FbxString fbxString;

	fbxString = pstrHeader;
	fbxString += pstrValue1;
	fbxString += " ";
	fbxString += pstrValue2;
	fbxString += " ";
	fbxString += pstrValue3;
	fbxString += pstrSuffix;
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::DisplayIntString(char* pstrHeader, int iValue, char* pstrValue, char* pstrSuffix, int nTabIndents)
{
	for (int i = 0; i < nTabIndents; i++) WriteStringToFile("\t");

	FbxString fbxString;

	fbxString = pstrHeader;
	fbxString += iValue;
	fbxString += " ";
	fbxString += pstrValue;
	fbxString += pstrSuffix;
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::DisplayIntString(char* pstrHeader, int value1, int value2, const char* const pstrValue, char* pstrSuffix, int nTabIndents)
{
	for (int i = 0; i < nTabIndents; i++) WriteStringToFile("\t");

	FbxString fbxString;
	fbxString = pstrHeader;
	fbxString += value1;
	fbxString += " ";
	fbxString += value2;
	fbxString += " ";
	fbxString += pstrValue;
	fbxString += pstrSuffix;
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::DisplayBool(bool value)
{
	FbxString fbxString;

	fbxString += (value) ? 1 : 0;
	fbxString += " ";
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::DisplayBool(char* pstrHeader, bool value, char* pstrSuffix, int nTabIndents)
{
	for (int i = 0; i < nTabIndents; i++) WriteStringToFile("\t");

	FbxString fbxString;
	fbxString = pstrHeader;
	fbxString += (value) ? 1 : 0;
	fbxString += pstrSuffix;
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::DisplayInt(int value)
{
	FbxString fbxString;
	fbxString += value;
	fbxString += " ";
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::DisplayInt(char* pstrHeader, int value, char* pstrSuffix, int nTabIndents)
{
	for (int i = 0; i < nTabIndents; i++) WriteStringToFile("\t");

	FbxString fbxString;
	fbxString = pstrHeader;
	fbxString += value;
	fbxString += pstrSuffix;
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::DisplayInt(char* pstrHeader, int value1, int value2, char* pstrSuffix, int nTabIndents)
{
	for (int i = 0; i < nTabIndents; i++) WriteStringToFile("\t");

	FbxString fbxString;
	fbxString = pstrHeader;
	fbxString += value1;
	fbxString += " ";
	fbxString += value2;
	fbxString += " ";
	fbxString += pstrSuffix;
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::DisplayIntFloat(char* pstrHeader, int value1, int value2, float value3, char* pstrSuffix, int nTabIndents)
{
	for (int i = 0; i < nTabIndents; i++) WriteStringToFile("\t");

	FbxString fbxString;
	fbxString = pstrHeader;
	fbxString += value1;
	fbxString += " ";
	fbxString += value2;
	fbxString += " ";
	fbxString += value3;
	fbxString += " ";
	fbxString += pstrSuffix;
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::DisplayInt(char* pstrHeader, int value1, int value2, int value3, char* pstrSuffix, int nTabIndents)
{
	for (int i = 0; i < nTabIndents; i++) WriteStringToFile("\t");

	FbxString fbxString;
	fbxString = pstrHeader;
	fbxString += value1;
	fbxString += " ";
	fbxString += value2;
	fbxString += " ";
	fbxString += value3;
	fbxString += " ";
	fbxString += pstrSuffix;
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::DisplayIntString(char* pstrHeader, int value1, int value2, int value3, char* pString, char* pstrSuffix, int nTabIndents)
{
	for (int i = 0; i < nTabIndents; i++) WriteStringToFile("\t");

	FbxString fbxString;
	fbxString = pstrHeader;
	fbxString += value1;
	fbxString += " ";
	fbxString += value2;
	fbxString += " ";
	fbxString += value3;
	fbxString += " ";
	fbxString += pString;
	fbxString += pstrSuffix;
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::DisplayInt(char* pstrHeader, int value1, int value2, int value3, int value4, char* pstrSuffix, int nTabIndents)
{
	for (int i = 0; i < nTabIndents; i++) WriteStringToFile("\t");

	FbxString fbxString;
	fbxString = pstrHeader;
	fbxString += value1;
	fbxString += " ";
	fbxString += value2;
	fbxString += " ";
	fbxString += value3;
	fbxString += " ";
	fbxString += value4;
	fbxString += " ";
	fbxString += pstrSuffix;
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::DisplayInt(int value1, int value2, int value3, int value4)
{
	FbxString fbxString;
	fbxString += value1;
	fbxString += " ";
	fbxString += value2;
	fbxString += " ";
	fbxString += value3;
	fbxString += " ";
	fbxString += value4;
	fbxString += " ";
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::DisplayFloat(float value1, float value2, float value3, float value4)
{
	FbxString fbxString;
	fbxString += value1;
	fbxString += " ";
	fbxString += value2;
	fbxString += " ";
	fbxString += value3;
	fbxString += " ";
	fbxString += value4;
	fbxString += " ";
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::DisplayFloat(float value1, float value2)
{
	FbxString fbxString;
	fbxString += value1;
	fbxString += " ";
	fbxString += value2;
	fbxString += " ";
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::DisplayFloat(double value)
{
	FbxString fbxString;
	fbxString += (float)value;
	fbxString += " ";
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::DisplayFloat(double value1, double value2)
{
	FbxString fbxString;
	fbxString += (float)value1;
	fbxString += " ";
	fbxString += (float)value2;
	fbxString += " ";
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::DisplayFloat(double value1, double value2, double value3)
{
	FbxString fbxString;
	fbxString += (float)value1;
	fbxString += " ";
	fbxString += (float)value2;
	fbxString += " ";
	fbxString += (float)value3;
	fbxString += " ";
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::DisplayFloat(double value1, double value2, double value3, double value4)
{
	FbxString fbxString;
	fbxString += (float)value1;
	fbxString += " ";
	fbxString += (float)value2;
	fbxString += " ";
	fbxString += (float)value3;
	fbxString += " ";
	fbxString += (float)value4;
	fbxString += " ";
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::DisplayDouble(char* pstrHeader, double value, char* pstrSuffix, int nTabIndents)
{
	for (int i = 0; i < nTabIndents; i++) WriteStringToFile("\t");

	FbxString fbxString;
	fbxString = pstrHeader;
	fbxString += value;
	fbxString += pstrSuffix;
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::DisplayFloat(float value)
{
	FbxString fbxString;
	fbxString += value;
	fbxString += " ";
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::DisplayFloat(char* pstrHeader, float value, char* pstrSuffix, int nTabIndents)
{
	for (int i = 0; i < nTabIndents; i++) WriteStringToFile("\t");

	FbxString fbxString;
	fbxString = pstrHeader;
	fbxString += value;
	fbxString += pstrSuffix;
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::DisplayFloat(char* pstrHeader, float value1, float value2, char* pstrSuffix, int nTabIndents)
{
	for (int i = 0; i < nTabIndents; i++) WriteStringToFile("\t");

	FbxString fbxString;
	fbxString = pstrHeader;
	fbxString += value1;
	fbxString += " ";
	fbxString += value2;
	fbxString += pstrSuffix;
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::Display2DVector(FbxVector2 value)
{
	FbxString fbxString;
	fbxString += (float)value[0];
	fbxString += " ";
	fbxString += (float)value[1];
	fbxString += " ";
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::DisplayUV2DVector(FbxVector2 value)
{
	FbxString fbxString;
	fbxString += (float)value[0];
	fbxString += " ";
	fbxString += (float)(1.0 - value[1]);
	fbxString += " ";
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::Display2DVector(FbxDouble2 value)
{
	FbxString fbxString;
	fbxString += (float)value[0];
	fbxString += " ";
	fbxString += (float)value[1];
	fbxString += " ";
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::Display2DVector(char* pstrHeader, FbxDouble2 value, char* pstrSuffix, int nTabIndents)
{
	for (int i = 0; i < nTabIndents; i++) WriteStringToFile("\t");

	FbxString fbxString;
	fbxString = pstrHeader;
	fbxString += (float)value[0];
	fbxString += " ";
	fbxString += (float)value[1];
	fbxString += pstrSuffix;
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::Display3DVector(char* pstrHeader, FbxDouble3 value)
{
	FbxString fbxString;
	fbxString = pstrHeader;
	fbxString += (float)value[0];
	fbxString += " ";
	fbxString += (float)value[1];
	fbxString += " ";
	fbxString += (float)value[2];
	fbxString += " ";
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::Display2DVector(char* pstrHeader, FbxVector2 value, char* pstrSuffix, int nTabIndents)
{
	for (int i = 0; i < nTabIndents; i++) WriteStringToFile("\t");

	FbxString fbxString;
	FbxString lFloatValue1 = (float)value[0];
	FbxString lFloatValue2 = (float)value[1];

	lFloatValue1 = value[0] <= -HUGE_VAL ? "-INFINITY" : lFloatValue1.Buffer();
	lFloatValue1 = value[0] >= HUGE_VAL ? "INFINITY" : lFloatValue1.Buffer();
	lFloatValue2 = value[1] <= -HUGE_VAL ? "-INFINITY" : lFloatValue2.Buffer();
	lFloatValue2 = value[1] >= HUGE_VAL ? "INFINITY" : lFloatValue2.Buffer();

	fbxString = pstrHeader;
	fbxString += lFloatValue1;
	fbxString += " ";
	fbxString += lFloatValue2;
	fbxString += pstrSuffix;
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::Display3DVector(char* pstrHeader, FbxDouble3 value, char* pstrSuffix, int nTabIndents)
{
	for (int i = 0; i < nTabIndents; i++) WriteStringToFile("\t");

	FbxString fbxString;
	fbxString = pstrHeader;
	fbxString += (float)value[0];
	fbxString += " ";
	fbxString += (float)value[1];
	fbxString += " ";
	fbxString += (float)value[2];
	fbxString += pstrSuffix;
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::Display3DVector(FbxVector4 value)
{
	FbxString fbxString;
	fbxString += (float)value[0];
	fbxString += " ";
	fbxString += (float)value[1];
	fbxString += " ";
	fbxString += (float)value[2];
	fbxString += " ";
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::Display3DVector(char* pstrHeader, FbxVector4 value, char* pstrSuffix, int nTabIndents)
{
	for (int i = 0; i < nTabIndents; i++) WriteStringToFile("\t");

	FbxString fbxString;
	FbxString lFloatValue1 = (float)value[0];
	FbxString lFloatValue2 = (float)value[1];
	FbxString lFloatValue3 = (float)value[2];

	lFloatValue1 = value[0] <= -HUGE_VAL ? "-INFINITY" : lFloatValue1.Buffer();
	lFloatValue1 = value[0] >= HUGE_VAL ? "INFINITY" : lFloatValue1.Buffer();
	lFloatValue2 = value[1] <= -HUGE_VAL ? "-INFINITY" : lFloatValue2.Buffer();
	lFloatValue2 = value[1] >= HUGE_VAL ? "INFINITY" : lFloatValue2.Buffer();
	lFloatValue3 = value[2] <= -HUGE_VAL ? "-INFINITY" : lFloatValue3.Buffer();
	lFloatValue3 = value[2] >= HUGE_VAL ? "INFINITY" : lFloatValue3.Buffer();

	fbxString = pstrHeader;
	fbxString += lFloatValue1;
	fbxString += " ";
	fbxString += lFloatValue2;
	fbxString += " ";
	fbxString += lFloatValue3;
	fbxString += pstrSuffix;
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::Display4DVector(char* pstrHeader, FbxDouble4 value, char* pstrSuffix, int nTabIndents)
{
	for (int i = 0; i < nTabIndents; i++) WriteStringToFile("\t");

	FbxString fbxString;
	fbxString = pstrHeader;
	fbxString += (float)value[0];
	fbxString += " ";
	fbxString += (float)value[1];
	fbxString += " ";
	fbxString += (float)value[2];
	fbxString += " ";
	fbxString += (float)value[3];
	fbxString += pstrSuffix;
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::Display4DVector(FbxVector4 value)
{
	FbxString fbxString;
	fbxString += (float)value[0];
	fbxString += " ";
	fbxString += (float)value[1];
	fbxString += " ";
	fbxString += (float)value[2];
	fbxString += " ";
	fbxString += (float)value[3];
	fbxString += " ";
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::Display4DVector(char* pstrHeader, FbxVector4 value, char* pstrSuffix, int nTabIndents)
{
	for (int i = 0; i < nTabIndents; i++) WriteStringToFile("\t");

	FbxString fbxString;
	fbxString = pstrHeader;
	fbxString += (float)value[0];
	fbxString += ", ";
	fbxString += (float)value[1];
	fbxString += ", ";
	fbxString += (float)value[2];
	fbxString += ", ";
	fbxString += (float)value[3];
	fbxString += pstrSuffix;
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::DisplayMatrix(char* pstrHeader, FbxDouble4x4 value, char* pstrSuffix, int nTabIndents)
{
	for (int i = 0; i < nTabIndents; i++) WriteStringToFile("\t");

	DisplayString(pstrHeader);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) DisplayFloat((float)value[i][j]);
	}
	DisplayString(pstrSuffix);
}

void CDisplay::DisplayMatrix(FbxAMatrix value)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) DisplayFloat((float)value[i][j]);
	}
}

void CDisplay::DisplayMatrix(FbxMatrix value)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) DisplayFloat((float)value[i][j]);
	}
}

void CDisplay::DisplayMatrix(char* pstrHeader, FbxAMatrix value, char* pstrSuffix, int nTabIndents)
{
	for (int i = 0; i < nTabIndents; i++) WriteStringToFile("\t");

	WriteStringToFile(pstrHeader);
	DisplayMatrix(value);
	WriteStringToFile(pstrSuffix);
}

void CDisplay::WriteTransform(char* pstrHeader, FbxAMatrix value, FbxDouble3 S, FbxDouble3 R, FbxDouble3 T, char* pstrSuffix, int nTabIndents)
{
	for (int i = 0; i < nTabIndents; i++) WriteStringToFile("\t");

	WriteStringToFile(pstrHeader);
	DisplayMatrix(value);
	Display3DVector(S);
	Display3DVector(R);
	Display3DVector(T);
	WriteStringToFile(pstrSuffix);
}

void CDisplay::DisplayColor(char* pstrHeader, FbxPropertyT<FbxDouble3> value, char* pstrSuffix, int nTabIndents)
{
	for (int i = 0; i < nTabIndents; i++) WriteStringToFile("\t");

	FbxString fbxString;
	fbxString = pstrHeader;
	fbxString += (float)value.Get()[0];
	fbxString += " ";
	fbxString += (float)value.Get()[1];
	fbxString += " ";
	fbxString += (float)value.Get()[2];
	fbxString += " ";
	fbxString += pstrSuffix;
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::DisplayColor(FbxPropertyT<FbxDouble3> value)
{
	FbxString fbxString;
	fbxString += (float)value.Get()[0];
	fbxString += " ";
	fbxString += (float)value.Get()[1];
	fbxString += " ";
	fbxString += (float)value.Get()[2];
	fbxString += " ";
	WriteStringToFile(fbxString.Buffer());
}

void CDisplay::DisplayColor(FbxColor value)
{
	FbxString fbxString;
	fbxString += (float)value.mRed;
	fbxString += " ";
	fbxString += (float)value.mGreen;
	fbxString += " ";
	fbxString += (float)value.mBlue;
	fbxString += " ";
	WriteStringToFile(fbxString.Buffer());
}

void DisplayColor(char* pstrHeader, FbxColor value, char* pstrSuffix, int nTabIndents)
{
	for (int i = 0; i < nTabIndents; i++) WriteStringToFile("\t");

	FbxString fbxString;
	fbxString = pstrHeader;
	fbxString += (float)value.mRed;
	fbxString += " ";
	fbxString += (float)value.mGreen;
	fbxString += " ";
	fbxString += (float)value.mBlue;
	fbxString += " ";
	fbxString += pstrSuffix;
	WriteStringToFile(fbxString.Buffer());
}

