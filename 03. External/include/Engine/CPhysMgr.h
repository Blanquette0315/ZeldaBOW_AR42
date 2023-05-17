#pragma once

class PhysEngine;
class PhysData;

class CPhysMgr
	: public CSingleton<CPhysMgr>
{
private:
	PhysEngine* m_pPhys;

	bool		m_bPhysSlow;

public:
	void SetPhysSlow(bool _b) { m_bPhysSlow = _b; }
	bool GetPhysSlow() { return m_bPhysSlow; }

public:
	void init();
	void tick();

public:
	CPhysMgr();
	~CPhysMgr();
};

