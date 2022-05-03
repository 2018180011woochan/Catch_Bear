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
	void LoadScene(wstring sceneName);

	void SetLayerName(uint8 index, const wstring& name);
	const wstring& IndexToLayerName(uint8 index) { return _layerNames[index]; }
	uint8 LayerNameToIndex(const wstring& name);

public:
	shared_ptr<Scene> GetActiveScene() { return _activeScene; }
	bool IsEnd();

private:
	shared_ptr<Scene> LoadTestScene();


private:
	void LoadMapFile(shared_ptr<Scene> scene);
	void AddMapObject(shared_ptr<Scene> scene, vector<shared_ptr<class GameObject>> gameObj, wstring name, Vec3 trans, Vec3 scale, Vec3 rotate);



private:
	shared_ptr<Scene> _activeScene;

	array<wstring, MAX_LAYER>	_layerNames;		// ���̾� �̸�, ex) 1���� �ش��ϴ� ���̾�� UI��
	map<wstring, uint8>			_layerIndex;		// ���̾��� �ε��� ��ȣ�� ã�� ����
};


