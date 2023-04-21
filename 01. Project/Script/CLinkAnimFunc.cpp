#include "pch.h"
#include "CLinkAnimScript.h"

#include <Engine/CTransform.h>
#include <Engine/CRigidbody.h>

#include "CLinkScript.h"
#include "FSMNode.h"



void CLinkAnimScript::SetRotation()
{
	Transform()->finaltick();

	Vec3 vCross = Transform()->GetRelativeDir(DIR::FRONT).Cross(Transform()->GetWorldDir(DIR::FRONT));
	float fRad = acos(Transform()->GetRelativeDir(DIR::FRONT).Dot(Transform()->GetWorldDir(DIR::FRONT)));

	if (vCross.y == 1.f)
	{
		fRad *= -1.f;
	}

	Transform()->SetRelativeRotation(Vec3(0.f, fRad, 0.f));
}

void CLinkAnimScript::Func_WalkRunDash()
{
	Vec3 vDir = GetCombinedDir();
	

	Vec3 vCross = Vec3(0.f, 0.f, -1.f).Cross(vDir);
	float fRad = acosf(Vec3(0.f, 0.f, -1.f).Dot(vDir));

	// directx is left-handed axis
	if (vCross.y < 0.f)
	{
		fRad *= -1.f;
	}
	float fAngleSpeedPerFrame = m_fAnglePerSec / (1.f / FDT);

	Vec3 vRot = Transform()->GetRelativeRotation();
	Vec3 vRotDir = Vec3(sinf(vRot.y + XM_PI), 0.f, cosf(vRot.y + XM_PI));
	vCross = vRotDir.Cross(vDir);
	float fAdd = fRad - vRot.y;

	if (vCross.y < 0.f)
		fAngleSpeedPerFrame *= -1.f;

	if (fabsf(fAdd) <= fabsf(fAngleSpeedPerFrame))
		fAngleSpeedPerFrame = fAdd;

	//if (KEY_PRESSED(KEY::RBTN))
	//{
	//	Transform()->SetRelativeRotation(Vec3(0.f, fRad, 0.f));
	//	RigidBody()->SetVelocity(vDir);
	//}
	//else
	//{
		Transform()->AddRelativeRotation(Vec3(0.f, fAngleSpeedPerFrame, 0.f));
		//vDir = Vec3(sinf(vRot.y + fAngleSpeedPerFrame + XM_PI), 0.f, cosf(vRot.y + fAngleSpeedPerFrame + XM_PI));
		vDir = Transform()->GetRelativeDir(DIR::FRONT);
		if(IsCurAnim(LAT_DASH))
			RigidBody()->SetVelocity(vDir * m_fDashSpeed);
		else if(m_iMode == (UINT)LINK_MODE::WALK)
			RigidBody()->SetVelocity(vDir * m_fWalkSpeed);
		else if(m_iMode == (UINT)LINK_MODE::RUN)
			RigidBody()->SetVelocity(vDir * m_fRunSpeed);
	//}

}

void CLinkAnimScript::Func_TurnBack()
{
	Transform()->AddRelativeRotation(Vec3(0.f, XM_PI, 0.f));
}
