#pragma once

class PhysEngine;
class PhysData;

class CPhysMgr
	: public CSingleton<CPhysMgr>
{
private:
	PhysEngine* m_pPhys;

public:
	void init();
	void tick();

public:
	CPhysMgr();
	~CPhysMgr();
};

