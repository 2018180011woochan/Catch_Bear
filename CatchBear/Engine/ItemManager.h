#pragma once

// �Ϲ� ������ - 30�ʸ��� ��� �Ϲ� ������(5��) �ϳ��� ���� -> �� 6�� ������
// ����ũ ������ - 1�и��� ��� ����ũ ������(2��) �ϳ��� ���� -> �� 3�� ������
// ���� - 1�п� �ϳ� ����
class GameObject;
class MeshData;

class ItemManager
{
	DECLARE_SINGLE(ItemManager);

public:
	void Init();
	void Update();
	void LateUpdate();

public:
	void SetItemPosition();
	void CreateCommonItem();
	void CreateUniqueItem();
	void CreateTreasure();
	void Reset();

private:
	void Collision_ItemToPlayer();
	void Check_ItemPos();

public:
	list<shared_ptr<GameObject>>& GetCommonItemList() { return _commonItemList; }
	list<shared_ptr<GameObject>>& GetUniqueItemList() { return _uniqueItemList; }
	list<shared_ptr<GameObject>>& TreasureList() { return _treasureList; }


private:
	list<shared_ptr<GameObject>>		_commonItemList;	
	list<shared_ptr<GameObject>>		_uniqueItemList;
	list<shared_ptr<GameObject>>		_treasureList;

	shared_ptr<GameObject>	_player = make_shared<GameObject>();

	shared_ptr<MeshData> _commonItemMesh;
	shared_ptr<MeshData> _uniqueItemMesh;
	shared_ptr<MeshData> _treasureMesh;

	array<Vec3, 30>		_itemPosArray;
	int		_itemIndex = 0;
	int		_maxItemIndex = 29;

	// �ð��� �׽�Ʈ��
	float		_commonItemTimer = 0.f;
	float		_uniqueItemTimer = 0.f;
	float		_treasureTimer = 0.f;

	float		_commonItemTime = 5.f;
	float		_uniqueItemTime = 10.f;
	float		_treasureTime = 10.f;
};

