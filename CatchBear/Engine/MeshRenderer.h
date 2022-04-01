#pragma once
#include "Component.h"

class Mesh;
class Material;

// [32][32]
union InstanceID
{
	struct
	{
		uint32 meshID;
		uint32 materialID;
	};
	uint64 id;
};

// Mesh를 받아서 그린다
class MeshRenderer : public Component
{
public:
	MeshRenderer();
	virtual ~MeshRenderer();

public:
	shared_ptr<Material>	GetMaterial() { return _material; }
	shared_ptr<Mesh>		GetMesh() { return _mesh; }

public:
	void SetMesh(shared_ptr<Mesh> mesh) { _mesh = mesh; }
	void SetMaterial(shared_ptr<Material> material) { _material = material; }

public:
	void Render();
	void Render(shared_ptr<class InstancingBuffer>& buffer);
	void RenderShadow();

	uint64 GetInstanceID();

private:
	shared_ptr<Mesh>		_mesh;
	shared_ptr<Material>	_material;
};

