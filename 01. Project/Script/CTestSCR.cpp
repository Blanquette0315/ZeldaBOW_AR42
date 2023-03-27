#include "pch.h"
#include "CTestSCR.h"

#include <Engine/CGameObject.h>
#include <Engine/CTransform.h>
#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>
#include <Engine/CRenderMgr.h>
#include <Engine/CCamera.h>
#include <Engine/CLayer.h>

void CTestSCR::tick()
{
	if(KEY_PRESSED(KEY::U))
	{
		Vec3 vPos = Transform()->GetWorldPos();
		//vPos.x += 10.f;
		//vPos.y += 10.f;
		vPos.z += 10.f;
		Transform()->SetWorldPos(vPos);
	}

	if (KEY_PRESSED(KEY::Y))
	{
		Vec3 vPos = Transform()->GetWorldPos();
		//vPos.x -= 10.f;
		//vPos.y -= 10.f;
		vPos.z -= 10.f;
		Transform()->SetWorldPos(vPos);
	}


	if (KEY_TAP(KEY::RBTN))
	{
		//CLevel* pLevel = CLevelMgr::GetInst()->GetCurLevel();
		//const vector<CGameObject*>& vecObj = pLevel->GetLayer(0)->GetParentObject();
		//CGameObject* pObjOut = nullptr;
		//CTransform::ClearBoundingBoxDist();
		//
		//for (size_t i = 0; i < vecObj.size(); ++i)
		//{
		//	CGameObject* pTmp = nullptr;
		//	pTmp = vecObj[i]->Transform()->CheckRay(CRenderMgr::GetInst()->GetMainCam()->GetRay());
		//	if (pTmp)
		//	{
		//		pObjOut = pTmp;
		//	}
		//}
		//CGameObject* pTmp = nullptr;

		//if (PickingObj(pTmp))
		//{
		//	int test = 1;
		//}
	}
}

CTestSCR::CTestSCR()
	: CScript((int)SCRIPT_TYPE::TESTSCR)
{
}

CTestSCR::CTestSCR(const CTestSCR& _origin)
	: CScript(_origin)
{
}

CTestSCR::~CTestSCR()
{
}
