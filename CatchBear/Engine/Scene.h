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

private:
	void SetTimer();

public:
	void AddGameObject(shared_ptr<GameObject> gameObject);
	void AddPlayers(uint64 _playerid, shared_ptr<GameObject> gameObject);
	void RemoveGameObject(shared_ptr<GameObject> gameObject);

	const vector<shared_ptr<GameObject>>& GetGameObjects() { return _gameObjects; }
	const shared_ptr<GameObject>& GetPlayer(uint64 playerid) { return _players[playerid]; }
	shared_ptr<GameObject> GetGameObject(wstring name);

private:
	// ���͸� ���̾� �ܰ�� �����Ѵ�
	// [0]: ��, [1]: ����
	// -> ���� �ϳ��� ������ִ� ���� �ƴ϶� ������ �迭 or ������ ���ͷ� ���� �����ؼ� ����
	vector<shared_ptr<GameObject>>		_gameObjects;
	vector<shared_ptr<class Camera>>	_cameras;
	vector<shared_ptr<class Light>>		_lights;
	map<uint64, shared_ptr<GameObject>> _players;

	float	_curTime = 0.0f;
};

