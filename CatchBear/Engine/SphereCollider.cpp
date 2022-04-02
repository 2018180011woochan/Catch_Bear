#include "pch.h"
#include "SphereCollider.h"
#include "GameObject.h"
#include "Transform.h"

SphereCollider::SphereCollider() : BaseCollider(ColliderType::Sphere)
{
}

SphereCollider::~SphereCollider()
{
}

void SphereCollider::FinalUpdate()
{
	// bondingSphere ������ ��������

	// �浹 �׽�Ʈ�� �Ҷ� ��� �浹�� ������ �߿��ϴ�
	// ������ ����� �佺���̽����� ǥ���� ���� �ְ�, ���� �����̽��� �ش� ��ü�� ���� �����̽����� ǥ���� ���� �ִ�.
	// �����̽��� ����������� ����� �� ������ �� �� �ִ�.
	// ���⼭�� World�� �������� �����ϰ� �ִ�.

	// ��������� �ִ� center�� radius�� ���� �����̱� �ѵ�
	// ��׵��� FinalUpdate���� ���������� bondingSphere�� üũ�� ����
	// SphereCollider�� �پ��ִ� ��ü�� ���� �������� �״�� �������� �ִ�.
	// Scale�� �� �������� x, y, z�߿��� ���� ū �ɷ� �����ؼ� �־��ְ� �ִ�.

	// ������ �ش� ��ü�� ������ ��ü�� center�� ������ ���̰�
	// radius(�ݰ� ��ü)�� radius�� x, y, z�� ���ؼ� ���� �������� �� ������ ������ ǥ�����ְ� �ִ�.

	//_boundingSphere.Center = GetGameObject()->GetTransform()->GetWorldPosition();
	_boundingSphere.Center = GetGameObject()->GetTransform()->GetLocalPosition();

	Vec3 scale = GetGameObject()->GetTransform()->GetLocalScale();
	_boundingSphere.Radius = _radius * max(max(scale.x, scale.y), scale.z);
}

bool SphereCollider::Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance)
{
	// ������ �������� ������ �־���
	// �浹������ �Ÿ��� distance�� �� ����
	return _boundingSphere.Intersects(rayOrigin, rayDir, OUT distance);
}