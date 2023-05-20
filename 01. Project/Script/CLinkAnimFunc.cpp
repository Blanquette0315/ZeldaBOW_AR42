#include "pch.h"
#include "CLinkAnimScript.h"

#include <Engine/CTransform.h>
#include <Engine/CRigidbody.h>
#include <Engine/CAnimation3D.h>
#include <Engine/CPhysMgr.h>

#include "CLinkScript.h"
#include "CLockOnScript.h"
#include "FSMNode.h"
#include "CBonesocketScript.h"
#include "CMonsterScript.h"
#include "CLinkBombScript.h"


bool CLinkAnimScript::MoveRotation(Vec3 _vDir)
{
	if (_vDir == Vec3::Zero)
		return true;

	Vec3 vCross = Vec3(0.f, 0.f, -1.f).Cross(_vDir);
	float fRad = acosf(Vec3(0.f, 0.f, -1.f).Dot(_vDir));

	// directx is left-handed axis
	if (vCross.y < 0.f)
	{
		fRad *= -1.f;
	}
	float fAngleSpeedPerFrame = m_fAnglePerSec * FDT;

	Vec3 vRot = Transform()->GetRelativeRotation();
	Vec3 vRotDir = Vec3(sinf(vRot.y + XM_PI), 0.f, cosf(vRot.y + XM_PI));
	vCross = vRotDir.Cross(_vDir);
	float fAdd = fRad - vRot.y;
	fAdd = fAdd - XM_2PI * floorf(fAdd / XM_2PI);

	if (fAdd < 0 && fabsf(fAdd) > XM_PI)
		fAdd += XM_2PI;
	else if (fAdd > 0 && fabsf(fAdd) > XM_PI)
		fAdd -= XM_2PI;

	if (vCross.y < 0.f)
		fAngleSpeedPerFrame *= -1.f;

	bool bRotFinish = false;
	if (fabsf(fAdd) <= fabsf(fAngleSpeedPerFrame))
	{
		fAngleSpeedPerFrame = fAdd;
		bRotFinish = true;
	}

	Transform()->AddRelativeRotation(Vec3(0.f, fAngleSpeedPerFrame, 0.f));
	return bRotFinish;
}

void CLinkAnimScript::MoveToDir(DIR _eDir, bool _bReverse)
{
	Vec3 vDir = GetCombinedDir();
	if (vDir == Vec3::Zero)
		return;

	vDir = Transform()->GetRelativeDir(_eDir);
	if (_bReverse)
	{
		vDir *= -1;
	}

	RigidBody()->SetVelocity(vDir * m_fSelectedSpeed);
	/*if (IsCurAnim(LAT_DASH))
		RigidBody()->SetVelocity(vDir * m_fDashSpeed);
	else if (m_iMode == (UINT)LINK_MODE::LINK_MODE_WALK)
		RigidBody()->SetVelocity(vDir * m_fWalkSpeed);
	else if (m_iMode == (UINT)LINK_MODE::LINK_MODE_RUN)
		RigidBody()->SetVelocity(vDir * m_fRunSpeed);*/
}

void CLinkAnimScript::MoveToDirAdd(DIR _eDir, bool _bReverse)
{
	Vec3 vDir = GetCombinedDir();
	if (vDir == Vec3::Zero)
		return;

	vDir = Transform()->GetRelativeDir(_eDir);
	if (_bReverse)
	{
		vDir *= -1;
	}

	RigidBody()->AddVelocity(vDir * m_fSelectedSpeed);
}

void CLinkAnimScript::SelectSpeed()
{
		if (IsCurAnim(LAT_DASH))
			m_fSelectedSpeed = m_fDashSpeed;
		else if (CalBit(m_iMode, LINK_MODE_WALK, BIT_LEAST_ONE))
			m_fSelectedSpeed = m_fWalkSpeed;
		else if (CalBit(m_iMode, LINK_MODE_RUN, BIT_LEAST_ONE))
			m_fSelectedSpeed = m_fRunSpeed;
}

