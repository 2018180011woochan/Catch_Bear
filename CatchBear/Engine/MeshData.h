#pragma once
#include "Object.h"

class Mesh;
class Material;
class GameObject;

struct MeshRendererInfo
{
	shared_ptr<Mesh>		mesh;
	shared_ptr<Material>	materials;
};


class MeshData : public Object
{
public:
	MeshData();
	virtual ~MeshData();

public:
	// �޽� �ε� �Լ�
	void	LoadMeshFromFile(const wstring& path);
	

private:
	// ���������� ������ ���� �ε� (Transform, Mesh, Material)
	GameObject* LoadFrameHierarchyFromFile(GameObject* parent, FILE* pFile);


protected:
	// Mesh ���� �ε�: ControlPoints(Vertex), UV, Normal, Tangent, Polygons(SubIndices)
	void LoadMeshInfoFromFile(FILE* pFile);
	// Material ���� �ε�: Texture, Shading(Lambert or Phong)
	void LoadMaterialInfoFromFile(FILE* pFile);


protected:
	// Texture/Material ��ü�� �����ؼ� Resources�� �߰�
	void CreateTextures();
	void CreateMaterials();



public:
	// �׵��� �о�� ������ �������� �⺻���� ��ü �����ؼ� ����
	// ����Ƽ�� Prefab�� ����� ����
	vector<shared_ptr<GameObject>>	Instantiate();

protected:
	shared_ptr<Mesh>				_mesh;
	// vector: Material�� �������� �� ����. �ٵ� ���� �ϳ��Ӥ�
	vector<shared_ptr<Material>>	_materials;

	vector<MeshRendererInfo>		_meshRenders;

	// StaticMeshInfo: Mesh ���� (Vertex, Indices, Material)
	// -> ���߿� �� ������ �������� Mesh, Material �����ؼ� Resources�� �߰��ϰ� _meshRenders���� �־���
	StaticMeshInfo					_staticMeshInfo;
	wstring							_meshName;
};



