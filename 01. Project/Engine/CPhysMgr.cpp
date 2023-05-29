#include "pch.h"
#include "CPhysMgr.h"

#include <PhysEngine/Export/PhysX_API.h>

#include "CKeyMgr.h"
#include "CTimeMgr.h"
#include "CLevelMgr.h"

#include "CLevel.h"

#include "CTransform.h"
#include "CRigidBody.h"
#include "CCollider.h"


CPhysMgr::CPhysMgr()
	: m_pPhys(nullptr)
{
}

CPhysMgr::~CPhysMgr()
{
}

void CPhysMgr::init()
{
#ifdef _DEBUG
	PhysX_Initialize(1, true);
#else
	PhysX_Initialize(1, false);
#endif
}

void CPhysMgr::tick()
{
	if (m_bPhysSlow)
	{
		PhysX_Update(FDT);
	}
	else
	{
		PhysX_Update(FDT_TICK);
	}
	vector<CGameObject*> vecGameObj = CLevelMgr::GetInst()->GetCurLevel()->GetGameObjects();

	for (size_t i = 0; i < vecGameObj.size(); ++i)
	{
		if (CLevelMgr::GetInst()->GetLevelState() == LEVEL_STATE::PLAY)
		{
			if (nullptr != vecGameObj[i]->LandScape())
				continue;
			if (nullptr != vecGameObj[i]->RigidBody())
				vecGameObj[i]->RigidBody()->UpdatePhysResult();
		}
		
		// == finaltick
		vecGameObj[i]->Transform()->finaltick();
		
		if (CLevelMgr::GetInst()->GetLevelState() == LEVEL_STATE::PLAY)
		{
			if (nullptr != vecGameObj[i]->Collider())
				vecGameObj[i]->Collider()->UpdateCollider();
		}
	}
}