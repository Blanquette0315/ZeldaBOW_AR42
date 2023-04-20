#include "pch.h"
#include "CPhysMgr.h"

#include <PhysEngine/Export/PhysX_API.h>

#include "CKeyMgr.h"
#include "CTimeMgr.h"
#include "CLevelMgr.h"

#include "CLevel.h"
#include "CRigidBody.h"

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

	if (CLevelMgr::GetInst()->GetLevelState() == LEVEL_STATE::PLAY)
	{
		vector<CGameObject*> vecGameObj = CLevelMgr::GetInst()->GetCurLevel()->GetGameObjects();
		for (size_t i = 0; i < vecGameObj.size(); ++i)
		{
			if (nullptr != vecGameObj[i]->RigidBody())
				vecGameObj[i]->RigidBody()->UpdatePhysResult();
		}
	}
}