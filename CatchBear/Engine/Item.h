#pragma once
#include "MonoBehaviour.h"

enum class ITEM
{
	/*�Ϲ� ������*/	 SPEED_UP, TELEPORT, SHIELD, SPEED_DOWN, BLIND,
	/*����ũ ������*/ DEBUFF_OFF, STUN,
	END
};

// �� �÷��� Ÿ�� 3��
// �Ϲ� ������ - 30�ʸ��� ��� �Ϲ� ������(5��) �ϳ��� ���� -> �� 6�� ������
// ����ũ �������� 1�и��� ��� ����ũ ������(2��) �ϳ��� ���� -> �� 3�� ������

class Item : public MonoBehaviour
{
public:
	Item();
	Item(ITEM itemType);
	virtual ~Item();

public:
	virtual void Update() override;

private:
	void CreateItem();

public:
	ITEM GetItemType() { return _itemType; }


private:
	ITEM	_itemType = ITEM::END;

	float	_curTime = 0.f;
	float	_itemTime = 5.f;

	Vec3	_itemPos = { 0.f, 0.f, 0.f };
};
