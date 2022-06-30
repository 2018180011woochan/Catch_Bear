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

	//void RenderShadow();
	void RenderDeferred();
	void RenderLights();
	void RenderFinal();

	void RenderForward();

private:
	void PushLightData();

private:
	void UpdateLoginScene();
	void UpdateStageScene();

	void SetTimer();
	void SetFinalRanking();

	void CheckMouse();
	void CheckTagger();
	void StartGame();

public:
	void AddGameObject(shared_ptr<GameObject> gameObject);
	void AddPlayers(uint64 _playerid, shared_ptr<GameObject> gameObject);
	void AddTagMarks(uint64 _objid, shared_ptr<GameObject> gameObject);
	void AddVecPlayers(shared_ptr<GameObject> gameObject);
	void AddStaticObj(shared_ptr<GameObject> gameobject);
	void AddItemCoolTime(shared_ptr<GameObject> gameobject);
	void RemoveGameObject(shared_ptr<GameObject> gameObject);
	void RemoveItems();

	const vector<shared_ptr<GameObject>>& GetGameObjects() { return _gameObjects; }

	const shared_ptr<GameObject>& GetPlayer(uint64 playerid) { return _players[playerid]; }
	const shared_ptr<GameObject>& GetTagMarks (uint64 objid) { return _TagMarks[objid]; }
	const map<uint64, shared_ptr<GameObject>>& GetPlayer() { return _players; }
	const vector<shared_ptr<GameObject>>& GetStaticObj() { return _vecStaticObject; }
	const vector<shared_ptr<GameObject>>& GetVecPlayers() { return _vecPlayers; }
	const vector<shared_ptr<GameObject>>& GetItemCoolTimes() { return _vecItemCoolTimes; }
	shared_ptr<GameObject> GetGameObject(wstring name);

	vector<shared_ptr<class Light>>	GetLights() { return _lights; }

	int GetEnterPlayerNum() { return _players.size(); }

	void SetCurTime(float _time) { _curTime = _time; }
	void AddCurTime(float _time);

private:
	// 벡터를 레이어 단계로 구분한다
	// [0]: 벽, [1]: 몬스터
	// -> 벡터 하나를 만들어주는 것이 아니라 벡터의 배열 or 벡터의 벡터로 만들어서 구분해서 관리
	vector<shared_ptr<GameObject>>		_gameObjects;
	vector<shared_ptr<class Camera>>	_cameras;
	vector<shared_ptr<class Light>>		_lights;

	/////////// made by wc ///////////////////////////////////
	map<uint64, shared_ptr<GameObject>>		_players;
	vector<shared_ptr<GameObject>>			_vecStaticObject;
	vector<shared_ptr<GameObject>>			_vecPlayers;
	map<uint64, shared_ptr<GameObject>>		_TagMarks;
	//////////////////////////////////////////////////////////

	vector<shared_ptr<GameObject>>			_vecItemCoolTimes;

private:
	float	_curTime = -1.0f;

	POINT	_mousePos = { 0, 0 };
	RECT	_slotRt[3] = { {100, 620, 200, 710}, {220, 620, 320, 710}, {340, 620, 440, 710} };
	RECT	_endRt = { 470, 80, 680, 130 };
	RECT	_restartRt = { 770, 650, 950, 700 };

	float	_toStartTime = 0.0f;
public:
	bool	_isStart = false;
	int		_tagNum = -1;
	bool	_isFinish = false;
	bool	_isEnd = false;

	bool	_FinalPlayerEnter = false;
};
