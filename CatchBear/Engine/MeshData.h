#pragma once
#include "Object.h"

class Mesh;
class Material;
class GameObject;

struct MeshRendererInfo
{
	shared_ptr<Mesh>				mesh;
	vector<shared_ptr<Material>>	materials;
};

struct MaterialInfo
{
	Vec4		diffuse;
	Vec4		ambient;
	Vec4		specular;

	wstring		name;
	wstring		diffuseTexName;
};

struct StaticMeshInfo
{
	wstring						name;
	vector<Vertex>				vertices;
	vector<vector<uint32>>		indices;
	MaterialInfo				material;
};


class MeshData : public Object
{
public:
	MeshData();
	virtual ~MeshData();

public:
	void	LoadMeshFromFile(const wstring& path);
	
private:
	GameObject* LoadFrameHierarchyFromFile(GameObject* parent, FILE* pFile);

	void LoadMeshInfoFromFile(FILE* pFile);
	void LoadMaterialInfoFromFile(FILE* pFile);

private:
	shared_ptr<Mesh>				_mesh;
	vector<shared_ptr<Material>>	_materials;

	vector<MeshRendererInfo>		_meshRenders;

	StaticMeshInfo					_staticMeshInfo;
};

