#pragma once

// std::byte ������� ����
#define _HAS_STD_BYTE 0
#define _CRT_SECURE_NO_WARNINGS


// ���� include
#include <windows.h>
#include <tchar.h>
#include <memory>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <map>
#include <iostream>
using namespace std;

#include <filesystem>
namespace fs = std::filesystem;

#include "d3dx12.h"
#include "SimpleMath.h"
#include <d3d12.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace Microsoft::WRL;

#include <DirectXTex/DirectXTex.h>
#include <DirectXTex/DirectXTex.inl>

// ���� lib
#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "dxguid")
#pragma comment(lib, "d3dcompiler")

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex\\DirectXTex_debug.lib")
#else
#pragma comment(lib, "DirectXTex\\DirectXTex.lib")
#endif

// ���� typedef
using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;
using Vec2 = DirectX::SimpleMath::Vector2;
using Vec3 = DirectX::SimpleMath::Vector3;
using Vec4 = DirectX::SimpleMath::Vector4;
using Matrix = DirectX::SimpleMath::Matrix;



enum class CBV_REGISTER : uint8
{
	b0,
	b1,
	b2,
	b3,
	b4,

	END
};

enum class SRV_REGISTER : uint8
{
	t0 = static_cast<uint8>(CBV_REGISTER::END),
	t1,
	t2,
	t3,
	t4,
	t5,
	t6,
	t7,
	t8,
	t9,

	END
};

enum class UAV_REGISTER : uint8
{
	u0 = static_cast<uint8>(SRV_REGISTER::END),
	u1,
	u2,
	u3,
	u4,

	END
};

enum
{
	SWAP_CHAIN_BUFFER_COUNT = 2,

	CBV_REGISTER_COUNT = CBV_REGISTER::END,
	SRV_REGISTER_COUNT = static_cast<uint8>(SRV_REGISTER::END) - CBV_REGISTER_COUNT,
	CBV_SRV_REGISTER_COUNT = CBV_REGISTER_COUNT + SRV_REGISTER_COUNT,

	UAV_REGISTER_COUNT = static_cast<uint8>(UAV_REGISTER::END) - CBV_SRV_REGISTER_COUNT,
	TOTAL_REGISTER_COUNT = CBV_SRV_REGISTER_COUNT + UAV_REGISTER_COUNT
};

struct WindowInfo
{
	HWND	hwnd;		// ��� ������
	int32	width;		// �ʺ�
	int32	height;		// ����
	bool	windowed;	// â��� or ��üȭ��
};

struct Vertex
{
	Vertex() {}

	Vertex(Vec3 p, Vec2 u, Vec3 n, Vec3 t)
		: pos(p), uv(u), normal(n), tangent(t)
	{
	}
	
	Vec3 pos;
	Vec2 uv;
	Vec3 normal;
	Vec3 tangent;

	// Skinning
	Vec4 weights;
	Vec4 indices;
};

struct MaterialInfo
{
	Vec4		diffuse;
	Vec4		ambient;
	Vec4		specular;

	wstring		name;
	wstring		diffuseTexName;
};

struct StaticMeshInfo
{
	wstring						name;

	vector<Vertex>				vertices;
	vector<vector<uint32>>		indices;
	MaterialInfo				material;

	//vector<>					boneWeights;
};

struct AnimationFrameInfo
{
	int					key;
	double				time;
	vector<Matrix>		matOffset;
};

struct AnimationClipInfo
{
	wstring								name;
	float								startTime;
	float								endTime;
	float								length;
	int32								framePerSec;
	// �ش� �ִϸ��̼��� Ű ������ ����
	int nkeyFrames;
	//FbxTime::EMode	mode;
	vector<AnimationFrameInfo>	keyFrames;
	float position = 0.0f;	// ���� ���� ��ġ
};

// �ϴ� ���⿡ ��Ű�� ���� �� �־�ΰ� ���߿� �����Ű��
struct SkinningInfo
{
	int				nSkinnings;

	vector<wstring>	boneNames;
	vector<Matrix>	boneOffsets;
	vector<XMINT4>	boneIndices;
	vector<Vec4>	boneWeights;
};

struct CharacterBoneInfo
{
	wstring		boneName;
	wstring		parentName;
	int32		parentIdx = 0;

	int32		nFrame;
	int32		nTexture;

	Vec3		position;
	Vec3		scale;
	Vec3		rotation;

	Matrix		toParent;
	Matrix		world;
};

/// ////////////////////////////////

struct KeyFrameInfo
{
	double	time;
	int32	frame;

	// To Parent��ķ� �Ǿ� �ִٸ� To Root�� �ٲ�� ��
	// keyFrames To Root ��� -> SRT �� ����
	// : �������� 2���� ���¸� ���� �� SRT ���°� �� ����
	Vec3	scale;
	Vec4	rotation;
	Vec3	translate;

	Matrix	matTransform;
};

