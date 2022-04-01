#include "pch.h"
#include "MeshData.h"
#include "Mesh.h"
#include "Material.h"
#include "Resources.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "GameObject.h"


MeshData::MeshData() : Object(OBJECT_TYPE::MESH_DATA)
{
}

MeshData::~MeshData()
{
}

void MeshData::LoadMeshFromFile(const wstring& path)
{
	FILE* pFile = NULL;

	// wchar_t -> char*
	char* pStr;
	int iLen = WideCharToMultiByte(CP_ACP, 0, path.c_str(), -1, NULL, 0, NULL, NULL);
	pStr = new char[iLen];
	WideCharToMultiByte(CP_ACP, 0, path.c_str(), -1, pStr, iLen, 0, 0);

	// ��� ����
	wstring strpath = L"..\\Resources\\Binary\\" + path;

	fopen_s(&pFile, ws2s(strpath).c_str(), "rb");
	rewind(pFile);

	char pStrTocken[64] = { '\0' };
	GameObject* pGameObj = new GameObject();

	for (; ;)
	{
		if (ReadStringFromFile(pFile, pStrTocken))
		{
			if (!strcmp(pStrTocken, "<Hierarchy>"))
			{
				for (; ;)
				{
					ReadStringFromFile(pFile, pStrTocken);
					if (!strcmp(pStrTocken, "<Frame>:"))
					{
						pGameObj = LoadFrameHierarchyFromFile(NULL, pFile);
						fclose(pFile);

						// Resources�� �ؽ�ó, ���� �߰�
						CreateTextures();
						CreateMaterials();


						// Mesh, Material ����
						// - mesh
						shared_ptr<Mesh> mesh = make_shared<Mesh>();
						mesh->CreateStaticMeshFromFBX(&_staticMeshInfo);
						mesh->SetName(path);
						GET_SINGLE(Resources)->Add<Mesh>(mesh->GetName(), mesh);

						// - material
						shared_ptr<Material>	material = GET_SINGLE(Resources)->Get<Material>(_staticMeshInfo.material.name);

						// �տ��� ������ Mesh, Material�� ������� _meshRenders�� �߰�
						// ���Ŀ� Instantiate()���� _meshRenders�� ������� �޽��� ���� ������ ��������
						// -> �̸� �о�� Mesh, Material ������ ��ü�� �����ؼ� �������(_meshRenders)�� ���� �ִٰ� 
						//    ���߿� �ٸ� Ŭ�������� �ش� �޽��� �ʿ��� �� �� _meshRenders ������ ������� �����ؼ� ����
						MeshRendererInfo	info = {};
						info.mesh = mesh;
						info.materials = material;

						_meshRenders.push_back(info);

						return;
					}
					else if (!strcmp(pStrTocken, "</Hierarchy>"))
					{
						fclose(pFile);
						return;
					}
				}
			}
			if (!strcmp(pStrTocken, "</Hierarchy>"))
			{
				fclose(pFile);
				return;
			}
		}
	}
}

GameObject* MeshData::LoadFrameHierarchyFromFile(GameObject* parent, FILE* pFile)
{
	char pStrTocken[64] = { '\0' };
	UINT	nReads = 0;

	int		nFrame = ReadIntegerFromFile(pFile);
	GameObject* pGameObj = new GameObject();

	// ������ �̸����� �ٲ�� ��!
	ReadStringFromFile(pFile, pStrTocken);

	// char -> wchar_t
	int nSize = MultiByteToWideChar(CP_ACP, 0, pStrTocken, -1, NULL, NULL);
	wchar_t* pStr;
	pStr = new WCHAR[nSize];
	MultiByteToWideChar(CP_ACP, 0, pStrTocken, strlen(pStrTocken) + 1, pStr, nSize);

	_staticMeshInfo.material.name = pStr;

	for (; ;)
	{
		ReadStringFromFile(pFile, pStrTocken);

		if (!strcmp(pStrTocken, "<Transform>:"))
		{
			Vec3	xmf3Scale, xmf3Rotate, xmf3Trans;
			Matrix	xmf4x4ToParent;

			nReads = (UINT)::fread(&xmf4x4ToParent, sizeof(XMFLOAT4X4), 1, pFile);

			nReads = (UINT)::fread(&xmf3Scale, sizeof(XMFLOAT3), 1, pFile);
			nReads = (UINT)::fread(&xmf3Rotate, sizeof(XMFLOAT3), 1, pFile);
			nReads = (UINT)::fread(&xmf3Trans, sizeof(XMFLOAT3), 1, pFile);

			//pGameObj->GetTransform()->SetLocalToParent(xmf4x4ToParent);
			//pGameObj->GetTransform()->SetLocalScale(xmf3Scale);
			//pGameObj->GetTransform()->SetLocalRotation(xmf3Rotate);
			//pGameObj->GetTransform()->SetLocalPosition(xmf3Trans);
		}

		else if (!strcmp(pStrTocken, "<Mesh>:"))
		{
			LoadMeshInfoFromFile(pFile);
		}

		else if (!strcmp(pStrTocken, "<Materials>:"))
		{
			LoadMaterialInfoFromFile(pFile);
			return pGameObj;
		}
	}
}