void CLinkAnimScript::Func_WalkRunDash()
{
	MoveRotation(GetCombinedDir());
	MoveToDir(DIR::FRONT);
}

void CLinkAnimScript::Func_LockOnMove()
{
	CGameObject* pLockOnTarget = m_pLockOnRadar->GetLockOnTarget();

	// x^2 + y^2 = r^2 -> (x1, y1) tangent : x1 * x + y1 * y = r^2
	Vec3 vCoord = pLockOnTarget->Transform()->GetRelativePos() - Transform()->GetRelativePos();
	float fGradient = -vCoord.x / vCoord.z;
	Vec3 vDirMove = Vec3::Zero;
	Vec3 vDirLinkToLockOn = (vCoord - Vec3(0.f, vCoord.y, 0.f)).Normalize();

	vDirMove = Vec3(vCoord.z, 0.f, -vCoord.x).Normalize();

	// First -> Turn
	if (!m_bLockOnRotFinish)
	{
		if (MoveRotation(vDirLinkToLockOn))
			m_bLockOnRotFinish = true;
	}
	else
	{
		RigidBody()->SetVelocity(Vec3::Zero);
		if (KEY_PRESSED(KEY::W))
		{
			MoveToDirAdd(DIR::FRONT);
		}
		else if (KEY_PRESSED(KEY::S))
		{
			MoveToDirAdd(DIR::FRONT, true);
		}

		if (KEY_PRESSED(KEY::A))
		{
			RigidBody()->AddVelocity(-vDirMove * m_fSelectedSpeed);
			MoveRotation(vDirLinkToLockOn);
		}
		else if (KEY_PRESSED(KEY::D))
		{
			RigidBody()->AddVelocity(vDirMove * m_fSelectedSpeed);
			MoveRotation(vDirLinkToLockOn);
		}
	}
}

void CLinkAnimScript::Func_TurnBack()
{
	Transform()->AddRelativeRotation(Vec3(0.f, XM_PI, 0.f));
}

void CLinkAnimScript::Func_Jump()
{
	RigidBody()->AddVelocity(Vec3(0.f, m_fJumpSpeed, 0.f));
}

