#include "pch.h"
#include "CMissileScript.h"

CMissileScript::CMissileScript()
	: CScript(MISSILESCRIPT)
	, m_fSpeed(200.f)
{
}

CMissileScript::~CMissileScript()
{
}

void CMissileScript::tick()
{
	Vec3 vPos = Transform()->GetRelativePos();

	vPos.y += m_fSpeed * DT;

	Transform()->SetRelativePos(vPos);
}