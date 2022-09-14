#include "pch.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "Material.h"
#include "Transform.h"
#include "InstancingBuffer.h"
#include "Resources.h"
#include "AnimationController.h"

MeshRenderer::MeshRenderer() : Component(COMPONENT_TYPE::MESH_RENDERER)
{
}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::Render()
{
	GetTransform()->PushData();
	if (GetAnimationController())
	{
		GetAnimationController()->PushData();
	}
	_material->PushGraphicsData();
	_mesh->Render(1, 0);
	//_mesh->Render();
}

void MeshRenderer::Render(shared_ptr<class InstancingBuffer>& buffer)
{
	// 한번에 그려줘야 하는, 인스턴싱이 들어간 애들은 params.fx의 TRANSFORM_PARAMS : register(b1)을 사용하는게 아니라
	// 우리가 따로 넣어준 deferred.fx의 VS_IN 부분에서 받아주게 된다.
	
	buffer->PushData();

	// Animation
	if (GetAnimationController())
	{
		GetAnimationController()->PushData();
	}

	_material->PushGraphicsData();
	_mesh->Render(buffer, 0);
	//_mesh->Render(buffer);
}

void MeshRenderer::RenderShadow()
{
	// shadow는 인스턴싱 고려하지 않고 일반적인(하나씩 그리는) 방법으로 그림
	
	// 기존에는 모든 물체들을 자신의 material을 이용해서 밀어넣고 있었는데
	// shadow를 그릴땐 미리 만들어 놓은 shadow material을 이용해서 그것에 해당하는 내용으로 push해서 render하고 있음

	GetTransform()->PushData();
	GET_SINGLE(Resources)->Get<Material>(L"Shadow")->PushGraphicsData();
	_mesh->Render();
}

uint64 MeshRenderer::GetInstanceID()
{
	// 각 mesh, material을 만들때마다 ID를 부여, 이 ID를 통해 새로운 ID를 조합해서 만들어줌

	if (_mesh == nullptr || _material == nullptr)
		return 0;

	// uint64 id = (_mesh->GetID() << 32) | _material->GetID();
	InstanceID instanceID{ _mesh->GetID(), _material->GetID() };
	return instanceID.id;
}
