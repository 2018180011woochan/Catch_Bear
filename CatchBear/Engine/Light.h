#pragma once
#include "Component.h"

enum class LIGHT_TYPE : uint8
{
	DIRECTIONAL_LIGHT,
	POINT_LIGHT,
	SPOT_LIGHT,
};

// 빛마다 갖고있는 속성(색상)
struct LightColor
{
	Vec4	diffuse;
	Vec4	ambient;
	Vec4	specular;
};

// 조명과 관련된 모든 정보들을 들고 있음, 쉐이더에 넘겨줄것임
struct LightInfo
{
	LightColor	color;
	Vec4		position;		// 항상 사용되는 것은 아님(directional light는 의미가 없음)
	Vec4		direction;		// 빛이 쏘는 방향 (point light에서는 사용x)
	int32		lightType;		// 어떤 빛 타입인지 - 쉐이더쪽과 패딩을 맞춰주기 위해 int32
	float		range;			// 빛의 최대 범위
	float		angle;			// 빛을 쏘는 각도
	int32		padding;		// 아무 의미 없는 값, 여기 있는 데이터의 사이즈를 16바이트의 배수로 만들어주기 위해 사용
};

// 쉐이더에 넘겨줄 파라미터 정보
struct LightParams
{
	uint32		lightCount;		// 조명 개수
	Vec3		padding;
	LightInfo	lights[50];		// 쉐이더에 모든 정보를 묶어서 통으로 넣어줌
	float		time;
};

class Light : public Component
{
public:
	Light();
	virtual ~Light();
	
	virtual void FinalUpdate() override;
	void Render();
	void RenderShadow();

public:
	LIGHT_TYPE GetLightType() { return static_cast<LIGHT_TYPE>(_lightInfo.lightType); }

	const LightInfo& GetLightInfo() { return _lightInfo; }

	void SetLightDirection(Vec3& direction);

	void SetDiffuse(const Vec3& diffuse) { _lightInfo.color.diffuse = diffuse; }
	void SetAmbient(const Vec3& ambient) { _lightInfo.color.ambient = ambient; }
	void SetSpecular(const Vec3& specular) { _lightInfo.color.specular = specular; }

	void SetLightType(LIGHT_TYPE type);
	void SetLightRange(float range) { _lightInfo.range = range; }
	void SetLightAngle(float angle) { _lightInfo.angle = angle; }

	void SetLightIndex(int8 index) { _lightIndex = index; }

private:
	LightInfo _lightInfo = {};	// Light Component는 쉐이더로 복사해서 넘겨줄 정보 자체를 들고 있음

	int8						_lightIndex = -1;	// 내가 몇번째 빛인지 임시 저장해서 세팅해주기 위해
	shared_ptr<class Mesh>		_volumeMesh;		// 내가 어떤 영역을 차지하는지
	shared_ptr<class Material>	_lightMaterial;		// 셰이더에 인자를 넘겨줄때

	shared_ptr<GameObject>		_shadowCamera;
};

