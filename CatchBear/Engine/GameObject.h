#pragma once
#include "Component.h"
#include "Object.h"

class Transform;
class MeshRenderer;
class Camera;
class Light;
class MonoBehaviour;
class ParticleSystem;
class Terrain;
class BaseCollider;
class AnimationController;

// Component �������� �����ϴ� ����ü
// enable_shared_from_this: this�����͸� ����Ʈ ������ó�� ����� �� ���
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

	void UpdateBoundingBox();

public:
	shared_ptr<Component> GetFixedComponent(COMPONENT_TYPE type);
	vector<shared_ptr<MonoBehaviour>>& GetScripts() { return _scripts; }
	shared_ptr<MonoBehaviour>& GetScript(int index);

public:
	// ServerPacketHandler���� ��ġ ������ �����ϱ� ���� �Լ�
	float GetX();
	float GetY();
	float GetZ();

public:
	shared_ptr<Transform> GetTransform();
	shared_ptr<MeshRenderer> GetMeshRenderer();
	shared_ptr<Camera> GetCamera();
	shared_ptr<Light> GetLight();
	shared_ptr<ParticleSystem> GetParticleSystem();
	shared_ptr<Terrain> GetTerrain();
	shared_ptr<BaseCollider> GetCollider();
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
	// �Ϲ����� ������Ʈ(����)
	array<shared_ptr<Component>, FIXED_COMPONENT_COUNT> _components;	
	// ������ ��ũ��Ʈ�� �̿��� ������ִ� ������Ʈ
	vector<shared_ptr<MonoBehaviour>> _scripts;		

	bool	_checkFrustum = true;
	uint8	_layerIndex = 0;
	bool	_static = false;
	uint64  _playerID = 0;

public:
	char* _pFrameName;
	BoundingOrientedBox _boundingBox;
	XMFLOAT3	_boundingExtents;
	class PlayerState* _state;
};