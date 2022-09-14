#include "pch.h"
#include "CharacterData.h"
#include "Mesh.h"
#include "Material.h"
#include "Resources.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "AnimationController.h"
#include "AnimationTrack.h"
#include "AnimationSets.h"
#include "AnimationSet.h"

CharacterData::CharacterData() : MeshData()
{
}

CharacterData::~CharacterData()
{
}

void CharacterData::LoadCharacterFromFile(const wstring& path)
{
	FILE* pFile;
	_name = path;

	// wchar_t -> char*
	char* pStr;
	int iLen = WideCharToMultiByte(CP_ACP, 0, path.c_str(), -1, NULL, 0, NULL, NULL);
	pStr = new char[iLen];
	WideCharToMultiByte(CP_ACP, 0, path.c_str(), -1, pStr, iLen, 0, 0);

	wstring strpath = L"..\\Resources\\Binary\\" + path + L".bin";

	fopen_s(&pFile, ws2s(strpath).c_str(), "rb");
	if (pFile == NULL)
	{
		return;
	}
	rewind(pFile);

	char pStrTocken[64] = { '\0' };
	_modelInfo = make_shared<AnimationModelInfo>();
	_modelInfo->_rootObject = make_shared<Transform>();

	for (; ;)
	{
		if (ReadStringFromFileForCharac(pFile, pStrTocken))
		{
			if (!strcmp(pStrTocken, "<Hierarchy>:"))
			{
				_modelInfo->_rootObject = LoadFrameHierarchyFromFile(NULL, pFile);
				int l = 0;
			}

			else if (!strcmp(pStrTocken, "</Hierarchy>"))
			{
				CreateMaterials();
			}

			else if (!strcmp(pStrTocken, "<Animation>:"))
			{
				LoadAnimationInfo(pFile);

				// Mesh 생성해서 Reosurces에 추가
				shared_ptr<Mesh> mesh = make_shared<Mesh>();
				mesh->CreateAnimationMeshFromFBX(&_staticMeshInfo, _animationClipInfo, _skinningInfo);
				mesh->SetName(path);
				GET_SINGLE(Resources)->Add<Mesh>(mesh->GetName(), mesh);


				// Material 생성해서 Reosurces에 추가
				shared_ptr<Material>	material = GET_SINGLE(Resources)->Get<Material>(_name);
				shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"PlayerAnimation");
				
				material->SetShader(shader);

				MeshRendererInfo	info = {};
				info.mesh = mesh;
				info.materials = material;

				_meshRenders.push_back(info);

				fclose(pFile);
				return;
			}
		}
	}
}

shared_ptr<Transform> CharacterData::LoadFrameHierarchyFromFile(shared_ptr<Transform> parent, FILE* pFile)
{
	char pStrTocken[64] = { '\0' };
	UINT	nReads = 0;

	shared_ptr<Transform>	pTrans = make_shared<Transform>();

	for (; ;)
	{
		ReadStringFromFileForCharac(pFile, pStrTocken);

		if (!strcmp(pStrTocken, "<Frame>:"))
		{
			int nFrame = ReadIntegerFromFile(pFile);
			int nTexture = ReadIntegerFromFile(pFile);

			ReadStringFromFileForCharac(pFile, pStrTocken);
			pTrans->SetName(s2ws(pStrTocken));
		}

		else if (!strcmp(pStrTocken, "<Transform>:"))
		{
			Vec4 xmf4Rotation;
			Vec3 pos, rot, scale;

			nReads = (UINT)::fread(&pos, sizeof(float), 3, pFile);
			nReads = (UINT)::fread(&rot, sizeof(float), 3, pFile); //Euler Angle
			nReads = (UINT)::fread(&scale, sizeof(float), 3, pFile);
			nReads = (UINT)::fread(&xmf4Rotation, sizeof(float), 4, pFile); //Quaternion
		}

		else if (!strcmp(pStrTocken, "<TransformMatrix>:"))
		{
			nReads = (UINT)fread(&pTrans->_matToParent, sizeof(float), 16, pFile);
		}

		else if (!strcmp(pStrTocken, "<Children>:"))
		{
			int		nChild = ReadIntegerFromFile(pFile);

			if (nChild > 0)
			{
				for (int i = 0; i < nChild; ++i)
				{
					shared_ptr<Transform>	childTrans = make_shared<Transform>();
					childTrans = LoadFrameHierarchyFromFile(pTrans, pFile);
					pTrans->SetChild(childTrans);
				}
			}
		}

		else if (!strcmp(pStrTocken, "<SkinningInfo>:"))
		{
			LoadSkinningInfoFromFile(pFile);

			ReadStringFromFileForCharac(pFile, pStrTocken);
			if (!strcmp(pStrTocken, "<Mesh>:"))		LoadMeshInfoFromFile(pFile);
		}

		else if (!strcmp(pStrTocken, "<Mesh>:"))		LoadMeshInfoFromFile(pFile);
		else if (!strcmp(pStrTocken, "<Materials>:"))	LoadMaterialInfoFromFile(pFile);
		else if (!strcmp(pStrTocken, "</Frame>"))		break;
	}

	return pTrans;
}

