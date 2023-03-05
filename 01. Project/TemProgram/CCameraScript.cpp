#include "pch.h"
#include "CCameraScript.h"


CCameraScript::CCameraScript()
	: CScript(-1)
	, m_fSpeed(100.f)
	, m_bCamMove(false)
{
}

CCameraScript::~CCameraScript()
{
}

void CCameraScript::begin()
{
}

void CCameraScript::tick()
{
	//if (m_bCamMove)
	//{
	//	Move();
	//}
	Move();
}

void CCameraScript::Move()
{
	Vec3 vPos = Transform()->GetRelativePos();

	float fSpeed = m_fSpeed;

	if (KEY_PRESSED(KEY::LSHIFT))
	{
		fSpeed *= 5.f;
	}

	if (Camera()->GetProjType() == PROJ_TYPE::PERSPECTIVE)
	{
		Vec3 vFront = Transform()->GetRelativeDir(DIR::FRONT);
		Vec3 vRight = Transform()->GetRelativeDir(DIR::RIGHT);

		if (KEY_PRESSED(KEY::W))
			vPos += DT * vFront * fSpeed;
		if (KEY_PRESSED(KEY::S))
			vPos += DT * -vFront * fSpeed;
		if (KEY_PRESSED(KEY::A))
			vPos += DT * -vRight * fSpeed;
		if (KEY_PRESSED(KEY::D))
			vPos += DT * vRight * fSpeed;

		Vec2 vMouseDir = CKeyMgr::GetInst()->GetMouseDir();

		if (KEY_PRESSED(KEY::RBTN))
		{
			Vec3 vRot = Transform()->GetRelativeRotation();

			vRot.y += vMouseDir.x * DT * XM_PI;
			vRot.x -= vMouseDir.y * DT * XM_PI;
			Transform()->SetRelativeRotation(vRot);
		}

		Transform()->SetRelativePos(vPos);
	}

	else
	{
		Vec3 vRot = Transform()->GetRelativeRotation();
		if (vRot.x != 0.f)
			vRot.x = 0.f;
		if (vRot.y != 0.f)
			vRot.y = 0.f;

		if (KEY_PRESSED(KEY::W))
			vPos.y += DT * fSpeed;
		if (KEY_PRESSED(KEY::S))
			vPos.y -= DT * fSpeed;
		if (KEY_PRESSED(KEY::A))
			vPos.x += DT * fSpeed;
		if (KEY_PRESSED(KEY::D))
			vPos.x -= DT * fSpeed;

		if (vPos != Transform()->GetRelativePos())
			Transform()->SetRelativePos(vPos);

		if (KEY_PRESSED(KEY::N_1))
		{
			float fScale = Camera()->GetOrthographicScale();
			fScale += DT;
			Camera()->SetOrthographicScale(fScale);
		}
		else if (KEY_PRESSED(KEY::N_2))
		{
			float fScale = Camera()->GetOrthographicScale();
			fScale -= DT;
			Camera()->SetOrthographicScale(fScale);
		}
	}
}