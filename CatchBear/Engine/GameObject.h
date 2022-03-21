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

	shared_ptr<Component> GetFixedComponent(COMPONENT_TYPE type);

	shared_ptr<Transform> GetTransform();
	shared_ptr<MeshRenderer> GetMeshRenderer();
	shared_ptr<Camera> GetCamera();
	shared_ptr<Light> GetLight();
	shared_ptr<ParticleSystem> GetParticleSystem();
	shared_ptr<Terrain> GetTerrain();
	shared_ptr<BaseCollider> GetCollider();
	shared_ptr<AnimationController> GetAnimationController();

	void AddComponent(shared_ptr<Component> component);

	void SetCheckFrustum(bool checkFrustum) { _checkFrustum = checkFrustum; }
	bool GetCheckFrustum() { return _checkFrustum; }

	void SetLayerIndex(uint8 layer) { _layerIndex = layer; }
	uint8 GetLayerIndex() { return _layerIndex; }

	void SetStatic(bool flag) { _static = flag; }
	bool IsStatic() { return _static; }

public:
	shared_ptr<GameObject> GetTestPlayer() { return _testPlayer; }
	void SetTestPlayer(shared_ptr<GameObject> testPlayer) { _testPlayer = testPlayer; }

private:
	// �Ϲ����� ������Ʈ(����)
	array<shared_ptr<Component>, FIXED_COMPONENT_COUNT> _components;	
	// ������ ��ũ��Ʈ�� �̿��� ������ִ� ������Ʈ
	vector<shared_ptr<MonoBehaviour>> _scripts;		

	bool	_checkFrustum = true;
	uint8	_layerIndex = 0;
	bool	_static = true;

public:
	shared_ptr<GameObject> _testPlayer;
	char* _pFrameName;
};

