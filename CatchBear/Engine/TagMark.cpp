#include "pch.h"
#include "TagMark.h"
#include "Timer.h"
#include "Transform.h"
#include "GameObject.h"
#include "Component.h"
#include "SceneManager.h"
#include "Scene.h"
#include "ServerSession.h"

TagMark::TagMark()
{
}

TagMark::~TagMark()
{
}

void TagMark::Update()
{
	Vec3 rotation = GetTransform()->GetLocalRotation();
	rotation.y += DELTA_TIME * _fRotationSpeed;
	GetTransform()->SetLocalRotation(rotation);


	//_target = GET_SINGLE(SceneManager)->GetActiveScene()->GetGameObject(L"Player1");

	//// ���� ������ �÷��̾� ã�Ƽ� Ÿ�� �ٲ��ִ� �ڵ�, ���߿� �ٸ����� ������ ����
	//for (int i = 0; i < g_EnterPlayerCnt; ++i)
	//{
	//	if (GET_SINGLE(SceneManager)->GetActiveScene()->GetPlayer(i)->GetIsTagger() == true)
	//		_target = GET_SINGLE(SceneManager)->GetActiveScene()->GetPlayer(i);
	//}

	//Vec3 transform = _target->GetTransform()->GetLocalPosition();
	//transform.y += 2.f;
	//GetTransform()->SetLocalPosition(transform);

}

void TagMark::LateUpdate()
{
	// �����Ǹ� �� ���ϰ� �ؾ� ��
	// -> ��-�� �浹 ��, �������� �˷���� ��
	// -> �� �� �����ҰԿ�
}

void TagMark::SetPosition(Vec3 pos)
{
	pos.y += 2.f;
	GetGameObject()->GetTransform()->SetLocalPosition(pos);
}

void TagMark::SetRotation()
{
	Vec3 rotation = GetTransform()->GetLocalRotation();
	rotation.y += DELTA_TIME * _fRotationSpeed;
	GetTransform()->SetLocalRotation(rotation);
}
