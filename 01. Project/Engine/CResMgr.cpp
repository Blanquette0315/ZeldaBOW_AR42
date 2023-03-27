#include "pch.h"
#include "CResMgr.h"
#include "CGameObject.h"

CResMgr::CResMgr()
	: m_iLayoutOffset(0)
	, m_bChanged(false)
{

}

CResMgr::~CResMgr()
{
	for (UINT i = 0; i < (UINT)RES_TYPE::END; ++i)
	{
		map<wstring, Ptr<CRes>>::iterator iter = m_arrRes[i].begin();

		for (; iter != m_arrRes[i].end(); ++iter)
		{
			if (i == (UINT)RES_TYPE::PREFAB)
			{
				CPrefab* pPref = (CPrefab*)iter->second.Get();
				delete pPref->GetProtoObj();
				pPref->m_pProtoObj = nullptr;
			}
			iter->second = nullptr;
		}
	}
}

void CResMgr::AddRes(const wstring& _strKey, RES_TYPE _type, CRes* _pRes)
{
	Ptr<CRes> pRes = FindRes(_strKey, _type);

	assert(!pRes.Get());

	_pRes->SetKey(_strKey);
	m_arrRes[(UINT)_type].insert(make_pair(_strKey, _pRes));
	m_bChanged = true;
}

Ptr<CRes> CResMgr::FindRes(const wstring& _strKey, RES_TYPE _type)
{
	map<wstring, Ptr<CRes>>::iterator iter = m_arrRes[(UINT)_type].find(_strKey);

	if (iter == m_arrRes[(UINT)_type].end())
	{
		return nullptr;
	}

	return iter->second;
}

Ptr<CTexture> CResMgr::CreateTexture(const wstring& _strKey, UINT _iWidth, UINT _iHeight, DXGI_FORMAT _FORMAT, UINT _iBindFlag)
{
	// 동일한 키값으로 등록된 Texture가 있는지 확인
	Ptr<CTexture> pTex = FindRes<CTexture>(_strKey);
	assert(!pTex.Get());

	pTex = new CTexture(true);
	pTex->Create(_iWidth, _iHeight, _FORMAT, _iBindFlag);

	AddRes<CTexture>(_strKey, pTex.Get());

	return pTex;
}

Ptr<CTexture> CResMgr::CreateTexture(const wstring& _strKey, ComPtr<ID3D11Texture2D> _Tex2D)
{
	// 동일한 키값으로 등록된 Texture가 있는지 확인
	Ptr<CTexture> pTex = FindRes<CTexture>(_strKey);
	assert(!pTex.Get());

	pTex = new CTexture(true);
	pTex->Create(_Tex2D);

	AddRes<CTexture>(_strKey, pTex.Get());

	return pTex;
}

Ptr<CTexture> CResMgr::LoadTexture(const wstring& _strKey, const wstring& _strRelativePath, int _iMapLevel)
{
	CTexture* pRes = FindRes<CTexture>(_strKey).Get();
	if (nullptr != pRes)
	{
		return pRes;
	}

	pRes = new CTexture;

	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += _strRelativePath;

	if (FAILED(pRes->Load(strFilePath, _iMapLevel)))
	{
		return nullptr;
	}

	pRes->SetKey(_strKey);
	pRes->SetRelativePath(_strRelativePath);

	m_arrRes[(UINT)RES_TYPE::TEXTURE].insert(make_pair(_strKey, pRes));

	return pRes;
}