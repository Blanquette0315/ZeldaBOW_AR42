#include "pch.h"
#include "CMeshData.h"

#include "CPathMgr.h"
#include "CResMgr.h"

#include "CGameObject.h"
#include "CTransform.h"
#include "CMeshRender.h"
#include "CAnimator3D.h"
#include "CMapRender.h"

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

	// Animation 파트 추가
	if (false == m_pMesh->IsAnimMesh())
		return pNewObj;
	
	CAnimator3D* pAnimator = new CAnimator3D;
	pNewObj->AddComponent(pAnimator);
	
	pAnimator->SetBones(m_pMesh->GetBones());
	pAnimator->SetAnimClip(m_pMesh->GetAnimClip());

	return pNewObj;
}




CMeshData* CMeshData::LoadFromFBX(const wstring& _strPath)
{
	wstring strFullPath = CPathMgr::GetInst()->GetContentPath();

	path path_mesh = strFullPath + L"mesh\\" + path(_strPath).stem().wstring() + L"\\";
	if (false == exists(path_mesh))
	{
		create_directory(path_mesh);
	}
	strFullPath += _strPath;

	CFBXLoader loader;
	loader.init();
	loader.LoadFbx(strFullPath);

	// 메쉬 가져오기
	CMesh* pMesh = nullptr;
	pMesh = CMesh::CreateFromContainer(loader);

	// ResMgr 에 메쉬 등록
	if (nullptr != pMesh)
	{
		wstring strMeshKey = L"mesh\\" + path(_strPath).stem().wstring() + L"\\";
		strMeshKey += path(strFullPath).stem();
		strMeshKey += L".mesh";
		pMesh->SetName(strMeshKey);
		CResMgr::GetInst()->AddRes<CMesh>(strMeshKey, pMesh);

		// 메시를 실제 파일로 저장
		if (L"" == pMesh->GetKey())
		{
			delete pMesh;
			pMesh = CResMgr::GetInst()->FindRes<CMesh>(strMeshKey).Get();
		}
		pMesh->Save(strMeshKey);
		//pMesh->Save(strMeshKey);
	}

	vector<Ptr<CMaterial>> vecMtrl;

	// 메테리얼 가져오기
	for (UINT j = 0; j < loader.GetContainerCount(); ++j)
	{
		for (UINT i = 0; i < loader.GetContainer(j).vecMtrl.size(); ++i)
		{
			// 예외처리 (material 이름이 입력 안되어있을 수도 있다.)
			Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(loader.GetContainer(j).vecMtrl[i].strMtrlName);
			assert(pMtrl.Get());

			vecMtrl.push_back(pMtrl);
		}
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

	// 재질 이름 저장
	CEntity::SaveToYAML(emitter);

	// Key, RelativePath 저장
	SaveKeyPath(emitter);

	// Mesh Key 저장	
	// Mesh Data 저장
	emitter << YAML::Key << "Mesh";
	emitter << YAML::Value << YAML::BeginMap;
	SaveResourceRef(m_pMesh, emitter);
	emitter << YAML::EndMap;

	// material 정보 저장
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
	// 파일 읽기
	YAML::Node rootNode = YAML::LoadFile(WStringToString(_strFilePath));

	// 이름 불러오기
	CEntity::LoadFromYAML(rootNode);

	// Key, RelativePath 불러오기
	LoadKeyPath(rootNode);

	// Mesh Load
	YAML::Node node = rootNode["Mesh"];
	LoadResourceRef(m_pMesh, node);
	assert(m_pMesh.Get());

	// material 정보 읽기
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