#pragma once
#include "Texture.h"

enum class RENDER_TARGET_GROUP_TYPE : uint8
{
	SWAP_CHAIN, // BACK_BUFFER, FRONT_BUFFER
	SHADOW,		// SHADOW
	G_BUFFER,	// POSITION, NORMAL, COLOR
	LIGHTING,	// DIFFUSE LIGHT, SPECULAR LIGHT
	END,
};

enum
{
	RENDER_TARGET_SHADOW_GROUP_MEMBER_COUNT = 1,
	RENDER_TARGET_G_BUFFER_GROUP_MEMBER_COUNT = 3,
	RENDER_TARGET_LIGHTING_GROUP_MEMBER_COUNT = 2,
	RENDER_TARGET_GROUP_COUNT = static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::END)
};

struct RenderTarget
{
	shared_ptr<Texture> target;		// ���������� �׸� ���
	float clearColor[4];			// �� �����Ӹ��� �ʱⰪ���� �о��ִ� ����
};


// ���������� render targer�� ���ļ� �����ϴ� Ŭ����
class RenderTargetGroup
{
public:
	// ���� �ؽ�ó(dsTexture)�� ���� �޾���
	void Create(RENDER_TARGET_GROUP_TYPE groupType, vector<RenderTarget>& rtVec, shared_ptr<Texture> dsTexture);

	void OMSetRenderTargets(uint32 count, uint32 offset);
	void OMSetRenderTargets();

	void ClearRenderTargetView(uint32 index);
	void ClearRenderTargetView();

	shared_ptr<Texture> GetRTTexture(uint32 index) { return _rtVec[index].target; }
	shared_ptr<Texture> GetDSTexture() { return _dsTexture; }

	void WaitTargetToResource();
	void WaitResourceToTarget();

private:
	RENDER_TARGET_GROUP_TYPE		_groupType;		// � �뵵�� �׷����� ����� ���̳�
	vector<RenderTarget>			_rtVec;			// RenderTargetGroup�� �������� ��� rt�� ���ͷ� �ް� ����
	uint32							_rtCount;
	shared_ptr<Texture>				_dsTexture;
	ComPtr<ID3D12DescriptorHeap>	_rtvHeap;

private:
	uint32							_rtvHeapSize;
	D3D12_CPU_DESCRIPTOR_HANDLE		_rtvHeapBegin;
	D3D12_CPU_DESCRIPTOR_HANDLE		_dsvHeapBegin;

	// �ؽ�ó�� �������ϴ� �뵵�� Ȱ���ϰ� �ִµ�, �� �ؽ�ó�� GPU������ �׸��� �׸��µ��� �������� ����ϰ� �ִ�.
	// �׷��� �߰��� �׷����� ������ ���� ������, �����Ÿ����� ����
	// ����Ÿ�� �뵵�� Ȱ���ϸ鼭 ���ÿ� �Ϲ� �ؽ�ó �뵵��(���ҽ� �뵵��) ���ÿ� ���� �ʰԲ� ������ �ʿ䰡 ����
	// ����� �ּ� �ѹ� �����ش�
private:
	D3D12_RESOURCE_BARRIER			_targetToResource[8];	// Ÿ�ٿ��� ���ҽ��� �Ѿ�� ġ�� ������
	D3D12_RESOURCE_BARRIER			_resourceToTarget[8];	// ���ҽ����� rt���� �Ѿ�� ġ�� ������
};
