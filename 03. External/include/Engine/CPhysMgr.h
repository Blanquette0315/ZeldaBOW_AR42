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
	PhysData* m_pData2;

public:
	void init();
	void tick();

public:
	CPhysMgr();
	~CPhysMgr();
};

