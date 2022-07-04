#include "pch.h"
#include "Light.h"
#include "Transform.h"
#include "Engine.h"
#include "Resources.h"'
#include "Camera.h"
#include "Transform.h"
#include "Texture.h"
#include "SceneManager.h"

Light::Light() : Component(COMPONENT_TYPE::LIGHT)
{
	_shadowCamera = make_shared<GameObject>();
	_shadowCamera->AddComponent(make_shared<Transform>());
	_shadowCamera->AddComponent(make_shared<Camera>());
	uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
	_shadowCamera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, true);	// UI는 안찍음
}

Light::~Light()
{
}

void Light::FinalUpdate()
{
	_lightInfo.position = GetTransform()->GetWorldPosition();

	// light가 자신의 좌표를 계산할때 쉐도우카메라의 좌표도 똑같은 위치에 똑같은 각도로 바라보도록(transform을 똑같이)
	_shadowCamera->GetTransform()->SetLocalPosition(GetTransform()->GetLocalPosition());
	_shadowCamera->GetTransform()->SetLocalRotation(GetTransform()->GetLocalRotation());
	_shadowCamera->GetTransform()->SetLocalScale(GetTransform()->GetLocalScale());

	_shadowCamera->FinalUpdate();
}

void Light::Render()
{
	assert(_lightIndex >= 0);

	// volumeMesh를 이용해서 지금 차지하고 있는 영역을 표시하고 있음, 그걸 PushData 해줌
	GetTransform()->PushData();

	if (static_cast<LIGHT_TYPE>(_lightInfo.lightType) == LIGHT_TYPE::DIRECTIONAL_LIGHT)
	{
		shared_ptr<Texture> shadowTex = GET_SINGLE(Resources)->Get<Texture>(L"ShadowTarget");
		_lightMaterial->SetTexture(2, shadowTex);

		Matrix matVP = _shadowCamera->GetCamera()->GetViewMatrix() * _shadowCamera->GetCamera()->GetProjectionMatrix();
		_lightMaterial->SetMatrix(0, matVP);
	}
	else
	{
		float scale = 2 * _lightInfo.range;
		GetTransform()->SetLocalScale(Vec3(scale, scale, scale));
	}

	_lightMaterial->SetInt(0, _lightIndex);
	_lightMaterial->PushGraphicsData();

	_volumeMesh->Render();
}

void Light::RenderShadow()
{
	// 쉐도우 카메라를 이용해서 빛이 바라보고 있는 똑같은 방향으로 카메라를 찍어줌
	// 그림자 영향을 받은 모든 물체들을 그려준다

	_shadowCamera->GetCamera()->SortShadowObject();	// 씬에 존재하는 모든 물체 중 그림자 영향 받는 애들 걸러냄
	_shadowCamera->GetCamera()->Render_Shadow();	// 그 애들을 실질적으로 그려줌
}

void Light::SetLightDirection(Vec3& direction)
{
	direction.Normalize();

	_lightInfo.direction = direction;

	GetTransform()->LookAt(direction);
}

void Light::SetLightType(LIGHT_TYPE type)
{
	_lightInfo.lightType = static_cast<int32>(type);

	switch (type)
	{
	case LIGHT_TYPE::DIRECTIONAL_LIGHT:
		_volumeMesh = GET_SINGLE(Resources)->Get<Mesh>(L"Rectangle");
		_lightMaterial = GET_SINGLE(Resources)->Get<Material>(L"DirLight");

		_shadowCamera->GetCamera()->SetScale(1.f);
		_shadowCamera->GetCamera()->SetFar(10000.f);
		_shadowCamera->GetCamera()->SetWidth(4096);
		_shadowCamera->GetCamera()->SetHeight(4096);
		break;
	case LIGHT_TYPE::POINT_LIGHT:
		_volumeMesh = GET_SINGLE(Resources)->Get<Mesh>(L"Sphere");
		_lightMaterial = GET_SINGLE(Resources)->Get<Material>(L"PointLight");
		break;
	case LIGHT_TYPE::SPOT_LIGHT:
		_volumeMesh = GET_SINGLE(Resources)->Get<Mesh>(L"Sphere");
		_lightMaterial = GET_SINGLE(Resources)->Get<Material>(L"PointLight");
		break;
	}

}