void CLinkAnimScript::Func_LowerBodyBlend()
{
	if (CalBit(m_iCond, LAC_KEY_WSAD, BIT_LEAST_ONE))
	{
		if (CalBit(m_iMode, LINK_MODE_LOCKON, BIT_LEAST_ONE) || CalBit(m_pCurAnimNode->iPreferences, LAP_BOW_CHARGE, BIT_LEAST_ONE))
		{
			if (CalBit(m_iMode, LINK_MODE_WALK, BIT_LEAST_ONE))
			{
				if (KEY_PRESSED(KEY::W))
				{
					if (!IsCurAnimLower(LAT_LOCKON_WALK_F))
					{
						SetAnimNode(m_pCurAnimNodeLower, LAT_LOCKON_WALK_F);
						m_pAnimator->PlayLowerAnim(LINK_ANIM_WCHAR[LAT_LOCKON_WALK_F], true);
					}
				}
				else if (KEY_PRESSED(KEY::S))
				{
					if (!IsCurAnimLower(LAT_LOCKON_WALK_B))
					{
						SetAnimNode(m_pCurAnimNodeLower, LAT_LOCKON_WALK_B);
						m_pAnimator->PlayLowerAnim(LINK_ANIM_WCHAR[LAT_LOCKON_WALK_B], true);
					}
				}
				else if (KEY_PRESSED(KEY::A))
				{
					if (!IsCurAnimLower(LAT_LOCKON_WALK_L))
					{
						SetAnimNode(m_pCurAnimNodeLower, LAT_LOCKON_WALK_L);
						m_pAnimator->PlayLowerAnim(LINK_ANIM_WCHAR[LAT_LOCKON_WALK_L], true);
					}
				}
				else if (KEY_PRESSED(KEY::D))
				{
					if (!IsCurAnimLower(LAT_LOCKON_WALK_R))
					{
						SetAnimNode(m_pCurAnimNodeLower, LAT_LOCKON_WALK_R);
						m_pAnimator->PlayLowerAnim(LINK_ANIM_WCHAR[LAT_LOCKON_WALK_R], true);
					}
				}
			}
			else if (CalBit(m_iMode, LINK_MODE_RUN, BIT_LEAST_ONE))
			{
				if (KEY_PRESSED(KEY::W))
				{
					if (!IsCurAnimLower(LAT_LOCKON_RUN_F))
					{
						SetAnimNode(m_pCurAnimNodeLower, LAT_LOCKON_RUN_F);
						m_pAnimator->PlayLowerAnim(LINK_ANIM_WCHAR[LAT_LOCKON_RUN_F], true);
					}
				}
				else if (KEY_PRESSED(KEY::S))
				{
					if (!IsCurAnimLower(LAT_LOCKON_RUN_B))
					{
						SetAnimNode(m_pCurAnimNodeLower, LAT_LOCKON_RUN_B);
						m_pAnimator->PlayLowerAnim(LINK_ANIM_WCHAR[LAT_LOCKON_RUN_B], true);
					}
				}
				else if (KEY_PRESSED(KEY::A))
				{
					if (!IsCurAnimLower(LAT_LOCKON_RUN_L))
					{
						SetAnimNode(m_pCurAnimNodeLower, LAT_LOCKON_RUN_L);
						m_pAnimator->PlayLowerAnim(LINK_ANIM_WCHAR[LAT_LOCKON_RUN_L], true);
					}
				}
				else if (KEY_PRESSED(KEY::D))
				{
					if (!IsCurAnimLower(LAT_LOCKON_RUN_R))
					{
						SetAnimNode(m_pCurAnimNodeLower, LAT_LOCKON_RUN_R);
						m_pAnimator->PlayLowerAnim(LINK_ANIM_WCHAR[LAT_LOCKON_RUN_R], true);
					}
				}
			}
		}
		else
		{
			if (CalBit(m_iMode, LINK_MODE_WALK, BIT_LEAST_ONE) && !IsCurAnimLower(LAT_WALK))
			{
				SetAnimNode(m_pCurAnimNodeLower, LAT_WALK);
				m_pAnimator->PlayLowerAnim(LINK_ANIM_WCHAR[LAT_WALK], true);
			}
			else if (CalBit(m_iMode, LINK_MODE_RUN, BIT_LEAST_ONE) && !IsCurAnimLower(LAT_RUN))
			{
				SetAnimNode(m_pCurAnimNodeLower, LAT_RUN);
				m_pAnimator->PlayLowerAnim(LINK_ANIM_WCHAR[LAT_RUN], true);
			}
		}
	}
	else
	{
		m_pCurAnimNodeLower = nullptr;
		m_pAnimator->StopLowerAnim();
	}


	if (CalBit(m_iMode, LINK_MODE_LOCKON, BIT_LEAST_ONE))
	{
		Func_LockOnMove();
	}
	else if (CalBit(m_pCurAnimNode->iPreferences, LAP_BOW_CHARGE, BIT_LEAST_ONE))
	{
		Func_BowChargeMove();
	}
	else
	{
		Func_WalkRunDash();
	}
}

void CLinkAnimScript::Func_SwordRun()
{
	MoveRotation(GetCombinedDir());
	m_fSelectedSpeed = m_fRunSpeed;
	MoveToDir(DIR::FRONT);
}

void CLinkAnimScript::Func_BowRun()
{
	MoveRotation(GetCombinedDir());
	m_fSelectedSpeed = m_fRunSpeed;
	MoveToDir(DIR::FRONT);
}

void CLinkAnimScript::Func_BowEquipOn()
{
	CBonesocketScript* pBoneScr = m_pBowObj->GetScript<CBonesocketScript>();
	pBoneScr->setBoneIdx((UINT)LINK_BONE_STRING::Weapon_L);
	pBoneScr->ClearOffset();

	m_bArrEquip[(UINT)EQUIPMENT_STATE::BOW] = true;
}

