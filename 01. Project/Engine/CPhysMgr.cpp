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
	PhysX_Initialize(1, true);
}

void CPhysMgr::tick()
{
	PhysX_Update(FDT);
	vector<CGameObject*> vecGameObj = CLevelMgr::GetInst()->GetCurLevel()->GetGameObjects();

	for (size_t i = 0; i < vecGameObj.size(); ++i)
	{
		if (CLevelMgr::GetInst()->GetLevelState() == LEVEL_STATE::PLAY)
		{
			if (nullptr != vecGameObj[i]->RigidBody())
				vecGameObj[i]->RigidBody()->UpdatePhysResult();
		}

		//vecGameObj[i]->Transform()->UpdateWorldTrans();

		//if (CLevelMgr::GetInst()->GetLevelState() == LEVEL_STATE::PLAY)
		//{
		//	if (nullptr != vecGameObj[i]->Collider())
		//		vecGameObj[i]->Collider()->UpdateCollider();
		//}
	}

}