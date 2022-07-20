#pragma once

enum class CONSTANT_BUFFER_TYPE : uint8
{
	GLOBAL,			// 전역 데이터를 가지고 있을 것임(Light 같은)
	TRANSFORM,
	MATERIAL,
	BONE_OFFSET,
	ANIMATED_BONE_TRANS,
	END
};

enum
{
	CONSTANT_BUFFER_COUNT = static_cast<uint8>(CONSTANT_BUFFER_TYPE::END)
};

class ConstantBuffer
{
public:
	ConstantBuffer();
	~ConstantBuffer();

	// 어떤 레지스터에 매핑이 되어 있는지
	void Init(CBV_REGISTER reg, uint32 size, uint32 count);

	void Clear();
	void PushGraphicsData(void* buffer, uint32 size);
	void SetGraphicsGlobalData(void* buffer, uint32 size);
	void PushComputeData(void* buffer, uint32 size);

	D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress(uint32 index);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(uint32 index);

private:
	void CreateBuffer();
	void CreateView();

private:
	ComPtr<ID3D12Resource>	_cbvBuffer;
	BYTE*					_mappedBuffer = nullptr;	// 데이터를 memcpy하기 위한 버퍼
	uint32					_elementSize = 0;			// 버퍼를 몇개 만들지 - 각 요소의 크기
	uint32					_elementCount = 0;			// 버퍼를 몇개 만들지 - 각 요소의 개수

	ComPtr<ID3D12DescriptorHeap>	_cbvHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE		_cpuHandleBegin = {};		// 시작 핸들 주소
	uint32							_handleIncrementSize = 0;	// 몇 칸을 더해야 다음 핸들로 넘어갈지(각 핸들간 간격)

	uint32					_currentIndex = 0;			// 버퍼를 어디까지 사용했는지

	CBV_REGISTER			_reg = {};
};

