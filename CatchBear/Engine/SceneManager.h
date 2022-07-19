#pragma once

// �̱���
// ���� ��𼭳� Ȱ���� �� �ֵ���
// ���ϼ� ����

class Scene;

enum
{
	MAX_LAYER = 32
};

class SceneManager
{

	DECLARE_SINGLE(SceneManager);

public:
	void Update();
	void Render();
	void LoadScene(SCENE_ID sceneID);

	void SetLayerName(uint8 index, const wstring& name);
	const wstring& IndexToLayerName(uint8 index) { return _layerNames[index]; }
	uint8 LayerNameToIndex(const wstring& name);

	void SetEnterLobby(int _id, bool _isEnter) { _isPlayersEnterLobby[_id] = _isEnter; }
	void SetPlayerReady(int _id, bool _isReady) { _isPlayersReady[_id] = _isReady; }

	bool GetEnterLobby(int _id) { return _isPlayersEnterLobby[_id]; }
	bool GetPlayerReady(int _id) { return _isPlayersReady[_id]; }
	PLAYER_TYPE GetPlayerType(int _id) { return _playerTypes[_id]; }

	void ReStart();	// ���� ������ �ٽ��ϱ� ��ư ������ ����Ǵ� �Լ�
public:
	shared_ptr<Scene> GetActiveScene() { return _activeScene; }
	bool IsEnd();


public:
	void changeScene(SCENE_ID eScene);
	SCENE_ID getSceneID() { return _curScene; }



public:
	void SetPlayerType(int index, PLAYER_TYPE playerType);

private:
	shared_ptr<Scene> LoadTestScene();
	shared_ptr<Scene> LoadLoginScene();
	shared_ptr<Scene> LoadLobbyScene();

	void CheckPlayerType(int index, wstring& key, bool isIcon = false);


private:
	void LoadMapFile(shared_ptr<Scene> scene);
	void AddMapObject(shared_ptr<Scene> scene, vector<shared_ptr<class GameObject>> gameObj, wstring name, Vec3 trans, Vec3 scale, Vec3 rotate);

	

private:
	shared_ptr<Scene> _activeScene;

	array<wstring, MAX_LAYER>	_layerNames;		// ���̾� �̸�, ex) 1���� �ش��ϴ� ���̾�� UI��
	map<wstring, uint8>			_layerIndex;		// ���̾��� �ε��� ��ȣ�� ã�� ����

	SCENE_ID	_curScene = SCENE_ID::SCENE_CNT;

	vector<PLAYER_TYPE>	_playerTypes;

	map<int, bool> _isPlayersEnterLobby;			// �÷��̾ �κ���� ���Դ°�
	map<int, bool> _isPlayersReady;					// �÷��̾ ���� �ߴ°�
};


