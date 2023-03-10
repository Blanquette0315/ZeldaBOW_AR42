#pragma once

class PhysEngine;
class PhysData;

class CPhysMgr
	: public CSingleton<CPhysMgr>
{
private:
	PhysEngine* m_pPhys;
	float		m_fFrame;

	PhysData* m_pData;

public:
	void init();
	void tick();

public:
	CPhysMgr();
	~CPhysMgr();
};

