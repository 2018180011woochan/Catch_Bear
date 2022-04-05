#pragma once

class GameObject;

class Scene
{
public:
	void Awake();
	void Start();
	void Update();
	void LateUpdate();
	void FinalUpdate();

	shared_ptr<class Camera> GetMainCamera();

	void Render();

	void ClearRTV();

	void RenderShadow();
	void RenderDeferred();
	void RenderLights();
	void RenderFinal();

	void RenderForward();

private:
	void PushLightData();

public:
	void AddGameObject(shared_ptr<GameObject> gameObject);
	void AddPlayers(uint64 _playerid, shared_ptr<GameObject> gameObject);
	void AddStaticObj(shared_ptr<GameObject> gameobject);
	void RemoveGameObject(shared_ptr<GameObject> gameObject);

	const vector<shared_ptr<GameObject>>& GetGameObjects() { return _gameObjects; }

	const shared_ptr<GameObject>& GetPlayer(uint64 playerid) { return _players[playerid]; }
	const vector<shared_ptr<GameObject>>& GetStaticObj() { return _vecStaticObject; }

private:
	// ���͸� ���̾� �ܰ�� �����Ѵ�
	// [0]: ��, [1]: ����
	// -> ���� �ϳ��� ������ִ� ���� �ƴ϶� ������ �迭 or ������ ���ͷ� ���� �����ؼ� ����
	vector<shared_ptr<GameObject>>		_gameObjects;
	vector<shared_ptr<class Camera>>	_cameras;
	vector<shared_ptr<class Light>>		_lights;

	/////////// made by wc ///////////////////////////////////
	map<uint64, shared_ptr<GameObject>>		_players;
	vector<shared_ptr<GameObject>>			_vecStaticObject;
	//////////////////////////////////////////////////////////
};

