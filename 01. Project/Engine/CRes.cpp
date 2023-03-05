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
	// ���� �����̸�, �ش� ��θ� �������ش�.
	// ���ҽ��� �̸��� �ٲ���ٸ� �̸��� �°� ��θ� �ٲپ� �������־�� �Ѵ�.
	if (m_strRelativePath.empty() || m_bChangeName)
	{
		m_strRelativePath = _strRelativePath;
		return true;
	}
	// �ѹ� �̻� ���Ͽ� ����� ���� ������ false�� ��ȯ
	else
	{
		// ���� ��ο� �� ������ �����ϰ� ���ش�.
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