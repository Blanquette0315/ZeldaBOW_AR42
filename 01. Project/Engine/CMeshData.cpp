#include "pch.h"
#include "CMeshData.h"

#include "CPathMgr.h"
#include "CResMgr.h"

#include "CGameObject.h"
#include "CTransform.h"
#include "CMeshRender.h"

#include "CFBXLoader.h"




CMeshData::CMeshData(bool _bEngine)
	: CRes(RES_TYPE::MESHDATA, _bEngine)
{
}

CMeshData::~CMeshData()
{
}


CGameObject* CMeshData::Instantiate()
{
	CGameObject* pNewObj = new CGameObject;
	pNewObj->AddComponent(new CTransform);
	pNewObj->AddComponent(new CMeshRender);

	pNewObj->MeshRender()->SetMesh(m_pMesh);

	for (UINT i = 0; i < m_vecMtrl.size(); ++i)
	{
		pNewObj->MeshRender()->SetSharedMaterial(m_vecMtrl[i], i);
	}

	return pNewObj;
}




CMeshData* CMeshData::LoadFromFBX(const wstring& _strPath)
{
	wstring strFullPath = CPathMgr::GetInst()->GetContentPath();
	strFullPath += _strPath;

	CFBXLoader loader;
	loader.init();
	loader.LoadFbx(strFullPath);

	// �޽� ��������
	CMesh* pMesh = nullptr;
	pMesh = CMesh::CreateFromContainer(loader);

	// ResMgr �� �޽� ���
	if (nullptr != pMesh)
	{
		wstring strMeshKey = L"mesh\\";
		strMeshKey += path(strFullPath).stem();
		strMeshKey += L".mesh";
		CResMgr::GetInst()->AddRes<CMesh>(strMeshKey, pMesh);

		// �޽ø� ���� ���Ϸ� ����
		pMesh->Save(strMeshKey);
	}

	vector<Ptr<CMaterial>> vecMtrl;

	// ���׸��� ��������
	for (UINT i = 0; i < loader.GetContainer(0).vecMtrl.size(); ++i)
	{
		// ����ó�� (material �̸��� �Է� �ȵǾ����� ���� �ִ�.)
		Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(loader.GetContainer(0).vecMtrl[i].strMtrlName);
		assert(pMtrl.Get());

		vecMtrl.push_back(pMtrl);
	}

	CMeshData* pMeshData = new CMeshData(true);
	pMeshData->m_pMesh = pMesh;
	pMeshData->m_vecMtrl = vecMtrl;

	return pMeshData;
}

void CMeshData::Save(const wstring& _strRelativePath)
{
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath() + _strRelativePath;

	YAML::Emitter emitter;
	emitter << YAML::BeginMap;

	// ���� �̸� ����
	CEntity::SaveToYAML(emitter);

	// Key, RelativePath ����
	SaveKeyPath(emitter);

	// Mesh Key ����	
	// Mesh Data ����
	emitter << YAML::Key << "Mesh";
	emitter << YAML::Value << YAML::BeginMap;
	SaveResourceRef(m_pMesh, emitter);
	emitter << YAML::EndMap;

	// material ���� ����
	UINT iMtrlCount = (UINT)m_vecMtrl.size();
	emitter << YAML::Key << "MtrlCount";
	emitter << YAML::Value << iMtrlCount;

	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		if (nullptr == m_vecMtrl[i])
			continue;

		emitter << YAML::Key << "Material" + std::to_string(i);
		emitter << YAML::Value << YAML::BeginMap;
		SaveResourceRef<CMaterial>(m_vecMtrl[i], emitter);
		emitter << YAML::EndMap;
	}

	emitter << YAML::EndMap;
	std::ofstream fout(strFilePath);
	fout << emitter.c_str();
}

int CMeshData::Load(const wstring& _strFilePath)
{
	// ���� �б�
	YAML::Node rootNode = YAML::LoadFile(WStringToString(_strFilePath));

	// �̸� �ҷ�����
	CEntity::LoadFromYAML(rootNode);

	// Key, RelativePath �ҷ�����
	LoadKeyPath(rootNode);

	// Mesh Load
	YAML::Node node = rootNode["Mesh"];
	LoadResourceRef(m_pMesh, node);
	assert(m_pMesh.Get());

	// material ���� �б�
	UINT iMtrlCount = rootNode["MtrlCount"].as<UINT>();

	m_vecMtrl.resize(iMtrlCount);

	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		node = rootNode["Material" + std::to_string(i)];
		if (node == nullptr)
			continue;

		Ptr<CMaterial> pMtrl;
		LoadResourceRef<CMaterial>(pMtrl, node);

		m_vecMtrl[i] = pMtrl;
	}

	return S_OK;
}