#include "pch.h"
#include "FontDevice.h"

FontDevice::FontDevice(UINT nFrame, ComPtr<ID3D12Device> pd3dDevice, ComPtr<ID3D12CommandQueue> pd3dCommandQueue)
{
	_fWidth = 0.f;
	_fWidth = 0.f;
	_vWrappedRenderTargets.resize(nFrame);
	_vd2dRenderTargets.resize(nFrame);
	_vTextBlocks.resize(1);

	//Initialize(pd3dDevice, pd3dCommandQueue);
}

void FontDevice::Initialize(ComPtr<ID3D12Device> pd3dDevice, ComPtr<ID3D12CommandQueue> pd3dCommandQueue)
{
	
}
