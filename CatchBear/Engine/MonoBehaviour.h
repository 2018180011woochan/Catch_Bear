#pragma once
#include "Component.h"

// �츮�� ���������� ���� ���� Ŀ���� ��ǰ�� ..
class GameObject;

class MonoBehaviour : public Component
{
public:
	MonoBehaviour();
	virtual ~MonoBehaviour();

private:
	// MonoBehaviour�� ��ӹ��� �ֵ��� FinalUpdate�� ������� ���ϵ��� ���Ƶ�
	// �������� ������ ����� �ϰ���� ���� ����ϴ� �Լ�
	virtual void FinalUpdate() sealed {}

//public:
//	shared_ptr<GameObject> GetPlayer() { if (_player != nullptr) return _player; }
//	void SetPlayer(shared_ptr<GameObject> player) { _player = player; }
//
//private:
//	shared_ptr<GameObject>	_player;
};