void MeshData::LoadMeshInfoFromFile(FILE* pFile)
{
	char pStrTocken[64] = { '\0' };
	int	nReads, nVertices, nUVs, nNormals, nTangents, nPolygons, nIndices, nSubMeshes, nAllCnt;

	ReadStringFromFile(pFile, pStrTocken);

	for (; ;)
	{
		ReadStringFromFile(pFile, pStrTocken);

		if (!strcmp(pStrTocken, "<ControlPoints>:"))
		{
			nReads = (UINT)fread(&nVertices, sizeof(int), 1, pFile);

			if (nVertices)
			{
				_staticMeshInfo.vertices.resize(nVertices);
				Vec3* pos = new Vec3[nVertices];

				nReads = (UINT)fread(pos, sizeof(Vec3), nVertices, pFile);

				for (int i = 0; i < nVertices; ++i)
					_staticMeshInfo.vertices[i].pos = pos[i];
			}
		}

		else if (!strcmp(pStrTocken, "<UVs>:"))
		{
			nReads = (UINT)fread(&nUVs, sizeof(int), 1, pFile);
			nReads = (UINT)fread(&nAllCnt, sizeof(int), 1, pFile);

			if (nUVs)
			{
				Vec2* uv = new Vec2[nUVs];

				nReads = (UINT)fread(uv, sizeof(Vec2), nUVs, pFile);

				for (int i = 0; i < nUVs; ++i)
					_staticMeshInfo.vertices[i].uv = uv[i];
			}
		}

		else if (!strcmp(pStrTocken, "<Normals>:"))
		{
			nReads = (UINT)fread(&nNormals, sizeof(int), 1, pFile);
			nReads = (UINT)fread(&nAllCnt, sizeof(int), 1, pFile);

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
			nReads = (UINT)fread(&nAllCnt, sizeof(int), 1, pFile);

			if (nAllCnt)
			{
				Vec3* tangent = new Vec3[nTangents];

				nReads = (UINT)fread(tangent, sizeof(Vec3), nTangents, pFile);

				for (int i = 0; i < nTangents; ++i)
					_staticMeshInfo.vertices[i].tangent = tangent[i];
			}
		}

		else if (!strcmp(pStrTocken, "<Polygons>:"))
		{
			nReads = (UINT)fread(&nPolygons, sizeof(int), 1, pFile);

			ReadStringFromFile(pFile, pStrTocken);

			if (!strcmp(pStrTocken, "<SubIndices>:"))
			{
				nIndices = ReadIntegerFromFile(pFile);
				nSubMeshes = ReadIntegerFromFile(pFile);

				_staticMeshInfo.indices.resize(nSubMeshes);

				ReadStringFromFile(pFile, pStrTocken);

				if (!strcmp(pStrTocken, "<SubIndex>:"))
				{
					int nIndex = ReadIntegerFromFile(pFile);
					nIndices = ReadIntegerFromFile(pFile);	// 1470

					// ���� �ε��� �о��
					UINT* subIndices = new UINT[nIndices];
					nReads = (UINT)fread(subIndices, sizeof(UINT), nIndices, pFile);

					for (int i = 0; i < nIndices; ++i)
					{
						_staticMeshInfo.indices[nSubMeshes - 1].push_back(subIndices[i]);
					}
				}
			}

		}

		else if (!strcmp(pStrTocken, "</Mesh>"))
			return;
	}
}