void CharacterData::LoadSkinningInfoFromFile(FILE* pFile)
{
	char pStrTocken[64] = { '\0' };
	UINT	nReads = 0;
	int		nSkinnings = 0;

	ReadStringFromFileForCharac(pFile, pStrTocken);

	for (; ;)
	{
		ReadStringFromFileForCharac(pFile, pStrTocken);

		if (!strcmp(pStrTocken, "<BonesPerVertex>:"))
		{
			int		nVertexs = ReadIntegerFromFile(pFile);
		}

		else if (!strcmp(pStrTocken, "<Bounds>:"))
		{
			Vec3 aabbCenter, aabbExtents;

			nReads = (UINT)::fread(&aabbCenter, sizeof(Vec3), 1, pFile);
			nReads = (UINT)::fread(&aabbExtents, sizeof(Vec3), 1, pFile);
		}

		else if (!strcmp(pStrTocken, "<BoneNames>:"))
		{
			nSkinnings = ReadIntegerFromFile(pFile);
			_skinningInfo.nSkinnings = nSkinnings;

			if (nSkinnings > 0)
			{
				_skinningInfo.boneNames.resize(nSkinnings);

				for (int i = 0; i < nSkinnings; ++i)
				{
					ReadStringFromFileForCharac(pFile, pStrTocken);
					_skinningInfo.boneNames[i] = s2ws(pStrTocken);
				}
			}
		}

		else if (!strcmp(pStrTocken, "<BoneOffsets>:"))
		{
			nSkinnings = ReadIntegerFromFile(pFile);

			if (nSkinnings > 0)
			{
				_skinningInfo.boneOffsets.resize(nSkinnings);

				nReads = (UINT)fread(&_skinningInfo.boneOffsets[0], sizeof(Matrix), nSkinnings, pFile);
			}
		}

		else if (!strcmp(pStrTocken, "<BoneIndices>:"))
		{
			nSkinnings = ReadIntegerFromFile(pFile);

			if (nSkinnings > 0)
			{
				_skinningInfo.boneIndices.resize(nSkinnings);

				nReads = (UINT)fread(&_skinningInfo.boneIndices[0], sizeof(XMINT4), nSkinnings, pFile);
			}
		}

		else if (!strcmp(pStrTocken, "<BoneWeights>:"))
		{
			nSkinnings = ReadIntegerFromFile(pFile);

			if (nSkinnings > 0)
			{
				_skinningInfo.boneWeights.resize(nSkinnings);

				nReads = (UINT)fread(&_skinningInfo.boneWeights[0], sizeof(Vec4), nSkinnings, pFile);
			}
		}

		else if (!strcmp(pStrTocken, "</SkinningInfo>"))
			return;
	}
}

