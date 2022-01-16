#pragma once

#include "GameObject.h"
#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

class Resources
{
	DECLARE_SINGLE(Resources);

public:
	// ���ҽ��� ���Ͽ��� �ε��ϰ�, �ε�� ���� �����´ٴ���, �̹� �޸𸮿� �ִ��� �ӽ÷� �߰��ϴ� ��ɵ���
	// ���ø����� ������� - �츮�� ����ϴ°� ����� �𸣱� ������
	template<typename T>
	shared_ptr<T> Load(const wstring& key, const wstring& path);

	template<typename T>
	bool Add(const wstring& key, shared_ptr<T> object);

	template<typename T>
	shared_ptr<T> Get(const wstring& key);

	template<typename T>
	OBJECT_TYPE GetObjectType();

	shared_ptr<Mesh> LoadCubeMesh();
	shared_ptr<Mesh> LoadSphereMesh();

private:
	// map ���·� ���ҽ��� ��� ���� ���� - <�̸��� �����༭ ������ �� �ִ� key, ���� ������Ʈ>
	// ������Ʈ Ÿ�Կ� ���� ������ ���� ���� �������
	// _resources ��ü�� �迭������, KeyObjMap(��)���� �̷���� �迭��
	using KeyObjMap = std::map<wstring/*key*/, shared_ptr<Object>>;
	array<KeyObjMap, OBJECT_TYPE_COUNT> _resources;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
inline shared_ptr<T> Resources::Load(const wstring& key, const wstring& path)
{
	// ������Ʈ Ÿ���� ������ �װͿ� ���� ���ҽ� �迭���� ���°���� ����
	OBJECT_TYPE objectType = GetObjectType<T>();
	KeyObjMap& keyObjMap = _resources[static_cast<uint8>(objectType)];	// ������ - ���� �����͸� �ǵ帮����

	auto findIt = keyObjMap.find(key);
	if (findIt != keyObjMap.end())
		return static_pointer_cast<T>(findIt->second);

	shared_ptr<T> object = make_shared<T>();
	object->Load(0);
	KeyObjMap[key] = object;

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
	// �� Object Ŭ������ GetType()�� Ȱ�� X ?
	// GetType()�� Ȱ���ϱ� ���ؼ��� ���� ��ä�� �������� �Ѵ�.
	// GetObjectType()�� ��ü�� ����� ������ �ƴ�.
	// ��ü�� ����� ���� Ŭ���� Ÿ�Ը� ���� �Ǻ��� �� �־�� �ϱ� ������ GetType()���δ� ó���� �� ����

	if (std::is_same_v<T, GameObject>)
		return OBJECT_TYPE::GAMEOBJECT;
	else if (std::is_same_v<T, Material>)
		return OBJECT_TYPE::MATERIAL;
	else if (std::is_same_v<T, Mesh>)
		return OBJECT_TYPE::MESH;
	else if (std::is_same_v<T, Shader>)
		return OBJECT_TYPE::SHADER;
	else if (std::is_same_v<T, Texture>)
		return OBJECT_TYPE::TEXTURE;
	else if (std::is_convertible_v<T, Component>)
		return OBJECT_TYPE::COMPONENT;
	else
		return OBJECT_TYPE::NONE;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
