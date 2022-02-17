#pragma once

enum class OBJECT_TYPE : uint8
{
	NONE,
	GAMEOBJECT,	// PREFAB (게임 오브젝트도 완성품 자체를 리소스처럼 저장했다가 꺼내쓰는 형태로 사용, 리소스로 볼 수 있음)
	COMPONENT,
	MATERIAL,
	MESH,
	MESH_DATA,
	SHADER,
	TEXTURE,

	END
};

enum
{
	OBJECT_TYPE_COUNT = static_cast<uint8>(OBJECT_TYPE::END)
};

class Object
{
public:
	Object(OBJECT_TYPE type);
	virtual ~Object();

	OBJECT_TYPE GetType() { return _objectType; }

	void SetName(const wstring& name) { _name = name; }
	const wstring& GetName() { return _name; }

	uint32 GetID() { return _id; }

	// TODO: Instantiate - 실질적으로 그 오브젝트의 복사본(클론)을 생성해주는 역할

protected:
	friend class Resources;
	virtual void Load(const wstring& path) { }
	virtual void Save(const wstring& path) { }


protected:
	OBJECT_TYPE		_objectType = OBJECT_TYPE::NONE;
	wstring			_name;

protected:
	uint32		_id = 0;
};

