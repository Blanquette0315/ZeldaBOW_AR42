#pragma once
#include "singleton.h"

class CEventMgr
	: public CSingleton<CEventMgr>
{
private:
	vector<tEvent>			m_vecEvent;

	vector<CGameObject*>	m_vecGarbage;

	LEVEL_STATE				m_eLevelState;
	bool					m_bLevelStateChange;

	bool					m_bLevelChanged;

public:
	void tick();

public:
	void AddEvent(tEvent& _evn) { m_vecEvent.push_back(_evn); }

	bool IsLevelChanged() { return m_bLevelChanged; }

public:
	CEventMgr();
	~CEventMgr();
};