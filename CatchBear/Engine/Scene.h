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
	void CheckMouse();
	void CheckTagger();
	void StartGame();

public:
	void AddGameObject(shared_ptr<GameObject> gameObject);
	void AddPlayers(uint64 _playerid, shared_ptr<GameObject> gameObject);
	void AddVecPlayers(shared_ptr<GameObject> gameObject);
	void AddStaticObj(shared_ptr<GameObject> gameobject);
	void RemoveGameObject(shared_ptr<GameObject> gameObject);

	const vector<shared_ptr<GameObject>>& GetGameObjects() { return _gameObjects; }

	const shared_ptr<GameObject>& GetPlayer(uint64 playerid) { return _players[playerid]; }
	const map<uint64, shared_ptr<GameObject>>& GetPlayer() { return _players; }
	const vector<shared_ptr<GameObject>>& GetStaticObj() { return _vecStaticObject; }
	const vector<shared_ptr<GameObject>>& GetVecPlayers() { return _vecPlayers; }
	shared_ptr<GameObject> GetGameObject(wstring name);

	vector<shared_ptr<class Light>>	GetLights() { return _lights; }

	int GetEnterPlayerNum() { return _players.size(); }

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
	vector<shared_ptr<GameObject>>			_vecPlayers;
	//////////////////////////////////////////////////////////

private:
	float	_curTime = 0.0f;

	POINT	_mousePos = { 0, 0 };
	RECT	_slotRt[3] = { {100, 620, 200, 710}, {220, 620, 320, 710}, {340, 620, 440, 710} };
	float	_toStartTime = 0.0f;
	bool	_isStart = false;
};