void CharacterData::LoadMeshInfoFromFile(FILE* pFile)
{
	char pStrTocken[64] = { '\0' };
	int	nReads, nVertices, nUVs, nNormals, nTangents, nSubMeshes;

	nVertices = ReadIntegerFromFile(pFile);

	for (; ;)
	{
		ReadStringFromFileForCharac(pFile, pStrTocken);

		if (!strcmp(pStrTocken, "<Bounds>:"))
		{
			Vec3 aabbCenter, aabbExtents;

			nReads = (UINT)::fread(&aabbCenter, sizeof(Vec3), 1, pFile);
			nReads = (UINT)::fread(&aabbExtents, sizeof(Vec3), 1, pFile);
		}

		else if (!strcmp(pStrTocken, "<Positions>:") || !strcmp(pStrTocken, "<ControlPoints>:"))
		{
			nReads = (UINT)fread(&nVertices, sizeof(int), 1, pFile);

			if (nVertices)
			{
				_staticMeshInfo.vertices.resize(nVertices);
				Vec3* pos = new Vec3[nVertices];

				nReads = (UINT)fread(pos, sizeof(Vec3), nVertices, pFile);

				for (int i = 0; i < nVertices; ++i)
				{
					_staticMeshInfo.vertices[i].pos = pos[i];

					_staticMeshInfo.vertices[i].indices.x = static_cast<float>(_skinningInfo.boneIndices[i].x);
					_staticMeshInfo.vertices[i].indices.y = static_cast<float>(_skinningInfo.boneIndices[i].y);
					_staticMeshInfo.vertices[i].indices.z = static_cast<float>(_skinningInfo.boneIndices[i].z);
					_staticMeshInfo.vertices[i].indices.w = static_cast<float>(_skinningInfo.boneIndices[i].w);

					_staticMeshInfo.vertices[i].weights.x = _skinningInfo.boneWeights[i].x;
					_staticMeshInfo.vertices[i].weights.y = _skinningInfo.boneWeights[i].y;
					_staticMeshInfo.vertices[i].weights.z = _skinningInfo.boneWeights[i].z;
					_staticMeshInfo.vertices[i].weights.w = _skinningInfo.boneWeights[i].w;
				}
			}
		}

		else if (!strcmp(pStrTocken, "<TextureCoords0>:") || !strcmp(pStrTocken, "<UVs>:"))
		{
			nReads = (UINT)fread(&nUVs, sizeof(int), 1, pFile);

			if (nUVs)
			{
				Vec2* uv = new Vec2[nUVs];

				nReads = (UINT)fread(uv, sizeof(Vec2), nUVs, pFile);

				for (int i = 0; i < nUVs; ++i)
					_staticMeshInfo.vertices[i].uv = uv[i];
			}
		}

		else if (!strcmp(pStrTocken, "<TextureCoords1>:"))
		{
			nReads = (UINT)fread(&nUVs, sizeof(int), 1, pFile);

			if (nUVs)
			{
				Vec2* uv = new Vec2[nUVs];

				nReads = (UINT)fread(uv, sizeof(Vec2), nUVs, pFile);
			}
		}

		else if (!strcmp(pStrTocken, "<Normals>:"))
		{
			nReads = (UINT)fread(&nNormals, sizeof(int), 1, pFile);

			if (nNormals)
			{
				Vec3* normal = new Vec3[nNormals];

				nReads = (UINT)fread(normal, sizeof(Vec3), nNormals, pFile);

				for (int i = 0; i < nNormals; ++i)
					_staticMeshInfo.vertices[i].normal = normal[i];
			}
		}

		else if (!strcmp(pStrTocken, "<Tangents>:"))
		{
			nReads = (UINT)fread(&nTangents, sizeof(int), 1, pFile);

			if (nTangents)
			{
				Vec3* tangent = new Vec3[nTangents];

				nReads = (UINT)fread(tangent, sizeof(Vec3), nTangents, pFile);

				for (int i = 0; i < nTangents; ++i)
					_staticMeshInfo.vertices[i].tangent = tangent[i];
			}
		}

		else if (!strcmp(pStrTocken, "<BiTangents>:"))
		{
			nReads = (UINT)fread(&nTangents, sizeof(int), 1, pFile);

			if (nTangents)
			{
				Vec3* tangent = new Vec3[nTangents];

				nReads = (UINT)fread(tangent, sizeof(Vec3), nTangents, pFile);
			}
		}

		else if (!strcmp(pStrTocken, "<SubMeshes>:"))
		{
			nSubMeshes = ReadIntegerFromFile(pFile);

			ReadStringFromFileForCharac(pFile, pStrTocken);

			if (!strcmp(pStrTocken, "<SubMesh>:"))
			{
				int nSubMesh = ReadIntegerFromFile(pFile);
				int nIndices = ReadIntegerFromFile(pFile);

				_staticMeshInfo.indices.resize(nSubMeshes);

				// 이제 인덱스 읽어옴
				UINT* subIndices = new UINT[nIndices];
				nReads = (UINT)fread(subIndices, sizeof(UINT), nIndices, pFile);

				for (int i = 0; i < nIndices; ++i)
				{
					_staticMeshInfo.indices[nSubMeshes - 1].push_back(subIndices[i]);
				}
			}
		}

		else if (!strcmp(pStrTocken, "<Colors>:"))
		{
			int nColors;
			nReads = (UINT)::fread(&nColors, sizeof(int), 1, pFile);
			if (nColors > 0)
			{
				XMFLOAT4* m_pxmf4Colors;
				m_pxmf4Colors = new XMFLOAT4[nColors];
				nReads = (UINT)::fread(m_pxmf4Colors, sizeof(XMFLOAT4), nColors, pFile);
			}
		}

		else if (!strcmp(pStrTocken, "</Mesh>"))
			return;
	}
}

