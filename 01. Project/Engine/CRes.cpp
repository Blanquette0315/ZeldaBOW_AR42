#include "pch.h"
#include "CRes.h"

CRes::CRes(RES_TYPE _eResType, bool _bEngineRes)
	: m_eResType(_eResType)
	, m_iRefCount(0)
	, m_bEngineRes(_bEngineRes)
	, m_bChangeName(false)
{
}

CRes::CRes(const CRes& _other)
	: CEntity(_other)
	, m_eResType(_other.m_eResType)
	, m_iRefCount(0)
	, m_bEngineRes(false)
	, m_bChangeName(false)
{
}

CRes::~CRes()
{
	assert(!(0 < m_iRefCount));
}


bool CRes::CheckRelativePath(const wstring& _strRelativePath)
{
	// 최조 저장이면, 해당 경로를 저장해준다.
	// 리소스의 이름이 바뀌었다면 이름에 맞게 경로를 바꾸어 저쟝해주어야 한다.
	if (m_strRelativePath.empty() || m_bChangeName)
	{
		m_strRelativePath = _strRelativePath;
		return true;
	}
	// 한번 이상 파일에 저장된 적이 있으면 false를 반환
	else
	{
		// 같은 경로에 재 저장은 가능하게 해준다.
		if (m_strRelativePath == _strRelativePath)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

void CRes::SaveKeyPath(FILE* _pFile)
{
	SaveWStringToFile(m_strKey, _pFile);
	SaveWStringToFile(m_strRelativePath, _pFile);
}

void CRes::LoadKeyPath(FILE* _pFile)
{
	LoadWStringFromFile(m_strKey, _pFile);
	LoadWStringFromFile(m_strRelativePath, _pFile);
}