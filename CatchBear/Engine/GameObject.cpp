#include "pch.h"
#include "GameObject.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "Light.h"
#include "MonoBehaviour.h"
#include "ParticleSystem.h"
#include "Terrain.h"
#include "BaseCollider.h"
#include "AnimationController.h"
#include "PlayerState.h"
#include "IdleState.h"
#include "MoveState.h"
#include "AttackState.h"
#include "DashState.h"

#include "SceneManager.h"
#include "Scene.h"

#include "SceneManager.h"
#include "Scene.h"

GameObject::GameObject() : Object(OBJECT_TYPE::GAMEOBJECT)
{
}

GameObject::~GameObject()
{
}


void GameObject::Awake()
{
	for (shared_ptr<Component>& component : _components)
	{
		if (component)
			component->Awake();
	}

	for (shared_ptr<MonoBehaviour>& script : _scripts)
	{
		script->Awake();
	}
}

void GameObject::Start()
{
	for (shared_ptr<Component>& component : _components)
	{
		if (component)
			component->Start();
	}

	for (shared_ptr<MonoBehaviour>& script : _scripts)
	{
		script->Start();
	}
}

void GameObject::Update()
{
	for (shared_ptr<Component>& component : _components)
	{
		if (component)
			component->Update();

	}

	for (shared_ptr<MonoBehaviour>& script : _scripts)
	{
		script->Update();
	}

	UpdateBoundingBox();
}

void GameObject::LateUpdate()
{
	for (shared_ptr<Component>& component : _components)
	{
		if (component)
			component->LateUpdate();
}

	for (shared_ptr<MonoBehaviour>& script : _scripts)
	{
		script->LateUpdate();
	}
}

void GameObject::FinalUpdate()
{
	for (shared_ptr<Component>& component : _components)
	{
		if (component)
			component->FinalUpdate();
	}

}

void GameObject::UpdateBoundingBox()
{
	auto& gameObjects = GET_SINGLE(SceneManager)->GetActiveScene()->GetGameObjects();

	for (auto& gameObject : gameObjects)
	{
		gameObject->_boundingBox.Center = gameObject->GetTransform()->GetLocalPosition();
		//gameObject->_boundingBox.Extents = gameObject->GetBoundingExtents();
		XMStoreFloat4(&gameObject->_boundingBox.Orientation, 
			XMQuaternionNormalize(XMLoadFloat4(&gameObject->_boundingBox.Orientation)));
	}
}

shared_ptr<Component> GameObject::GetFixedComponent(COMPONENT_TYPE type)
{
	uint8 index = static_cast<uint8>(type);
	assert(index < FIXED_COMPONENT_COUNT);
	return _components[index];
}

shared_ptr<MonoBehaviour>& GameObject::GetScript(int index)
{
	return _scripts[index];
}

float GameObject::GetX()
{
	return GetTransform()->GetWorldPosition().x;
}

float GameObject::GetY()
{
	return GetTransform()->GetWorldPosition().y;
}

float GameObject::GetZ()
{
	return GetTransform()->GetWorldPosition().z;
}

shared_ptr<Transform> GameObject::GetTransform()
{
	shared_ptr<Component> component = GetFixedComponent(COMPONENT_TYPE::TRANSFORM);
	return static_pointer_cast<Transform>(component);
}

shared_ptr<MeshRenderer> GameObject::GetMeshRenderer()
{
	shared_ptr<Component> component = GetFixedComponent(COMPONENT_TYPE::MESH_RENDERER);
	return static_pointer_cast<MeshRenderer>(component);
}

shared_ptr<Camera> GameObject::GetCamera()
{
	shared_ptr<Component> component = GetFixedComponent(COMPONENT_TYPE::CAMERA);
	return static_pointer_cast<Camera>(component);
}

shared_ptr<Light> GameObject::GetLight()
{
	shared_ptr<Component> component = GetFixedComponent(COMPONENT_TYPE::LIGHT);
	return static_pointer_cast<Light>(component);
}

shared_ptr<ParticleSystem> GameObject::GetParticleSystem()
{
	shared_ptr<Component> component = GetFixedComponent(COMPONENT_TYPE::PARTICLE_SYSTEM);
	return static_pointer_cast<ParticleSystem>(component);
}

shared_ptr<Terrain> GameObject::GetTerrain()
{
	shared_ptr<Component> component = GetFixedComponent(COMPONENT_TYPE::TERRAIN);
	return static_pointer_cast<Terrain>(component);
}

shared_ptr<BaseCollider> GameObject::GetCollider()
{
	shared_ptr<Component> component = GetFixedComponent(COMPONENT_TYPE::COLLIDER);
	return static_pointer_cast<BaseCollider>(component);
}

shared_ptr<AnimationController> GameObject::GetAnimationController()
{
	shared_ptr<Component> component = GetFixedComponent(COMPONENT_TYPE::ANIMATOR);
	return static_pointer_cast<AnimationController>(component);
}

void GameObject::AddComponent(shared_ptr<Component> component)
{
	// this?? ?????? ?????? -> this?? ???? ?????????? ?????????? ?????? ???????? ????
	// make_shared<GameObject>(this) -> make_shared?? ???? ???? ?????? ?????? ???????? ????????????
	// this ?????? ???????? ?????????? 1?? ?????? ????????
	// ?????? ???????? ???????? ???? ???????? enable_shared_from_this?? ?????????? ??
	// shared_from_this()?? ???????? ???? ?????? shared_ptr?? ???????? ?????? ?? ????
	// => ???? ?????? ???? ?????? ???????? ???????? ???? ?? enable_shared_from_this, shared_from_this() ????
	component->SetGameObject(shared_from_this());

	uint8 index = static_cast<uint8>(component->GetType());	// ???? ???? ?????? ???????? ?????? ??????
	if (index < FIXED_COMPONENT_COUNT)
	{
		_components[index] = component;	// ???????? ?????? ????????
	}
	else
	{
		_scripts.push_back(dynamic_pointer_cast<MonoBehaviour>(component));	// ???????? ???????? ????????
	}
}
