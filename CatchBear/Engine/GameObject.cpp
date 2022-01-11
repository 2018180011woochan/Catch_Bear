#include "pch.h"
#include "GameObject.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "MonoBehaviour.h"

GameObject::GameObject()
{
}

GameObject::~GameObject()
{
}

void GameObject::Init()
{
	// ��� ��ü���� Transform�� ��� �ֵ��� �������
	AddComponent(make_shared<Transform>());
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

shared_ptr<Transform> GameObject::GetTransform()
{
	uint8 index = static_cast<uint8>(COMPONENT_TYPE::TRANSFORM);
	return static_pointer_cast<Transform>(_components[index]);
}

void GameObject::AddComponent(shared_ptr<Component> component)
{
	// this�� ���ڸ� �Ѱ��� -> this�� ���� ������Ʈ�� ���������� ����Ʈ �����Ͱ� �ƴ�
	// make_shared<GameObject>(this) -> make_shared�� �ϴ� ���� ���ο� ����Ʈ �����͸� ������ָ鼭
	// this ��ü�� �����ϰ� ���۷����� 1�� ��ü�� �������
	// ����Ʈ �����ͷ� Ȱ���ϰ� �ִ� �������� enable_shared_from_this�� ��ӹ޾ƾ� ��
	// shared_from_this()�� ����ϸ� �ڱ� �ڽ��� shared_ptr�� ���� �ǳ��� �� ����
	// => �ڱ� �ڽſ� ���� ����Ʈ �����͸� �Ѱ��ְ� ���� �� enable_shared_from_this, shared_from_this() ���
	component->SetGameObject(shared_from_this());

	uint8 index = static_cast<uint8>(component->GetType());	// ���� � Ÿ���� �߰��ϰ� ������ �̾ƿ�
	if (index < FIXED_COMPONENT_COUNT)
	{
		_components[index] = component;	// �⺻���� ������ ������Ʈ
	}
	else
	{
		_scripts.push_back(dynamic_pointer_cast<MonoBehaviour>(component));	// ����ڰ� Ŀ������ ������Ʈ
	}
}
