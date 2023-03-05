#pragma once



class CPathMgr
	: public CSingleton<CPathMgr>
{
private:
	wchar_t		m_szContentPath[256];

public:
	void init();

	const wchar_t* GetContentPath() { return m_szContentPath; }

public:
	CPathMgr();
	~CPathMgr();
};

