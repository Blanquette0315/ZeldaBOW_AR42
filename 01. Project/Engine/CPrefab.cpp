#include "pch.h"
#include "CPrefab.h"

#include "CGameObject.h"

// 초기화
SAVE_GAMEOBJECT CPrefab::Save_GameObject_Func = nullptr;
LOAD_GAMEOBJECT CPrefab::Load_GameObject_Func = nullptr;

CPrefab::CPrefab()
	: CRes(RES_TYPE::PREFAB)
	, m_pProtoObj(nullptr)
{
}

CPrefab::CPrefab(CGameObject* _pProto)
	: CRes(RES_TYPE::PREFAB)
	, m_pProtoObj(_pProto)
{
	if (-1 != _pProto->GetLayerIdx())
	{
		m_pProtoObj = _pProto->Clone();
	}
}

CPrefab::~CPrefab()
{
	if (nullptr != m_pProtoObj)
		delete m_pProtoObj;
}

CGameObject* CPrefab::Instantiate()
{
	if(nullptr == m_pProtoObj)
		return nullptr;

	return m_pProtoObj->Clone();
}

void CPrefab::Save(const wstring& _strRelativePath)
{
	// 파일과 1:1 매칭 확인
	if (!CheckRelativePath(_strRelativePath))
		assert(nullptr);

	FILE* pFile = nullptr;

	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += _strRelativePath;

	_wfopen_s(&pFile, strFilePath.c_str(), L"wb");

	// 키, 경로 저장
	SaveKeyPath(pFile);

	// 원본 오브젝트 정보 저장
	Save_GameObject_Func(m_pProtoObj, pFile);

	fclose(pFile);
}

int CPrefab::Load(const wstring& _strFilePath)
{
	FILE* pFile = nullptr;

	_wfopen_s(&pFile, _strFilePath.c_str(), L"rb");

	LoadKeyPath(pFile);

	m_pProtoObj = Load_GameObject_Func(pFile);

	fclose(pFile);

	return S_OK;
}
