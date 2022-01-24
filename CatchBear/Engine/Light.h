#pragma once
#include "Component.h"

enum class LIGHT_TYPE : uint8
{
	DIRECTIONAL_LIGHT,
	POINT_LIGHT,
	SPOT_LIGHT,
};

// ������ �����ִ� �Ӽ�(����)
struct LightColor
{
	Vec4	diffuse;
	Vec4	ambient;
	Vec4	specular;
};

// ����� ���õ� ��� �������� ��� ����, ���̴��� �Ѱ��ٰ���
struct LightInfo
{
	LightColor	color;
	Vec4		position;		// �׻� ���Ǵ� ���� �ƴ�(directional light�� �ǹ̰� ����)
	Vec4		direction;		// ���� ��� ���� (point light������ ���x)
	int32		lightType;		// � �� Ÿ������ - ���̴��ʰ� �е��� �����ֱ� ���� int32
	float		range;			// ���� �ִ� ����
	float		angle;			// ���� ��� ����
	int32		padding;		// �ƹ� �ǹ� ���� ��, ���� �ִ� �������� ����� 16����Ʈ�� ����� ������ֱ� ���� ���
};

// ���̴��� �Ѱ��� �Ķ���� ����
struct LightParams
{
	uint32		lightCount;		// ���� ����
	Vec3		padding;
	LightInfo	lights[50];		// ���̴��� ��� ������ ��� ������ �־���
};

class Light : public Component
{
public:
	Light();
	virtual ~Light();
	
	virtual void FinalUpdate() override;

public:
	const LightInfo& GetLightInfo() { return _lightInfo; }

	void SetLightDirection(const Vec3& direction) { _lightInfo.direction = direction; }

	void SetDiffuse(const Vec3& diffuse) { _lightInfo.color.diffuse = diffuse; }
	void SetAmbient(const Vec3& ambient) { _lightInfo.color.ambient = ambient; }
	void SetSpecular(const Vec3& specular) { _lightInfo.color.specular = specular; }

	void SetLightType(LIGHT_TYPE type) { _lightInfo.lightType = static_cast<int32>(type); }
	void SetLightRange(float range) { _lightInfo.range = range; }
	void SetLightAngle(float angle) { _lightInfo.angle = angle; }

private:
	LightInfo _lightInfo = {};	// Light Component�� ���̴��� �����ؼ� �Ѱ��� ���� ��ü�� ��� ����
};