void CharacterData::LoadMaterialInfoFromFile(FILE* pFile)
{
	char pStrTocken[64] = { '\0' };
	UINT	nReads = 0;
	int		nMaterials = ReadIntegerFromFile(pFile);

	// !나중에 파일에 추가해서 읽어오기!
	_staticMeshInfo.material.name = _name;

	ReadStringFromFileForCharac(pFile, pStrTocken);

	if (!strcmp(pStrTocken, "<Material>:"))
	{
		int		nMaterial = ReadIntegerFromFile(pFile);

		for (; ;)
		{
			ReadStringFromFileForCharac(pFile, pStrTocken);

			if (!strcmp(pStrTocken, "<AlbedoColor>:"))
				nReads = (UINT)fread(&_staticMeshInfo.material.diffuse, sizeof(float), 4, pFile);

			else if (!strcmp(pStrTocken, "<AlbedoMap>:"))
			{
				ReadStringFromFileForCharac(pFile, pStrTocken);
				_staticMeshInfo.material.diffuseTexName = s2ws(pStrTocken);
			}

			else if (!strcmp(pStrTocken, "</Materials>"))	return;
		}
	}
}

void CharacterData::LoadAnimationInfo(FILE* pFile)
{
	char	pStrTocken[64] = { '\0' };
	UINT	nReads = 0;
	int32	nAnimationSets, nFrames;
	int nSkin;
	
	ReadStringFromFileForCharac(pFile, pStrTocken);

	if (!strcmp(pStrTocken, "<AnimationSets>:"))
	{
		nAnimationSets = ReadIntegerFromFile(pFile);
	}
	for (; ;)
	{
		ReadStringFromFileForCharac(pFile, pStrTocken);

		if (!strcmp(pStrTocken, "<FrameNames>:"))
		{
			// 1. SkinMesh의 총 개수
			int nSkins = ReadIntegerFromFile(pFile);	// 1

			// 2. 몇 번째 SkinMesh인지
			nSkin = ReadIntegerFromFile(pFile);		// 0
			_modelInfo->_allAnimationSets = make_shared<AnimationSets>(nAnimationSets);

			// 3. SkinMesh Name
			ReadStringFromFileForCharac(pFile, pStrTocken);	// SkineMesh name: EvilBear
			
			// 4. 애니메이션에 사용되는 뼈들의 개수
			nFrames = ReadIntegerFromFile(pFile);	// 72

			_modelInfo->_vecAnimatedFrame.resize(nFrames);

			// 5. 애니메이션에 사용되는 프레임(뼈)들의 이름
			for (int i = 0; i < nFrames; ++i)
			{
				ReadStringFromFileForCharac(pFile, pStrTocken);	// FrameName

				wstring	frameName = s2ws(pStrTocken);

				_modelInfo->_vecAnimatedFrame[i] = make_shared<Transform>();
				_modelInfo->_vecAnimatedFrame[i] = _modelInfo->_rootObject->FindTransform(frameName);
			}
		}

		else if (!strcmp(pStrTocken, "<AnimationSet>:"))
		{
			shared_ptr<AnimationClipInfo>	info = make_shared<AnimationClipInfo>();

			int nSet = ReadIntegerFromFile(pFile);

			ReadStringFromFileForCharac(pFile, pStrTocken);
			info->name = s2ws(pStrTocken);

			info->length = ReadFloatFromFile(pFile);
			info->framePerSec = ReadIntegerFromFile(pFile);
			// 해당 애니메이션의 키 프레임 개수
			info->nkeyFrames = ReadIntegerFromFile(pFile);
			
			_modelInfo->_allAnimationSets->_animationSet[nSet] = new AnimationSet(info->length, info->framePerSec, info->nkeyFrames, nFrames, info->name);
			info->keyFrames.resize(info->nkeyFrames);

			_animationClipInfo.push_back(info);

			// i 프레임마다 애니메이션의 정보를 받아온다
			//   - 프레임 번호, 해당 프레임 재생 시간, 뼈들의 행렬
			for (int i = 0; i < info->nkeyFrames; ++i)
			{
				ReadStringFromFileForCharac(pFile, pStrTocken);

				if (!strcmp(pStrTocken, "<Transforms>:"))
				{
					AnimationFrameInfo	frameInfo;

					frameInfo.key = ReadIntegerFromFile(pFile);
					frameInfo.time = ReadFloatFromFile(pFile);
					int nSkin = ReadIntegerFromFile(pFile);

					frameInfo.matOffset.resize(nFrames);

					_modelInfo->_allAnimationSets->_animationSet[nSet]->_keyFrameTimes[i] = static_cast<float>(frameInfo.time);
					nReads = (UINT)fread(&_modelInfo->_allAnimationSets->_animationSet[nSet]->_keyFrameTrans[i][0], sizeof(Matrix), nFrames, pFile);

					_animationClipInfo[nSet]->keyFrames[i] = frameInfo;
				}
			}

		}

		else if (!strcmp(pStrTocken, "</Animation>"))
			return;
	}
}

