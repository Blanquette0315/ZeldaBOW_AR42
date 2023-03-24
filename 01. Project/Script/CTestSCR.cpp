#include "pch.h"
#include "CTestSCR.h"

#include <Engine/CGameObject.h>
#include <Engine/CTransform.h>

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
