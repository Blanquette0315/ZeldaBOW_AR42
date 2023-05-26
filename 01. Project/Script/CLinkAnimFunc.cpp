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

	int iFrameIdx = m_pCurAnimNode->pAnim->GetCurFrame();

	if (m_fSelectedSpeed == m_fWalkSpeed)
	{
		if (iFrameIdx == 53)
		{
			
		}
		if (iFrameIdx == 53 + 12)
		{

		}
	}
	else if (m_fSelectedSpeed == m_fRunSpeed)
	{

	}
	else if (m_fSelectedSpeed == m_fDashSpeed)
	{

	}
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
	Func_JumpStartSound();
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

	Func_BowEquipSound();
}

void CLinkAnimScript::Func_BowEquipOff()
{
	if (m_pCurAnimNode->pAnim->GetCurFrame() == 707)
	{
		CBonesocketScript* pBoneScr = m_pBowObj->GetScript<CBonesocketScript>();
		pBoneScr->setBoneIdx((UINT)LINK_BONE_STRING::Pod_A);
		pBoneScr->setOffsetPos(Vec3(0.f, 0.f, -0.07f));
		pBoneScr->setOffsetRot(Vec3(0.f, 0.f, -45.f));

		m_bArrEquip[(UINT)EQUIPMENT_STATE::BOW] = false;
	}
}

void CLinkAnimScript::Func_SwordAttackMove()
{
	Vec3 vDir = Transform()->GetRelativeDir(DIR::FRONT);
	RigidBody()->SetVelocity(vDir * m_fWalkSpeed);

	Func_SwordSwingSound();
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

	Func_SwordEquipSound();
}

void CLinkAnimScript::Func_SwordEquipOff()
{
	if (m_pCurAnimNode->pAnim->GetCurFrame() == 317)
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

			Vec3 vInstPos = Transform()->GetRelativePos();
			vInstPos += Transform()->GetRelativeDir(DIR::FRONT) * m_fParryingOffset;
			vInstPos.y += 15.f;

			if (m_EffectParrying.Get())
			{
				CGameObject* pInstObj = m_EffectParrying->Instantiate();
				pInstObj->Transform()->SetRelativeRotation(Vec3(Transform()->GetRelativeRotation()));
				Instantiate(pInstObj, vInstPos, 0);
			}

			if (m_EffectScreenFlash.Get())
				Instantiate(m_EffectScreenFlash->Instantiate(), Vec3::Zero, 0);

			Func_JustGuardSound();
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
			TimeSlow(true, 10.f);
			CPhysMgr::GetInst()->SetPhysSlow(true);
			m_bEvasionOnce = true;
			m_bInvincible = true;	
			m_bCanJustAttackStart = true;
			Func_TimeSlowSound();
		}
		else
		{
			m_fEvasionAccTime += FDT;
		}
	}

	if (m_bEvasionOnce && m_pCurAnimNode->pAnim->GetCurFrame() > 1365 )
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

		RigidBody()->SetVelocity(vDir * fLength / 5.f);
	}
}

void CLinkAnimScript::Func_DisableCanJust()
{
	m_bCanJustAttackStart = false;

}

void CLinkAnimScript::Func_SetVelocityZero()
{
	RigidBody()->SetVelocity(Vec3::Zero);
}

