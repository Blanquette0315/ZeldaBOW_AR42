#include "pch.h"
#include "CNavTestSCR.h"

#include <Engine/CNavMgr.h>
#include <Engine/CTransform.h>

CNavTestSCR::CNavTestSCR()
	: CScript(SCRIPT_TYPE::NAVTESTSCR)
{
	m_fSpeed = 300.f;
	m_bOnce = true;
}

CNavTestSCR::CNavTestSCR(const CNavTestSCR& _origin)
	: CScript(_origin)
{
}

CNavTestSCR::~CNavTestSCR()
{
}

void CNavTestSCR::tick()
{
	static Vec3 startpos = { 118.655f, 69.3553f, 140.f };
	// static Vec3 endpos = { 50.f, -1.78814e-05f, 159.9f };
	static Vec3 endpos = { -118.555f, 70.1553f, 40.f };
	static Vec3 startposMul10 = startpos * 10.f;
	static Vec3 endposMul10 = endpos * 10.f;

	if (m_bOnce)
	{
		CNavMgr::GetInst()->init(L"mesh\\all_tiles_navmesh.bin");
		m_bOnce = false;
		Transform()->SetRelativePos(startpos * 10.f);
	}

	Vec3 vPos = Transform()->GetRelativePos();
	Vec3 vDir = Vec3::Zero;
	if (!CNavMgr::GetInst()->FindNextMoveDir(Transform()->GetRelativePos(), endposMul10, vDir))
	{
		return;
	}
	Transform()->SetRelativePos(vPos + vDir * m_fSpeed * FDT);
}

// 118.655 69.3553 140
// -118.555 70.1553 40
// 50 -1.78814e-05 159.9