void MeshData::LoadMaterialInfoFromFile(FILE* pFile)
{
	char pStrTocken[64] = { '\0' };
	int nMaterials = ReadIntegerFromFile(pFile);

	ReadStringFromFile(pFile, pStrTocken);

	if (!strcmp(pStrTocken, "<Material>:"))
	{
		nMaterials = ReadIntegerFromFile(pFile);
		if (nMaterials == 0)
		{
			ReadStringFromFile(pFile, pStrTocken);
			_staticMeshInfo.material.name = s2ws(pStrTocken);

			// char -> wchar_t
			int nSize = MultiByteToWideChar(CP_ACP, 0, pStrTocken, -1, NULL, NULL);
			wchar_t* pStr;
			pStr = new WCHAR[nSize];
			MultiByteToWideChar(CP_ACP, 0, pStrTocken, strlen(pStrTocken) + 1, pStr, nSize);

			// ���߿� �����Ұ���. �Ű�X
			/////////////////////////////////////////////////////////////////////////////////////
			// SimpleNaturePack_Texture_01
			//_staticMeshInfo.material.diffuseTexName = pStr;
			//_staticMeshInfo.material.diffuseTexName = L"SimpleNaturePack_Texture_01";
			//_staticMeshInfo.material.diffuseTexName = L"rpgpp_lt_tex_a";
			/////////////////////////////////////////////////////////////////////////////////////
			
			if (!strcmp(pStrTocken, "Present"))
				_staticMeshInfo.material.diffuseTexName = L"paper_diffuse1";
			else if (!strcmp(pStrTocken, "Present4"))
				_staticMeshInfo.material.diffuseTexName = L"paper_diffuse4";
			else if (!strcmp(pStrTocken, "Nature_Mat_01"))
				_staticMeshInfo.material.diffuseTexName = L"SimpleNaturePack_Texture_01";
		}

		ReadStringFromFile(pFile, pStrTocken);

		if (!strcmp(pStrTocken, "<ShadingModel>:"))
		{
			ReadStringFromFile(pFile, pStrTocken);

			if (!strcmp(pStrTocken, "<Lambert>:"))
			{
				ReadStringFromFile(pFile, pStrTocken);

				_staticMeshInfo.material.ambient.x = ReadFloatFromFile(pFile);
				_staticMeshInfo.material.ambient.y = ReadFloatFromFile(pFile);
				_staticMeshInfo.material.ambient.z = ReadFloatFromFile(pFile);

				_staticMeshInfo.material.diffuse.x = ReadFloatFromFile(pFile);
				_staticMeshInfo.material.diffuse.y = ReadFloatFromFile(pFile);
				_staticMeshInfo.material.diffuse.z = ReadFloatFromFile(pFile);

				_staticMeshInfo.material.specular.x = ReadFloatFromFile(pFile);
				_staticMeshInfo.material.specular.y = ReadFloatFromFile(pFile);
				_staticMeshInfo.material.specular.z = ReadFloatFromFile(pFile);

				float fGlossiness = ReadFloatFromFile(pFile);
			}

			else if (!strcmp(pStrTocken, "Phong"))
			{
				_staticMeshInfo.material.ambient.x = ReadFloatFromFile(pFile);
				_staticMeshInfo.material.ambient.y = ReadFloatFromFile(pFile);
				_staticMeshInfo.material.ambient.z = ReadFloatFromFile(pFile);

				_staticMeshInfo.material.diffuse.x = ReadFloatFromFile(pFile);
				_staticMeshInfo.material.diffuse.y = ReadFloatFromFile(pFile);
				_staticMeshInfo.material.diffuse.z = ReadFloatFromFile(pFile);

				_staticMeshInfo.material.specular.x = ReadFloatFromFile(pFile);
				_staticMeshInfo.material.specular.y = ReadFloatFromFile(pFile);
				_staticMeshInfo.material.specular.z = ReadFloatFromFile(pFile);

				float transparencyFactor = ReadFloatFromFile(pFile);
				float shininess = ReadFloatFromFile(pFile);
				float reflectionFactor = ReadFloatFromFile(pFile);
			}
		}

		if (!strcmp(pStrTocken, "</Materials>"))
			return;
	}
}

void MeshData::CreateTextures()
{
	// �츮�� ����ϴ� Static Mesh, Player Model�� Diffuse Texture�� ����

	// Diffuse
	wstring		fileName = _staticMeshInfo.material.diffuseTexName.c_str();
	wstring		fullPath = L"..\\Resources\\Texture\\" + fileName + L".png";
	if (!fileName.empty())
		GET_SINGLE(Resources)->Load<Texture>(fileName, fullPath);
}

void MeshData::CreateMaterials()
{
	// �츮�� ����ϴ� Static Mesh, Player Model�� Material�� ��� �ϳ�

	shared_ptr<Material>	material = make_shared<Material>();
	wstring		key = _staticMeshInfo.material.name;

	material->SetName(key);
	material->SetShader(GET_SINGLE(Resources)->Get<Shader>(L"Deferred"));

	material->SetInt(0, 1);

	{
		wstring		diffuseName = _staticMeshInfo.material.diffuseTexName.c_str();
		wstring		fileName = fs::path(diffuseName).filename();
		wstring		key = fileName;

		shared_ptr<Texture>	diffuseTex = GET_SINGLE(Resources)->Get<Texture>(key);
		if (diffuseTex)	material->SetTexture(0, diffuseTex);
	}

	GET_SINGLE(Resources)->Add<Material>(key, material);
}

vector<shared_ptr<GameObject>> MeshData::Instantiate()
{
	// vector: Mesh�� Material�� �������� �� ����
	// �ٵ� �츮�� �� �ϳ����̶� �� �ǹ̴� ����
	vector<shared_ptr<GameObject>>	v;

	for (MeshRendererInfo& info : _meshRenders)
	{
		shared_ptr<GameObject>	gameObject = make_shared<GameObject>();
		gameObject->AddComponent(make_shared<Transform>());
		gameObject->AddComponent(make_shared<MeshRenderer>());
		gameObject->GetMeshRenderer()->SetMesh(info.mesh);
		gameObject->GetMeshRenderer()->SetMaterial(info.materials);

		v.push_back(gameObject);
	}

	return v;
}