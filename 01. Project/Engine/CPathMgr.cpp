#include "pch.h"
#include "CPathMgr.h"

CPathMgr::CPathMgr()
{

}

CPathMgr::~CPathMgr()
{

}

void CPathMgr::init()
{
	// 실행경로 가져오기
	GetCurrentDirectory(256, m_szContentPath);

	// 상위폴더로 변경
	size_t iLen = wcsnlen_s(m_szContentPath, 256);
	for (size_t i = iLen; i > 0; --i)
	{
		if (L'\\' == m_szContentPath[i])
		{
			m_szContentPath[i + 1] = L'\0';
			break;
		}

		continue;
	}

	// + L"bin\\content"
	wcscat_s(m_szContentPath, L"bin\\content\\");
}
