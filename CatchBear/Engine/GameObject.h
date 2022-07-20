#pragma once
#include "Component.h"
#include "Object.h"

class Transform;
class MeshRenderer;
class Camera;
class Light;
class MonoBehaviour;
class Terrain;
class AnimationController;

// Component 무리들을 관리하는 조립체
// enable_shared_from_this: this포인터를 스마트 포인터처럼 사용할 때 사용
class GameObject : public Object, public enable_shared_from_this<GameObject>
{
public:
	GameObject();
	virtual ~GameObject();

	void Awake();
	void Start();
	void Update();
	void LateUpdate();
	void FinalUpdate();

public:
	shared_ptr<Component> GetFixedComponent(COMPONENT_TYPE type);
	vector<shared_ptr<MonoBehaviour>>& GetScripts() { return _scripts; }
	shared_ptr<MonoBehaviour>& GetScript(int index);

public:
	// ServerPacketHandler에서 위치 정보를 제어하기 위한 함수
	float GetX();
	float GetY();
	float GetZ();

public:
	shared_ptr<Transform> GetTransform();
	shared_ptr<MeshRenderer> GetMeshRenderer();
	shared_ptr<Camera> GetCamera();
	shared_ptr<Light> GetLight();
	shared_ptr<Terrain> GetTerrain();
	shared_ptr<AnimationController> GetAnimationController();

public:
	void AddComponent(shared_ptr<Component> component);

	void SetCheckFrustum(bool checkFrustum) { _checkFrustum = checkFrustum; }
	bool GetCheckFrustum() { return _checkFrustum; }

	void SetLayerIndex(uint8 layer) { _layerIndex = layer; }
	uint8 GetLayerIndex() { return _layerIndex; }

	void SetStatic(bool flag) { _static = flag; }
	bool IsStatic() { return _static; }
	void SetPlayerID(uint64 _id) { _playerID = _id; }
	uint64 GetPlayerID() { return _playerID; }
private:
	// 일반적인 컴포넌트(고정)
	array<shared_ptr<Component>, FIXED_COMPONENT_COUNT> _components;	
	// 유저가 스크립트를 이용해 만들어주는 컴포넌트
	vector<shared_ptr<MonoBehaviour>> _scripts;		

	bool	_checkFrustum = true;
	uint8	_layerIndex = 0;
	bool	_static = true;
	uint64  _playerID = 0;

public:
	char* _pFrameName;
};

