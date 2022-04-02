#pragma once
#include "MonoBehaviour.h"

// �� �÷��� Ÿ�� 3��
// �Ϲ� ������ - 30�ʸ��� ��� �Ϲ� ������(5��) �ϳ��� ���� -> �� 6�� ������
// ����ũ ������ - 1�и��� ��� ����ũ ������(2��) �ϳ��� ���� -> �� 3�� ������
// ���� - 1�п� �ϳ� ����

enum class ITEM_TYPE
{
	COMMON, UNIQUE, TRESURE, END
};

enum class ITEM_EFFECT
{
	/*�Ϲ� ������*/	 SPEED_UP, TELEPORT, SHIELD, SPEED_DOWN, BLIND,
	/*����ũ ������*/ DEBUFF_OFF, STUN,
	/*����	   	*/	 NONE,
	END
};

class Item : public MonoBehaviour
{
public:
	Item();
	virtual ~Item();

public:
	virtual void Update() override;

public:
	ITEM_TYPE GetItemType() { return _itemType; }
	ITEM_EFFECT GetItemEffect() { return _itemEffect; }

public:
	void SetItemType(ITEM_TYPE itemType) { _itemType = itemType;  }
	void SetItemEffect(ITEM_EFFECT itemEffect) { _itemEffect = itemEffect; }
		
private:
	ITEM_TYPE		_itemType = ITEM_TYPE::END;
	ITEM_EFFECT		_itemEffect = ITEM_EFFECT::END;

	Vec3	_itemPos = { 0.f, 0.f, 0.f };
};
