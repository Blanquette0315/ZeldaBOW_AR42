#pragma once

class CLevel;

class CLevelMgr
	: public CSingleton<CLevelMgr>
{
private:
	CLevel*					m_pCurLevel;
	map<wstring, CLevel*>	m_mapLevel;

public:
	CLevel* GetCurLevel() { return m_pCurLevel; }
	LEVEL_STATE GetLevelState();

public:
	CGameObject* FindObjectByName(const wstring& _name);
	void FindObjectByName(const wstring& _name, vector<CGameObject*>& _out);

	void RegisterLevel(const wstring& _strKey, CLevel* _level);
	void DeRegisterLevel(const wstring& _strKey);
	CLevel* FindLevel(const wstring& _strKey);

	const map<wstring, CLevel*>& GetLevels() { return m_mapLevel; }

private:
	void ChangeLevelState(LEVEL_STATE _State);
	void ChangeLevel(CLevel* _NextLevel);

public:
	void init();
	void progress();

public:
	CLevelMgr();
	~CLevelMgr();

	friend class CEventMgr;
};