struct AnimClipInfo
{
	wstring			animName;
	// �� ������ ����
	int32			frameCount;
	// ���� �ð�
	double			duration;
	vector<vector<KeyFrameInfo>>	keyFrames;
	// ���� ��ġ
	float			position;
};

#define DECLARE_SINGLE(type)		\
private:							\
	type() {}						\
	~type() {}						\
public:								\
	static type* GetInstance()		\
	{								\
		static type instance;		\
		return &instance;			\
	}								\

// static: ���� ������ �ƴ� ������ ����(���� �޸� ����)�� �ö�

#define GET_SINGLE(type)	type::GetInstance()

#define DEVICE				GEngine->GetDevice()->GetDevice()
#define GRAPHICS_CMD_LIST	GEngine->GetGraphicsCmdQueue()->GetGraphicsCmdList()
#define RESOURCE_CMD_LIST	GEngine->GetGraphicsCmdQueue()->GetResourceCmdList()
#define COMPUTE_CMD_LIST	GEngine->GetComputeCmdQueue()->GetComputeCmdList()

#define GRAPHICS_ROOT_SIGNATURE		GEngine->GetRootSignature()->GetGraphicsRootSignature()
#define COMPUTE_ROOT_SIGNATURE		GEngine->GetRootSignature()->GetComputeRootSignature()

#define INPUT				GET_SINGLE(Input)
#define DELTA_TIME			GET_SINGLE(Timer)->GetDeltaTime()

#define CONST_BUFFER(type)	GEngine->GetConstantBuffer(type)

// 3���� ������ ����
namespace Vector3
{
	inline XMFLOAT3 XMVectorToFloat3(XMVECTOR& xmvVector)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, xmvVector);
		return(xmf3Result);
	}
	inline XMFLOAT3 ScalarProduct(XMFLOAT3& xmf3Vector, float fScalar, bool bNormalize = true)
	{
		XMFLOAT3 xmf3Result;
		if (bNormalize)
			XMStoreFloat3(&xmf3Result, XMVector3Normalize(XMLoadFloat3(&xmf3Vector)) * fScalar);
		else
			XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector) * fScalar);
		return(xmf3Result);
	}
	inline XMFLOAT3 Add(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) +
			XMLoadFloat3(&xmf3Vector2)); return(xmf3Result);
	}
	inline XMFLOAT3 Add(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2, float fScalar)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) + (XMLoadFloat3(&xmf3Vector2) * fScalar));
		return(xmf3Result);
	}
	inline XMFLOAT3 Subtract(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) - XMLoadFloat3(&xmf3Vector2));
		return(xmf3Result);
	}
	inline float DotProduct(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3Dot(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
		return(xmf3Result.x);
	}
	inline XMFLOAT3 CrossProduct(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2, bool bNormalize = true)
	{
		XMFLOAT3 xmf3Result;
		if (bNormalize)
			XMStoreFloat3(&xmf3Result,
				XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&xmf3Vector1),
					XMLoadFloat3(&xmf3Vector2))));
		else
			XMStoreFloat3(&xmf3Result, XMVector3Cross(XMLoadFloat3(&xmf3Vector1),
				XMLoadFloat3(&xmf3Vector2)));
		return(xmf3Result);
	}
	inline XMFLOAT3 Normalize(XMFLOAT3& xmf3Vector)
	{
		XMFLOAT3 m_xmf3Normal;
		XMStoreFloat3(&m_xmf3Normal, XMVector3Normalize(XMLoadFloat3(&xmf3Vector)));
		return(m_xmf3Normal);
	}
	inline float Length(XMFLOAT3& xmf3Vector)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3Length(XMLoadFloat3(&xmf3Vector)));
		return(xmf3Result.x);
	}
	inline XMFLOAT3 TransformNormal(XMFLOAT3& xmf3Vector, XMMATRIX& xmmtxTransform)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3TransformNormal(XMLoadFloat3(&xmf3Vector), xmmtxTransform));
		return(xmf3Result);
	}
	inline XMFLOAT3 TransformCoord2(XMFLOAT3& xmf3Vector, XMMATRIX& xmmtxTransform)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3TransformCoord(XMLoadFloat3(&xmf3Vector), xmmtxTransform));
		return(xmf3Result);
	}
	inline XMFLOAT3 TransformCoord1(XMFLOAT3& xmf3Vector, XMFLOAT4X4& xmmtx4x4Matrix)
	{
		return(TransformCoord2((XMFLOAT3&)xmf3Vector, (XMMATRIX&)XMLoadFloat4x4(&xmmtx4x4Matrix)));
	}
}

namespace Matrix4x4
{
	inline XMFLOAT4X4 Identity()
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixIdentity());
		return(xmmtx4x4Result);
	}
}

#define ANIMATION_TYPE_ONCE			0
#define ANIMATION_TYPE_LOOP			1
#define ANIMATION_TYPE_PINGPONG		2

