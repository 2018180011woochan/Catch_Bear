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
	// 메쉬 로드 함수
	void	LoadMeshFromFile(const wstring& path);
	

private:
	// 계층구조의 프레임 정보 로드 (Transform, Mesh, Material)
	GameObject* LoadFrameHierarchyFromFile(GameObject* parent, FILE* pFile);


protected:
	// Mesh 정보 로드: ControlPoints(Vertex), UV, Normal, Tangent, Polygons(SubIndices)
	void LoadMeshInfoFromFile(FILE* pFile);
	// Material 정보 로드: Texture, Shading(Lambert or Phong)
	void LoadMaterialInfoFromFile(FILE* pFile);


protected:
	// Texture/Material 객체를 생성해서 Resources에 추가
	void CreateTextures();
	void CreateMaterials();


public:
	// 그동안 읽어온 정보를 바탕으로 기본적인 객체 생성해서 리턴
	// 유니티의 Prefab과 비슷한 개념
	vector<shared_ptr<GameObject>>	Instantiate();

protected:
	shared_ptr<Mesh>				_mesh;
	// vector: Material은 여러개일 수 있음. 근데 거의 하나임ㅋ
	vector<shared_ptr<Material>>	_materials;

	vector<MeshRendererInfo>		_meshRenders;

public:
	// StaticMeshInfo: Mesh 정보 (Vertex, Indices, Material)
	// -> 나중에 이 정보를 바탕으로 Mesh, Material 생성해서 Resources에 추가하고 _meshRenders에도 넣어줌
	StaticMeshInfo					_staticMeshInfo;
	wstring							_meshName;
};



