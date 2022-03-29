#pragma once

// �Ϲ� ������ - 30�ʸ��� ��� �Ϲ� ������(5��) �ϳ��� ���� -> �� 6�� ������
// ����ũ ������ - 1�и��� ��� ����ũ ������(2��) �ϳ��� ���� -> �� 3�� ������
// ���� - 1�п� �ϳ� ����
class Item;
class GameObject;

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

class ItemManager
{
	DECLARE_SINGLE(ItemManager);

public:
	void Init();
	void Update();
	void LateUpdate();

public:
	void SetItemPosition();
	void CreateItem();

public:
	array<shared_ptr<GameObject>, 3>& GetItemArray() { return _itemArray; }
	list<shared_ptr<GameObject>>& GetCommonItemList() { return _commonItemList; }
	list<shared_ptr<GameObject>>& GetUniqueItemList() { return _uniqueItemList; }
	list<shared_ptr<GameObject>>& TreasureList() { return _treasureList; }

	ITEM_TYPE GetItemType() { return _itemType; }
	ITEM_EFFECT GetItemEffect() { return _itemEffect; }

private:
	array<shared_ptr<GameObject>, 3>	_itemArray;			// ������ ����Ʈ ����ִ� ���
	list<shared_ptr<GameObject>>		_commonItemList;	
	list<shared_ptr<GameObject>>		_uniqueItemList;
	list<shared_ptr<GameObject>>		_treasureList;

	array<Vec3, 10>	_commonPosArray;	// �Ϲ� ������ ��ǥ ����ִ� ���
	array<Vec3, 10>	_uniquePosArray;	// ����ũ ������ ��ǥ ����ִ� ���
	array<Vec3, 5>	_treasurePosArray;	// ���� ��ǥ ����ִ� ���

	ITEM_TYPE	_itemType = ITEM_TYPE::END;
	ITEM_EFFECT	_itemEffect = ITEM_EFFECT::END;

	int		index = 0;

	// �ð��� �׽�Ʈ��
	float		_commonItemTimer = 0.f;
	float		_uniqueItemTimer = 0.f;
	float		_treasureTimer = 0.f;

	float		_commonItemTime = 5.f;
	float		_uniqueItemTime = 7.f;
	float		_treasureTime = 10.f;
};

