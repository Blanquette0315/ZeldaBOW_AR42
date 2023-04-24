#pragma once

class CLevel;

class CLevelMgr
	: public CSingleton<CLevelMgr>
{
private:
	CLevel*					m_pCurLevel;

public:
	CLevel* GetCurLevel() { return m_pCurLevel; }
	LEVEL_STATE GetLevelState();

public:
	CGameObject* FindObjectByName(const wstring& _name);
	void FindObjectByName(const wstring& _name, vector<CGameObject*>& _out);

private:
	void ChangeLevelState(LEVEL_STATE _State);
	void ChangeLevel(CLevel* _NextLevel);

public:
	void init();
	void refresh();	// Clear CurLevel All Layer's Vector & Register Object into Layer's Vector
	void progress();

public:
	CLevelMgr();
	~CLevelMgr();

	friend class CEventMgr;
};

