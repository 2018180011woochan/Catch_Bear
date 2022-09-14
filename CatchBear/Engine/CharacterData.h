#pragma once
#include "MeshData.h"

class Mesh;
class Material;
class GameObject;
class AnimationModelInfo;
class Transform;


class CharacterData : public MeshData
{
public:
	CharacterData();
	virtual ~CharacterData();

public:
	void LoadCharacterFromFile(const wstring& path);

private:
	shared_ptr<Transform> LoadFrameHierarchyFromFile(shared_ptr<Transform> parent, FILE* pFile);

private:
	void LoadSkinningInfoFromFile(FILE* pFile);
	void LoadMeshInfoFromFile(FILE* pFile);
	void LoadMaterialInfoFromFile(FILE* pFile);

	void LoadAnimationInfo(FILE* pFile);
	void CreateMaterials();
		
public:
	vector<shared_ptr<GameObject>>	Instantiate();

private:
	SkinningInfo							_skinningInfo;
	vector<shared_ptr<AnimationClipInfo>>	_animationClipInfo;
	shared_ptr<AnimationModelInfo>			_modelInfo;
	wstring _name;
};