void CLinkAnimScript::Func_BowEquipOff()
{
	CBonesocketScript* pBoneScr = m_pBowObj->GetScript<CBonesocketScript>();
	pBoneScr->setBoneIdx((UINT)LINK_BONE_STRING::Pod_A);
	pBoneScr->setOffsetPos(Vec3(0.f, 0.f, -0.07f));
	pBoneScr->setOffsetRot(Vec3(0.f, 0.f, -45.f));

	m_bArrEquip[(UINT)EQUIPMENT_STATE::BOW] = false;
}

void CLinkAnimScript::Func_SwordAttackMove()
{
	Vec3 vDir = Transform()->GetRelativeDir(DIR::FRONT);
	RigidBody()->SetVelocity(vDir * m_fWalkSpeed);
}

void CLinkAnimScript::Func_SwordEquipOn()
{
	CBonesocketScript* pBoneScr = m_pSwordObj->GetScript<CBonesocketScript>();
	pBoneScr->setBoneIdx((UINT)LINK_BONE_STRING::Weapon_R);
	pBoneScr->ClearOffset();
	
	m_bArrEquip[(UINT)EQUIPMENT_STATE::SWORD] = true;

	pBoneScr = m_pShieldObj->GetScript<CBonesocketScript>();
	pBoneScr->setBoneIdx((UINT)LINK_BONE_STRING::Weapon_L);
	pBoneScr->ClearOffset();
}

void CLinkAnimScript::Func_SwordEquipOff()
{
	CBonesocketScript* pBoneScr = m_pSwordObj->GetScript<CBonesocketScript>();
	pBoneScr->setBoneIdx((UINT)LINK_BONE_STRING::Pod_A);
	pBoneScr->setOffsetPos(Vec3(0.f, 0.f, 0.f));
	pBoneScr->setOffsetRot(Vec3(0.f, 0.f, 60.f));

	m_bArrEquip[(UINT)EQUIPMENT_STATE::SWORD] = false;

	pBoneScr = m_pShieldObj->GetScript<CBonesocketScript>();
	pBoneScr->setBoneIdx((UINT)LINK_BONE_STRING::Pod_A);
	pBoneScr->setOffsetPos(Vec3(-0.2f, 0.08f, -0.04f));
	pBoneScr->setOffsetRot(Vec3(0.f, 0.f, 0.f));
}

void CLinkAnimScript::Func_BowChargeMove()
{
	if (KEY_PRESSED(KEY::W))
	{
		MoveToDir(DIR::FRONT);
	}
	else if (KEY_PRESSED(KEY::S))
	{
		MoveToDir(DIR::FRONT, true);
	}

	if (KEY_PRESSED(KEY::A))
	{
		MoveToDir(DIR::RIGHT, true);
	}
	else if (KEY_PRESSED(KEY::D))
	{
		MoveToDir(DIR::RIGHT);
	}
}



void CLinkAnimScript::Func_ShieldJustStart()
{
	m_fParryingAccTime = 0.f;
	m_bParryingOnce = false;
}

void CLinkAnimScript::Func_ShieldJust()
{
	if (m_fParryingMaxTime < m_fParryingAccTime)
	{
		return;
	}
	else
	{
		if (m_bInJustRigid && m_pInJustRigidObj && m_bParryingOnce == false)
		{
			// time slow
			TimeSlow(true, 3.f);
			m_bParryingOnce = true;
			m_bInvincible = true;

			// monster stun
			m_pInJustRigidObj->GetScript<CMonsterScript>()->Parrying();
		}

		m_fParryingAccTime += FDT;
	}
}

void CLinkAnimScript::Func_ShieldJustEnd()
{
	// m_bInJustRigid = false;
	m_bInvincible = false;
	// m_pInJustRigidObj = nullptr;
	TimeSlow(false);
}

void CLinkAnimScript::Func_ShieldGuard()
{
	m_bArrEquip[(UINT)EQUIPMENT_STATE::SHIELD] = true;
	m_bArrEquip[(UINT)EQUIPMENT_STATE::SWORD] = false;
}

