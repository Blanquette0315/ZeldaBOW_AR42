#include "pch.h"
#include "CPhysMgr.h"

#include <PhysEngine/Export/PhysX_API.h>

#include "CKeyMgr.h"
#include "CTimeMgr.h"

CPhysMgr::CPhysMgr()
	: m_pPhys(nullptr)
{
}

CPhysMgr::~CPhysMgr()
{
}

void CPhysMgr::init()
{
	PhysX_Initialize(1, true);
}

void CPhysMgr::tick()
{
	PhysX_Update(FDT);
}