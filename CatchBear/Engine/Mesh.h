#pragma once
#include "Object.h"

class Material;
class StructuredBuffer;

struct IndexBufferInfo
{
	ComPtr<ID3D12Resource>	buffer;
	D3D12_INDEX_BUFFER_VIEW	bufferView;
	DXGI_FORMAT				format;
	uint32					count;
};

struct BoneInfo
{
	wstring					boneName;
	int32					parentIdx;
	Matrix					matOffset;
};

// 정점으로 이루어진 물체
class Mesh : public Object
{
public:
	Mesh();
	virtual ~Mesh();

public:
	void Init(const vector<Vertex>& vertexBuffer, const vector<uint32>& indexBuffer);
	void Render(uint32 instanceCount = 1, uint32 idx = 0);
	void Render(shared_ptr<class InstancingBuffer>& buffer, uint32 idx = 0);

	//void Render(uint32 instanceCount = 1);
	//void Render(shared_ptr<class InstancingBuffer>& buffer);

public:
	shared_ptr<StructuredBuffer>	GetBoneOffsetBuffer() { return _offsetBuffer; }
		
private:
	// 수정해야 함
	void CreateVertexBuffer(const vector<Vertex>& buffer);
	void CreateIndexBuffer(const vector<uint32>& buffer);

public:
	void CreateStaticMeshFromFBX(const StaticMeshInfo* meshInfo);
	void CreateAnimationMeshFromFBX(const StaticMeshInfo* meshInfo, vector<shared_ptr<AnimationClipInfo>> animationMat, SkinningInfo skinningInfo);
	uint32	GetSubsetCount() { return static_cast<uint32>(_vecIndexInfo.size()); }
	
	void CreateBonesAndAnimations(vector<shared_ptr<AnimationClipInfo>> animationMat, SkinningInfo skinningInfo);

private:
	ComPtr<ID3D12Resource>		_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW	_vertexBufferView = {};
	uint32						_vertexCount = 0;

	vector<IndexBufferInfo>		_vecIndexInfo;

	//ComPtr<ID3D12Resource>		_indexBuffer;
	//D3D12_INDEX_BUFFER_VIEW		_indexBufferView;
	//uint32						_indexCount = 0;

	// Animation
	vector<AnimClipInfo>		_animClips;
	vector<BoneInfo>			_bones;

	shared_ptr<StructuredBuffer>	_offsetBuffer;
};

