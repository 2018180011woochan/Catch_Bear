#pragma once

class ConstantBuffer
{
public:
	ConstantBuffer();
	~ConstantBuffer();

	void Init(uint32 size, uint32 count);

	void Clear();
	D3D12_CPU_DESCRIPTOR_HANDLE PushData(int32 rootParamIndex, void* buffer, uint32 size);

	D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress(uint32 index);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(uint32 index);

private:
	void CreateBuffer();
	void CreateView();

private:
	ComPtr<ID3D12Resource>	_cbvBuffer;
	BYTE*					_mappedBuffer = nullptr;	// �����͸� memcpy�ϱ� ���� ����
	uint32					_elementSize = 0;			// ���۸� � ������ - �� ����� ũ��
	uint32					_elementCount = 0;			// ���۸� � ������ - �� ����� ����

	ComPtr<ID3D12DescriptorHeap>	_cbvHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE		_cpuHandleBegin = {};		// ���� �ڵ� �ּ�
	uint32							_handleIncrementSize = 0;	// �� ĭ�� ���ؾ� ���� �ڵ�� �Ѿ��(�� �ڵ鰣 ����)

	uint32					_currentIndex = 0;			// ���۸� ������ ����ߴ���

};
