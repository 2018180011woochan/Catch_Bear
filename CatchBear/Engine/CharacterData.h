#pragma once
//#include "Object.h"
#include "MeshData.h"

class Mesh;
class Material;
class GameObject;

// �ϴ� ���⿡ ��Ű�� ���� �� �־�ΰ� ���߿� �����Ű��
struct SkinningInfo
{
	int				nSkinnings;

	vector<wstring>	boneNames;
	vector<Matrix>	boneOffsets;
	vector<XMINT4>	boneIndices;
	vector<Vec4>	boneWeights;
};

struct CharacterBoneInfo
{
	wstring		boneName;
	wstring		parentName;

	int32		nFrame;
	int32		nTexture;

	Vec3		position;
	Vec3		scale;
	Vec3		rotation;

	Matrix		toParent;
};

struct AnimationFrameInfo
{
	wstring					boneName;
	int32					parentIndex;
	Matrix					matOffset;
};

struct AnimationClipInfo
{
	wstring								name;
	float								startTime;
	float								endTime;
	float								length;
	int32								framePerSec;
	//FbxTime::EMode	mode;
	vector<vector<AnimationFrameInfo>>	keyFrames;
};


class CharacterData : public MeshData
{
public:
	CharacterData();
	virtual ~CharacterData();

public:
	void	LoadCharacterFromFile(const wstring& path);

private:
	// �� �Լ��� ����Լ��� ��� �θ��鼭 �������� �ϼ��ؾ� ��
	void LoadFrameHierarchyFromFile(shared_ptr<CharacterBoneInfo> parent, FILE* pFile, bool bFirst);

private:
	void LoadSkinningInfoFromFile(FILE* pFile);
	void LoadMeshInfoFromFile(FILE* pFile);
	void LoadMaterialInfoFromFile(FILE* pFile);

	void LoadAnimationInfo(FILE* pFile);

private:
	vector<shared_ptr<CharacterBoneInfo>>	characterInfo;
	SkinningInfo							skinningInfo;
	vector<wstring>							animationFrameName;
	vector<AnimationClipInfo>				animationClipInfo;
};