void CLinkAnimScript::Func_JustAtkEnd()
{
	// monster set state 
	if (m_pLockOnRadar->GetLockOnTarget() && m_bInvincible)
		m_pLockOnRadar->GetLockOnTarget()->GetScript<CMonsterScript>()->Damage(0, Vec3(0.f, 10000.f, 0.f));

	TimeSlowAffectedObj(true, GetOwner());
	TimeSlow(false);
	m_bInvincible = false;
	m_bCanJustAttackStart = false;
	m_bJustAtkEndOnce = false;
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

void CLinkAnimScript::Func_LastSwing()
{
	Func_SwordAttackMove();

	int idx =  g_random(g_gen) % 3;
	wstring strKey = L"sound\\link\\Link_Voice_LastAttack_";
	strKey += std::to_wstring(idx) + L".mp3";
	CResMgr::GetInst()->FindRes<CSound>(strKey)->Play(1, LINK_VOLUME * 1.f, false, GetOwner());
}

void CLinkAnimScript::Func_BowDrawSound()
{
	CResMgr::GetInst()->FindRes<CSound>(L"sound\\link\\Bow_Draw0.mp3")->Play(1, LINK_VOLUME * 1.f, false, GetOwner());
}

void CLinkAnimScript::Func_BowShootSound()
{
	CResMgr::GetInst()->FindRes<CSound>(L"sound\\link\\Bow_Release0.mp3")->Play(1, LINK_VOLUME * 1.f, false, GetOwner());
}

void CLinkAnimScript::Func_GuardHitSound()
{
	CResMgr::GetInst()->FindRes<CSound>(L"sound\\link\\Guard_Wood_Metal_0.mp3")->Play(1, LINK_VOLUME * 1.f, false, GetOwner());
}

void CLinkAnimScript::Func_JumpStartSound()
{
	CResMgr::GetInst()->FindRes<CSound>(L"sound\\link\\Mt_Grass_Pl_Jump_00.mp3")->Play(1, LINK_VOLUME * 1.f, false, GetOwner());
}

void CLinkAnimScript::Func_LandSound()
{
	CResMgr::GetInst()->FindRes<CSound>(L"sound\\link\\Mt_Grass_Pl_Land_01.mp3")->Play(1, LINK_VOLUME * 1.f, false, GetOwner());
}

void CLinkAnimScript::Func_SwordEquipSound()
{
	CResMgr::GetInst()->FindRes<CSound>(L"sound\\link\\Weapon_Sword_Metal_Equip00.mp3")->Play(1, LINK_VOLUME * 1.f, false, GetOwner());
}

void CLinkAnimScript::Func_SwordUnEquipSound()
{
	CResMgr::GetInst()->FindRes<CSound>(L"sound\\link\\Weapon_Sword_Metal_UnEquip00.mp3")->Play(1, LINK_VOLUME * 1.f, false, GetOwner());
}

void CLinkAnimScript::Func_BowEquipSound()
{
	CResMgr::GetInst()->FindRes<CSound>(L"sound\\link\\Pl_Equip_Bow00.mp3")->Play(1, LINK_VOLUME * 1.f, false, GetOwner());
}

void CLinkAnimScript::Func_BowUnEquipSound()
{
	CResMgr::GetInst()->FindRes<CSound>(L"sound\\link\\Pl_UnEquip_Bow00.mp3")->Play(1, LINK_VOLUME * 1.f, false, GetOwner());
}

void CLinkAnimScript::Func_JustGuardSound()
{
	CResMgr::GetInst()->FindRes<CSound>(L"sound\\link\\Shield_JustGuard.mp3")->Play(1, LINK_VOLUME * 1.f, false, GetOwner());
}

void CLinkAnimScript::Func_SwordSwingSound()
{
	CResMgr::GetInst()->FindRes<CSound>(L"sound\\link\\SE_ESf_SWING_SWORD_S.mp3")->Play(1, LINK_VOLUME * 0.7f, true, GetOwner());
}

void CLinkAnimScript::Func_TimeSlowSound()
{
	CResMgr::GetInst()->FindRes<CSound>(L"sound\\link\\SE_Scene_SlowLoop.mp3")->Play(1, LINK_VOLUME * 1.f, false, GetOwner());
}



void CLinkAnimScript::Func_DamageSmallSound()
{
	int idx = g_random(g_gen) % 4;
	wstring strKey = L"sound\\link\\Link_Voice_Hit_Small_";
	strKey += std::to_wstring(idx) + L".mp3";

	CResMgr::GetInst()->FindRes<CSound>(strKey)->Play(1, LINK_VOLUME * 1.f, false, GetOwner());
}

void CLinkAnimScript::Func_DamageMediumSound()
{

}

void CLinkAnimScript::Func_WalkSound_L()
{
	CResMgr::GetInst()->FindRes<CSound>(L"sound\\link\\Mt_Grass_Pl_MoveSlow_ToeL00.mp3")->Play(1, LINK_VOLUME * 1.f, false, GetOwner());
}

void CLinkAnimScript::Func_WalkSound_R()
{
	CResMgr::GetInst()->FindRes<CSound>(L"sound\\link\\Mt_Grass_Pl_MoveSlow_ToeR00.mp3")->Play(1, LINK_VOLUME * 1.f, false, GetOwner());
}

void CLinkAnimScript::Func_RunSound_L()
{
	CResMgr::GetInst()->FindRes<CSound>(L"sound\\link\\Mt_Grass_Pl_MoveNormal_ToeL00.mp3")->Play(1, LINK_VOLUME * 1.f, false, GetOwner());
}

void CLinkAnimScript::Func_RunSound_R()
{
	CResMgr::GetInst()->FindRes<CSound>(L"sound\\link\\Mt_Grass_Pl_MoveNormal_ToeR00.mp3")->Play(1, LINK_VOLUME * 1.f, false, GetOwner());
}

void CLinkAnimScript::Func_DashSound_L()
{
	CResMgr::GetInst()->FindRes<CSound>(L"sound\\link\\Mt_Grass_Pl_MoveFast_ToeL00.mp3")->Play(1, LINK_VOLUME * 1.f, false, GetOwner());
}

void CLinkAnimScript::Func_DashSound_R()
{
	CResMgr::GetInst()->FindRes<CSound>(L"sound\\link\\Mt_Grass_Pl_MoveFast_ToeR00.mp3")->Play(1, LINK_VOLUME * 1.f, false, GetOwner());
}

void CLinkAnimScript::Func_LowerFootstepSound()
{
	if (m_pCurAnimNodeLower == nullptr)
		return;

	int iFrameIdx = m_pCurAnimNodeLower->pAnim->GetCurFrame();

	if (iFrameIdx == 499 || iFrameIdx == 522 || iFrameIdx == 545 || iFrameIdx == 567)
	{
		Func_WalkSound_R();
	}
	else if (iFrameIdx == 511 || iFrameIdx == 534 || iFrameIdx == 557 || iFrameIdx == 579)
	{
		Func_WalkSound_L();
	}
	else if (iFrameIdx == 596 || iFrameIdx == 616 || iFrameIdx == 642 || iFrameIdx == 667)
	{
		Func_RunSound_R();
	}
	else if (iFrameIdx == 607 || iFrameIdx == 628 || iFrameIdx == 654 || iFrameIdx == 679)
	{
		Func_RunSound_L();
	}
}

void CLinkAnimScript::Func_UpperFootstepSound()
{
	int iFrameIdx = m_pCurAnimNode->pAnim->GetCurFrame();

	if (iFrameIdx == 53)
	{
		Func_WalkSound_R();
	}
	else if (iFrameIdx == 66)
	{
		Func_WalkSound_L();
	}
	else if (iFrameIdx == 115 || iFrameIdx == 475 || iFrameIdx == 1817)
	{
		Func_RunSound_R();
	}
	else if (iFrameIdx == 127 || iFrameIdx == 487 || iFrameIdx == 1829)
	{
		Func_RunSound_L();
	}
	else if (iFrameIdx == 190)
	{
		Func_DashSound_R();
	}
	else if (iFrameIdx == 202)
	{
		Func_DashSound_L();
	}
}

void CLinkAnimScript::Func_SwordEquipOffForced()
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

void CLinkAnimScript::Func_BowEquipOffForced()
{
	CBonesocketScript* pBoneScr = m_pBowObj->GetScript<CBonesocketScript>();
	pBoneScr->setBoneIdx((UINT)LINK_BONE_STRING::Pod_A);
	pBoneScr->setOffsetPos(Vec3(0.f, 0.f, -0.07f));
	pBoneScr->setOffsetRot(Vec3(0.f, 0.f, -45.f));

	m_bArrEquip[(UINT)EQUIPMENT_STATE::BOW] = false;
}