void CharacterData::CreateMaterials()
{
	shared_ptr<Material>	material = make_shared<Material>();
	//wstring		key = _staticMeshInfo.material.name;
	//wstring		key = _staticMeshInfo.material.diffuseTexName;
	wstring		key = _name;

	material->SetName(key);

	material->SetShader(GET_SINGLE(Resources)->Get<Shader>(L"Deferred"));

	material->SetInt(0, 0);

	if (key != L"Material")
	{
		//wstring		diffuseName = _staticMeshInfo.material.diffuseTexName.c_str();
		//wstring		fileName = fs::path(diffuseName).filename();
		//wstring		key = fileName;

		shared_ptr<Texture>	diffuseTex = GET_SINGLE(Resources)->Get<Texture>(key);
		if (diffuseTex)	material->SetTexture(0, diffuseTex);
	}

	GET_SINGLE(Resources)->Add<Material>(key, material);
}

vector<shared_ptr<GameObject>> CharacterData::Instantiate()
{
	vector<shared_ptr<GameObject>>	vCharacter;

	for (MeshRendererInfo& info : _meshRenders)
	{
		shared_ptr<GameObject>	gameObject = make_shared<GameObject>();
		gameObject->AddComponent(make_shared<Transform>());
		gameObject->AddComponent(make_shared<MeshRenderer>());
		gameObject->GetMeshRenderer()->SetMesh(info.mesh);
		gameObject->GetMeshRenderer()->SetMaterial(info.materials);
		gameObject->GetTransform()->SetChild(_modelInfo->_rootObject);

		gameObject->AddComponent(make_shared<AnimationController>());
		gameObject->GetAnimationController()->SetAnimClips(_animationClipInfo);
		gameObject->GetAnimationController()->SetModelInfo(_modelInfo, _skinningInfo);

		vCharacter.push_back(gameObject);
	}

	return vCharacter;
}
