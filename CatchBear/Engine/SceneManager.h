#pragma once

// 싱글톤
// 언제 어디서나 활용할 수 있도록
// 유일성 보장

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

	void ReStart();	// 게임 끝나고 다시하기 버튼 누르면 실행되는 함수
	void CreatePlayer(int PlayerID);

public:
	shared_ptr<Scene> GetActiveScene() { return _activeScene; }
	bool IsEnd();


public:
	void changeScene(SCENE_ID eScene);
	SCENE_ID getSceneID() { return _curScene; }


private:
	shared_ptr<Scene> LoadTestScene();
	shared_ptr<Scene> LoadLoginScene();


private:
	void LoadMapFile(shared_ptr<Scene> scene);
	void AddMapObject(shared_ptr<Scene> scene, vector<shared_ptr<class GameObject>> gameObj, wstring name, Vec3 trans, Vec3 scale, Vec3 rotate);

	

private:
	shared_ptr<Scene> _activeScene;

	array<wstring, MAX_LAYER>	_layerNames;		// 레이어 이름, ex) 1번에 해당하는 레이어는 UI다
	map<wstring, uint8>			_layerIndex;		// 레이어의 인덱스 번호를 찾기 위해

	SCENE_ID	_curScene = SCENE_ID::SCENE_CNT;
};