void CLinkAnimScript::Func_SwordLockOnWait()
{
	m_bArrEquip[(UINT)EQUIPMENT_STATE::SWORD] = true;
	m_bArrEquip[(UINT)EQUIPMENT_STATE::SHIELD] = false;
}

void CLinkAnimScript::Func_JustEvasionStart()
{
	// jump back
	Vec3 vDir = -Transform()->GetRelativeDir(DIR::FRONT);
	vDir *= 0.5f;
	vDir.y = m_fJumpSpeed;
	RigidBody()->SetVelocity(vDir);

	m_fEvasionAccTime = 0.f;
	m_bEvasionOnce = false;
}

void CLinkAnimScript::Func_JustEvasion()
{
	if (m_fEvasionMaxTime < m_fEvasionAccTime)
	{
	}
	else
	{
		if (m_bInJustRigid && m_pInJustRigidObj && m_bEvasionOnce == false)
		{
			// time slow
			TimeSlow(true, 3.f);
			CPhysMgr::GetInst()->SetPhysSlow(true);
			m_bEvasionOnce = true;
			m_bInvincible = true;	
			m_bCanJustAttackStart = true;
		}
		else
		{
			m_fEvasionAccTime += FDT;
		}
	}
	int i = m_pCurAnimNode->pAnim->GetCurFrame();
	if (m_bEvasionOnce && m_pCurAnimNode->pAnim->GetCurFrame() > 1367 )
	{
		TimeSlowAffectedObj(false, GetOwner());
		CPhysMgr::GetInst()->SetPhysSlow(false);
	}
}

void CLinkAnimScript::Func_JustEvasionEnd()
{
	if (m_bEvasionOnce)
	{

	}
}

void CLinkAnimScript::Func_JustAtkStart()
{

}

void CLinkAnimScript::Func_JustAtkDash()
{
	CGameObject* pLockOnObj = m_pLockOnRadar->GetLockOnTarget();
	if (pLockOnObj && CalBit(m_iMode, LINK_MODE_LOCKON, BIT_LEAST_ONE))
	{
		Vec3 vTargetPos = pLockOnObj->Transform()->GetRelativePos();
		Vec3 vPos = Transform()->GetRelativePos();
		float fLength = (vTargetPos - vPos).Length();
		Vec3 vDir = (vTargetPos - vPos);
		vDir.y = 0.f;
		vDir.Normalize();

		RigidBody()->SetVelocity(vDir * fLength);
	}
}

void CLinkAnimScript::Func_DisableCanJust()
{
	m_bCanJustAttackStart = false;
}

void CLinkAnimScript::Func_JustAtkEnd()
{
	TimeSlowAffectedObj(true, GetOwner());
	TimeSlow(false);
	m_bInvincible = false;

	// monster set state 
	// if(m_pLockOnRadar->GetLockOnTarget())
		// m_pLockOnRadar->GetLockOnTarget()->GetScript<CMonsterScript>()->Parrying();
}

void CLinkAnimScript::Func_CreateBomb()
{
	if (m_pBombPref.Get() == nullptr)
		return;

	if (m_pBombObj)
	{
		m_pBombObj->Destroy();
		m_pBombObj = nullptr;
	}

	m_pBombObj = m_pBombPref->Instantiate();
	m_pBombObj->GetScript<CBonesocketScript>()->setBoneIdx((UINT)LINK_BONE_STRING::Weapon_R);
	AddChild(GetOwner(), m_pBombObj);
}

void CLinkAnimScript::Func_ThrowBombStart()
{
	MakeParentReserve(m_pBombObj);
	m_pBombObj->GetScript<CBonesocketScript>()->SetDisable(true);
	Vec3 vDir = Transform()->GetRelativeDir(DIR::FRONT);
	vDir.y = tanf(XM_PI / 6.f);
	vDir.Normalize();
	m_pBombObj->GetScript<CLinkBombScript>()->SetDir(vDir);
	m_pBombObj = nullptr;
}
