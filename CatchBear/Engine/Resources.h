#pragma once

#include "GameObject.h"
#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "MeshData.h"

class Resources
{
	DECLARE_SINGLE(Resources);

public:
	void Init();

	// 리소스를 파일에서 로드하고, 로드된 것을 가져온다던가, 이미 메모리에 있던걸 임시로 추가하는 기능들을
	// 템플릿으로 만들어줌 - 우리가 사용하는게 어떤건지 모르기 때문에
	template<typename T>
	shared_ptr<T> Load(const wstring& key, const wstring& path);

	template<typename T>
	bool Add(const wstring& key, shared_ptr<T> object);

	template<typename T>
	shared_ptr<T> Get(const wstring& key);

	template<typename T>
	OBJECT_TYPE GetObjectType();

	shared_ptr<Mesh> LoadPointMesh();
	shared_ptr<Mesh> LoadRectangleMesh();
	shared_ptr<Mesh> LoadCubeMesh();
	shared_ptr<Mesh> LoadSphereMesh();
	shared_ptr<Mesh> LoadTerrainMesh(int32 sizeX = 15, int32 sizeZ = 15);

	shared_ptr<Texture> CreateTexture(const wstring& name, DXGI_FORMAT format, uint32 width, uint32 height,
		const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags,
		D3D12_RESOURCE_FLAGS resFlags = D3D12_RESOURCE_FLAG_NONE, Vec4 clearColor = Vec4());

	shared_ptr<Texture> CreateTextureFromResource(const wstring& name, ComPtr<ID3D12Resource> tex2D);

public:
	shared_ptr<class MeshData>		LoadFBX(const wstring& path);
	shared_ptr<class CharacterData>	LoadCharacter(const wstring& path);

private:
	void CreateDefaultShader();
	void CreateDefaultMaterial();
	void CreatePlayerTexture();

private:
	// map 형태로 리소스를 들고 있을 것임 - <이름을 지어줘서 구별할 수 있는 key, 실제 오브젝트>
	// 오브젝트 타입에 따라서 별도의 맵을 따로 만들어줌
	// _resources 자체는 배열이지만, KeyObjMap(맵)으로 이루어진 배열임
	using KeyObjMap = std::map<wstring/*key*/, shared_ptr<Object>>;
	array<KeyObjMap, OBJECT_TYPE_COUNT> _resources;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
inline shared_ptr<T> Resources::Load(const wstring& key, const wstring& path)
{
	// 오브젝트 타입을 추출해 
	OBJECT_TYPE objectType = GetObjectType<T>();
	// 그것에 따라 리소스 배열에서 몇번째인지 선택
	KeyObjMap& keyObjMap = _resources[static_cast<uint8>(objectType)];	// &: 참조값 - 원본 데이터를 건드리는중

	// key가 존재하면 그대로 return
	auto findIt = keyObjMap.find(key);
	if (findIt != keyObjMap.end())
		return static_pointer_cast<T>(findIt->second);

	// key가 없으면 실제 리소스를 만들고 로드해서 return
	shared_ptr<T> object = make_shared<T>();
	object->Load(path);
	keyObjMap[key] = object;

	return object;
}

template<typename T>
bool Resources::Add(const wstring& key, shared_ptr<T> object)
{
	OBJECT_TYPE objectType = GetObjectType<T>();
	KeyObjMap& keyObjMap = _resources[static_cast<uint8>(objectType)];

	auto findIt = keyObjMap.find(key);
	if (findIt != keyObjMap.end())
		return false;

	keyObjMap[key] = object;

	return true;
}

template<typename T>
shared_ptr<T> Resources::Get(const wstring& key)
{
	OBJECT_TYPE objectType = GetObjectType<T>();
	KeyObjMap& keyObjMap = _resources[static_cast<uint8>(objectType)];

	auto findIt = keyObjMap.find(key);
	if (findIt != keyObjMap.end())
		return static_pointer_cast<T>(findIt->second);

	return nullptr;
}

template<typename T>
inline OBJECT_TYPE Resources::GetObjectType()
{
	// 왜 Object 클래스의 GetType()을 활용 X ?
	// GetType()를 활용하기 위해서는 실제 객채를 만들어줘야 한다.
	// GetObjectType()는 객체를 만드는 개념이 아님.
	// 객체를 만들기 전에 클래스 타입만 보고 판별할 수 있어야 하기 때문에 GetType()으로는 처리할 수 없음

	if (std::is_same_v<T, GameObject>)	// 진짜 GameObject타입이면 반환
		return OBJECT_TYPE::GAMEOBJECT;
	else if (std::is_same_v<T, Material>)
		return OBJECT_TYPE::MATERIAL;
	else if (std::is_same_v<T, Mesh>)
		return OBJECT_TYPE::MESH;
	else if (std::is_same_v<T, Shader>)
		return OBJECT_TYPE::SHADER;
	else if (std::is_same_v<T, Texture>)
		return OBJECT_TYPE::TEXTURE;
	else if (std::is_same_v<T, MeshData>)
		return OBJECT_TYPE::MESH_DATA;
	else if (std::is_convertible_v<T, Component>)
		return OBJECT_TYPE::COMPONENT;
	else
		return OBJECT_TYPE::NONE;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