struct TransformParams
{
	Matrix	matWorld;
	Matrix	matView;
	Matrix	matProjection;
	Matrix	matWV;
	Matrix	matWVP;
	Matrix	matViewInv;
};

struct BoneOffsetParams
{
	Matrix matOfset[72];
};

struct AnimatedBoneParams
{
	Matrix matBoneTrans[72];
};

extern unique_ptr<class Engine> GEngine;

// Utils
wstring s2ws(const string& s);
string ws2s(const wstring& s);

#pragma region ReadFile
UINT ReadUnsignedIntegerFromFile(FILE* pInFile);

int ReadIntegerFromFile(FILE* pInFile);

float ReadFloatFromFile(FILE* pInFile);

int ReadStringFromFile(FILE* pInFile, char* pstrToken);

int ReadStringFromFileForCharac(FILE* pInFile, char* pstrToken);

#pragma endregion

inline XMFLOAT4X4 Interpolate(XMFLOAT4X4& xmf4x4Matrix1, XMFLOAT4X4& xmf4x4Matrix2, float t)
{
	XMFLOAT4X4 xmf4x4Result;
	XMVECTOR S0, R0, T0, S1, R1, T1;
	XMMatrixDecompose(&S0, &R0, &T0, XMLoadFloat4x4(&xmf4x4Matrix1));
	XMMatrixDecompose(&S1, &R1, &T1, XMLoadFloat4x4(&xmf4x4Matrix2));
	XMVECTOR S = XMVectorLerp(S0, S1, t);
	XMVECTOR T = XMVectorLerp(T0, T1, t);
	XMVECTOR R = XMQuaternionSlerp(R0, R1, t);
	XMStoreFloat4x4(&xmf4x4Result, XMMatrixAffineTransformation(S, XMVectorZero(), R, T));
	return(xmf4x4Result);
}

inline XMFLOAT4X4 Add(XMFLOAT4X4& xmmtx4x4Matrix1, XMFLOAT4X4& xmmtx4x4Matrix2)
{
	XMFLOAT4X4 xmf4x4Result;
	XMStoreFloat4x4(&xmf4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) + XMLoadFloat4x4(&xmmtx4x4Matrix2));
	return(xmf4x4Result);
}

inline XMFLOAT4X4 Scale(XMFLOAT4X4& xmf4x4Matrix, float fScale)
{
	XMFLOAT4X4 xmf4x4Result;
	XMStoreFloat4x4(&xmf4x4Result, XMLoadFloat4x4(&xmf4x4Matrix) * fScale);
	/*
			XMVECTOR S, R, T;
			XMMatrixDecompose(&S, &R, &T, XMLoadFloat4x4(&xmf4x4Matrix));
			S = XMVectorScale(S, fScale);
			T = XMVectorScale(T, fScale);
			R = XMVectorScale(R, fScale);
			//R = XMQuaternionMultiply(R, XMVectorSet(0, 0, 0, fScale));
			XMStoreFloat4x4(&xmf4x4Result, XMMatrixAffineTransformation(S, XMVectorZero(), R, T));
	*/
	return(xmf4x4Result);
}

inline XMFLOAT4X4 Multiply(XMFLOAT4X4& xmmtx4x4Matrix1, XMFLOAT4X4& xmmtx4x4Matrix2)
{
	XMFLOAT4X4 xmf4x4Result;
	XMStoreFloat4x4(&xmf4x4Result, XMMatrixMultiply(XMLoadFloat4x4(&xmmtx4x4Matrix1), XMLoadFloat4x4(&xmmtx4x4Matrix2)));
	return(xmf4x4Result);
}

inline XMFLOAT4X4 Multiply(XMFLOAT4X4& xmmtx4x4Matrix1, XMMATRIX& xmmtxMatrix2)
{
	XMFLOAT4X4 xmf4x4Result;
	XMStoreFloat4x4(&xmf4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) * xmmtxMatrix2);
	return(xmf4x4Result);
}

inline XMFLOAT4X4 Multiply(XMMATRIX& xmmtxMatrix1, XMFLOAT4X4& xmmtx4x4Matrix2)
{
	XMFLOAT4X4 xmf4x4Result;
	XMStoreFloat4x4(&xmf4x4Result, xmmtxMatrix1 * XMLoadFloat4x4(&xmmtx4x4Matrix2));
	return(xmf4x4Result);
}

inline XMFLOAT4X4 Identity()
{
	XMFLOAT4X4 xmf4x4Result;
	XMStoreFloat4x4(&xmf4x4Result, XMMatrixIdentity());
	return(xmf4x4Result);
}

inline XMFLOAT4X4 Zero()
{
	XMFLOAT4X4 xmf4x4Result;
	XMStoreFloat4x4(&xmf4x4Result, XMMatrixSet(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f));
	return(xmf4x4Result);
}