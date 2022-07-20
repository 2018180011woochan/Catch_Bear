#pragma once
#include "Object.h"

// Depth Stencil Buffer와 Swap Chain에서 관리하고 있었던 
// 실질적으로 후면/전면 버퍼에 해당하는 부분과 더불어 
// 직접적으로 이미지 파일을 로드하는 텍스처 뿐만 아니라 
// 코드로 수동으로 만드는 텍스처도 만들 수 있게 작업을 해준다.
// -> Deferred에서 pos, normal, color를 받아주기 위한 새로운 텍스처(버퍼)를 만들기 위해
// -> 리소스를 통한 로드보다는 실시간으로 메모리에서 만들어주는 것이 더 편리

class Texture : public Object
{
public:
	Texture();
	virtual ~Texture();

	virtual void Load(const wstring& path) override;

public:
	void Create(DXGI_FORMAT format, uint32 width, uint32 height,
		const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags,
		D3D12_RESOURCE_FLAGS resFlags, Vec4 clearColor = Vec4());

	// SwapChain
	void CreateFromResource(ComPtr<ID3D12Resource> tex2D);

public:
	ComPtr<ID3D12Resource> GetTex2D() { return _tex2D; }
	ComPtr<ID3D12DescriptorHeap> GetSRV() { return _srvHeap; }
	ComPtr<ID3D12DescriptorHeap> GetRTV() { return _rtvHeap; }
	ComPtr<ID3D12DescriptorHeap> GetDSV() { return _dsvHeap; }
	ComPtr<ID3D12DescriptorHeap> GetUAV() { return _uavHeap; }

	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVHandle() { return _srvHeapBegin; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetUAVHandle() { return _uavHeapBegin; }

	float GetWidth() { return static_cast<float>(_desc.Width); }
	float GetHeight() { return static_cast<float>(_desc.Height); }

private:
	// 이미지
	ScratchImage			 		_image;
	D3D12_RESOURCE_DESC				_desc;
	// 텍스처 리소스
	ComPtr<ID3D12Resource>			_tex2D;

	// 텍스처는 한 번만 로드한 후 변경없이 사용하기 때문에 하나의 뷰만 생성
	ComPtr<ID3D12DescriptorHeap>	_srvHeap;	// 뷰

	ComPtr<ID3D12DescriptorHeap>	_rtvHeap;
	ComPtr<ID3D12DescriptorHeap>	_dsvHeap;
	ComPtr<ID3D12DescriptorHeap>	_uavHeap;

private:
	D3D12_CPU_DESCRIPTOR_HANDLE		_srvHeapBegin = {};
	D3D12_CPU_DESCRIPTOR_HANDLE		_uavHeapBegin = {};
};

