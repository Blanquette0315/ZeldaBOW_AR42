#include "pch.h"
#include "CLinkAnimScript.h"

#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>
#include <Engine/CAnimator3D.h>
#include <Engine/CAnimation3D.h>
#include <Engine/CGameObject.h>

#include "CCameraMgr.h"
#include "FSMNode.h"
#include "CGroundCheckScript.h"
#include "CLockOnScript.h"
#include "CBonesocketScript.h"
#include "CLinkCamScript.h"
#include "CUIHeartScript.h"

map<wstring, CAnimation3D*> CLinkAnimScript::m_mapAnim = {};
map<wstring, tAnimNode*> CLinkAnimScript::m_mapAnimNode = {};
bool CLinkAnimScript::m_bIsAnimNodeLoaded = false;

CLinkAnimScript::CLinkAnimScript()
	: CScript(SCRIPT_TYPE::LINKANIMSCRIPT)
	, m_pAnimator(nullptr)
	, m_pCurAnimNode(nullptr)
	//, m_pNextAnimNode(nullptr)
	, m_iCond(0)
	, m_fAnglePerSec(XM_2PI)
	, m_iMode((UINT)LINK_MODE::LINK_MODE_WALK)
	, m_fWalkSpeed(1.f)
	, m_fRunSpeed(1.5f)
	, m_fDashSpeed(2.f)
	, m_fJumpSpeed(4.f)
	, m_bOnceAtAnimStart(true)
	, m_bComboProgress(false)
	, m_bLockOn(false)
	, m_bIsAnimChanged(false)
	, m_pPrevAnimNode(nullptr)
	, m_bLockOnRotFinish(false)
	, m_bInvincible(false)
	, m_pInJustRigidObj(nullptr)
	, m_fParryingAccTime(0.f)
	, m_fParryingMaxTime(0.1f)
	, m_fEvasionAccTime(0.f)
	, m_fEvasionMaxTime(0.1f)
	, m_bParryingOnce(false)
	, m_bEvasionOnce(false)
	, m_bArrEquip{}
	, m_bCanJustAttackStart(false)
	, m_fJustAtkAccTime(0.f)
	, m_fJustAtkMaxTime(2.f)
{
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Walk Speed", &m_fWalkSpeed, 0.f, 20.f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Run Speed", &m_fRunSpeed, 0.f, 20.f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Dash Speed", &m_fDashSpeed, 0.f, 20.f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Jump Speed", &m_fJumpSpeed, 0.f, 20.f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Angle/s Speed", &m_fAnglePerSec, 0.f, 20.f);

	AddScriptParam(SCRIPT_PARAM::FLOAT, "Combo Time", &m_fComboMaxTime, 0.f, 1.f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Parrying Time", &m_fParryingMaxTime, 0.f, 1.f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Evasion Time", &m_fEvasionMaxTime, 0.f, 1.f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "JustAtk Time", &m_fJustAtkMaxTime, 0.f, 1.f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "JustMove Force", &m_fJustMoveForce, 0.f, 20.f);
	AddScriptParam(SCRIPT_PARAM::PREFAB, "Bomb Prefab", &m_pBombPref);

	AddScriptParam(SCRIPT_PARAM::PREFAB, "Parrying Effect Prefab", &m_EffectParrying);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Parrying Offset", &m_fParryingOffset, 0.f, 100.f);

	AddScriptParam(SCRIPT_PARAM::PREFAB, "ScreenFlash Effect Prefab", &m_EffectScreenFlash);
}

CLinkAnimScript::CLinkAnimScript(const CLinkAnimScript& _origin)
	: CScript(_origin)
	, m_pAnimator(nullptr)
	, m_pCurAnimNode(nullptr)
	//, m_pNextAnimNode(nullptr)
	, m_iCond(0)
	, m_fAnglePerSec(_origin.m_fAnglePerSec)
	, m_iMode(_origin.m_iMode)
	, m_fWalkSpeed(_origin.m_fWalkSpeed)
	, m_fRunSpeed(_origin.m_fRunSpeed)
	, m_fJumpSpeed(_origin.m_fJumpSpeed)
	, m_bOnceAtAnimStart(_origin.m_bOnceAtAnimStart)
	, m_fComboAccTime(0.f)
	, m_fComboMaxTime(_origin.m_fComboMaxTime)
	, m_bComboProgress(false)
	, m_bLockOn(false)
	, m_bIsAnimChanged(false)
	, m_pPrevAnimNode(nullptr)
	, m_bLockOnRotFinish(false)
	, m_bInvincible(false)
	, m_pInJustRigidObj(nullptr)
	, m_fParryingAccTime(0.f)
	, m_fParryingMaxTime(_origin.m_fParryingMaxTime)
	, m_fEvasionAccTime(0.f)
	, m_fEvasionMaxTime(_origin.m_fEvasionMaxTime)
	, m_bParryingOnce(false)
	, m_bEvasionOnce(false)
	, m_bArrEquip{}
	, m_bCanJustAttackStart(false)
	, m_fJustAtkAccTime(0.f)
	, m_fJustAtkMaxTime(_origin.m_fJustAtkMaxTime)
	, m_fJustMoveForce(_origin.m_fJustMoveForce)
	, m_pBombPref(_origin.m_pBombPref)
	, m_EffectParrying(_origin.m_EffectParrying)
	, m_EffectScreenFlash(_origin.m_EffectScreenFlash)
	, m_fParryingOffset(_origin.m_fParryingOffset)
{
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Walk Speed", &m_fWalkSpeed, 0.f, 20.f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Run Speed", &m_fRunSpeed, 0.f, 20.f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Dash Speed", &m_fDashSpeed, 0.f, 20.f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Jump Speed", &m_fJumpSpeed, 0.f, 20.f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Angle/s Speed", &m_fAnglePerSec, 0.f, 20.f);
	
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Combo Time", &m_fComboMaxTime, 0.f, 1.f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Parrying Time", &m_fParryingMaxTime, 0.f, 1.f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Evasion Time", &m_fEvasionMaxTime, 0.f, 1.f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "JustAtk Time", &m_fJustAtkMaxTime, 0.f, 1.f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "JustMove Force", &m_fJustMoveForce, 0.f, 20.f);
	AddScriptParam(SCRIPT_PARAM::PREFAB, "Bomb Prefab", &m_pBombPref);

	AddScriptParam(SCRIPT_PARAM::PREFAB, "Parrying Effect Prefab", &m_EffectParrying);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Parrying Offset", &m_fParryingOffset, 0.f, 100.f);

	AddScriptParam(SCRIPT_PARAM::PREFAB, "ScreenFlash Effect Prefab", &m_EffectScreenFlash);
}

CLinkAnimScript::~CLinkAnimScript()
{
}

bool CLinkAnimScript::IsCurAnim(LINK_ANIM_TYPE _eLAT)
{
	return m_pCurAnimNode->pAnimKey == LINK_ANIM_WCHAR[_eLAT]; 
}

bool CLinkAnimScript::IsCurAnimLower(LINK_ANIM_TYPE _eLAT)
{
	if (m_pCurAnimNodeLower)
	{
		return m_pCurAnimNodeLower->pAnimKey == LINK_ANIM_WCHAR[_eLAT];
	}
	else
	{
		return false;
	}
}

LINK_FRONT_TOE CLinkAnimScript::GetFrontToe(CAnimation3D* _pCurAnim)
{
	CAnimator3D* pAnimator = Animator3D();
	CAnimation3D* pAnim = nullptr;

	if (pAnimator) 
	{
		if (m_pCurAnimNodeLower)
			pAnim = pAnimator->GetCurAnimationLower();
		else
			pAnim = pAnimator->GetCurAnimation();
	}
		

	if (pAnim)
	{
		int iCurFrame = pAnim->GetCurFrame();
		//float fCurTime = iCurFrame / 60.f;
		const tMTBone& tBoneToe_L = pAnimator->GetBoneByIdx((UINT)LINK_BONE_STRING::Toe_L);
		const tMTBone& tBoneToe_R = pAnimator->GetBoneByIdx((UINT)LINK_BONE_STRING::Toe_R);
		const vector<tMTKeyFrame>& vecKeyFrame_Toe_L = tBoneToe_L.vecKeyFrame;
		const vector<tMTKeyFrame>& vecKeyFrame_Toe_R = tBoneToe_R.vecKeyFrame;

		//int iIdxToe_L = vecKeyFrame_Toe_L.size() / 2;
		//int iIdxToe_R = vecKeyFrame_Toe_R.size() / 2;
		//iIdxToe_L = FindClosestIdx(vecKeyFrame_Toe_L, fCurTime, iIdxToe_L, 0, vecKeyFrame_Toe_L.size());
		//iIdxToe_R = FindClosestIdx(vecKeyFrame_Toe_R, fCurTime, iIdxToe_R, 0, vecKeyFrame_Toe_R.size());

		//// should consider direction of link
		//if (vecKeyFrame_Toe_L[iIdxToe_L].vTranslate.z < vecKeyFrame_Toe_R[iIdxToe_R].vTranslate.z)
		if (vecKeyFrame_Toe_L[iCurFrame].vTranslate.z < vecKeyFrame_Toe_R[iCurFrame].vTranslate.z)
			return LINK_FRONT_TOE::LEFT;
		else
			return LINK_FRONT_TOE::RIGHT;
	}

	return LINK_FRONT_TOE::END;
}

int CLinkAnimScript::FindClosestIdx(const vector<tMTKeyFrame>& _vec, float _fTime, int _iIdx, int _iStart, int _iEnd)
{
	if (_iStart == _iIdx)
		return _iIdx;

	if (_vec[_iIdx].dTime > _fTime)
	{
		return FindClosestIdx(_vec, _fTime, (_iStart + _iIdx) / 2, _iStart, _iIdx);
	}
	else
	{
		return FindClosestIdx(_vec, _fTime, (_iEnd + _iIdx) / 2, _iIdx, _iEnd);
	}
}

void CLinkAnimScript::CalcMoveDirection()
{
	// camera and link tick point problem can occur.
	// maybe... rigidbody tick determine where the link is. -> so doesn't matter.
	Vec3 vCamFrontDir = m_pLinkCamObj->Transform()->GetWorldDir(DIR::FRONT);

	m_vDir[(UINT)LINK_DIRECTION::FRONT] = Vec3(vCamFrontDir.x, 0.f, vCamFrontDir.z);
	m_vDir[(UINT)LINK_DIRECTION::FRONT].Normalize();
	m_vDir[(UINT)LINK_DIRECTION::BACK] = Vec3(-m_vDir[(UINT)LINK_DIRECTION::FRONT].x, 0.f, -m_vDir[(UINT)LINK_DIRECTION::FRONT].z);
	m_vDir[(UINT)LINK_DIRECTION::LEFT] = Vec3(-m_vDir[(UINT)LINK_DIRECTION::FRONT].z, 0.f, m_vDir[(UINT)LINK_DIRECTION::FRONT].x);
	m_vDir[(UINT)LINK_DIRECTION::RIGHT] = Vec3(m_vDir[(UINT)LINK_DIRECTION::FRONT].z, 0.f, -m_vDir[(UINT)LINK_DIRECTION::FRONT].x);
	m_vDir[(UINT)LINK_DIRECTION::NONE] = Vec3::Zero;
}

bool CLinkAnimScript::ShouldTurnBack()
{
	if (GetCombinedDir() == Vec3::Zero)
		return false;

	float fRad = acosf(GetCombinedDir().Dot(Transform()->GetRelativeDir(DIR::FRONT)));
	if (fRad > XM_PI / 4.f * 3.f)
		return true;
	else
		return false;
}

Vec3 CLinkAnimScript::GetCombinedDir()
{
	LINK_DIRECTION eDirX;
	LINK_DIRECTION eDirZ;

	if (KEY_PRESSED(KEY::W))
		eDirZ = LINK_DIRECTION::FRONT;
	else if (KEY_PRESSED(KEY::S))
		eDirZ = LINK_DIRECTION::BACK;
	else
		eDirZ = LINK_DIRECTION::NONE;

	if (KEY_PRESSED(KEY::A))
		eDirX = LINK_DIRECTION::LEFT;
	else if (KEY_PRESSED(KEY::D))
		eDirX = LINK_DIRECTION::RIGHT;
	else
		eDirX = LINK_DIRECTION::NONE;

	return (GetMoveDir(eDirX) + GetMoveDir(eDirZ)).Normalize();
}

bool CLinkAnimScript::IsGround()
{
	return m_pGroundChecker->IsGround();
}

void CLinkAnimScript::MoveByForce(Vec3 _vDirXZ, float _fForce)
{
	_vDirXZ.y = 0.f;
	_vDirXZ.Normalize();
	_vDirXZ *= 5.f;
	_vDirXZ += Transform()->GetRelativeDir(DIR::UP);
	_vDirXZ.Normalize();

	RigidBody()->AddForce(_vDirXZ * 0.01f * _fForce);
}

void CLinkAnimScript::ApplyDamage()
{
	m_tLinkStatus.iHP -= m_tLinkDamaged.iDamage;
	m_pHPUI->SetReflect(true);

	m_tLinkDamaged = {};
}

void CLinkAnimScript::JustAttackTimer()
{
	if (m_bCanJustAttackStart)
	{
		if (m_fJustAtkMaxTime < m_fJustAtkAccTime)
		{
			m_bCanJustAttackStart = false;
			m_fJustAtkAccTime = 0.f;
		}
		else
		{
			m_fJustAtkAccTime += FDT;
		}
	}
	else
	{
		m_fJustAtkAccTime = 0.f;
	}
}

void CLinkAnimScript::ClearAnimNode()
{
	m_mapAnim.clear();
	Safe_Del_Map(m_mapAnimNode);
}

void CLinkAnimScript::init()
{
	m_pAnimator = Animator3D();
	m_pAnimator->ResizeVecBone();
	m_pAnimator->SetBoneUpperAndElseLower(0, LinkBodyDivPoint);
	m_pAnimator->CreateBoneCheckBuffer();
	m_mapAnim = m_pAnimator->GetMapAnimation();

	if (m_bIsAnimNodeLoaded == false)
	{
		CreateAnimNode();
		MakeFSM();

		m_bIsAnimNodeLoaded = true;
	}
	else
	{
		ReplaceNodeAnim();
	}
}

void CLinkAnimScript::begin()
{
	SetAnimNode(m_pCurAnimNode, LAT_WAIT);
	m_pLinkCamObj = CCameraMgr::GetInst()->GetCameraObj(CAMERA_SELECTION::LINK);

	CGameObject* pGroundCheckObj = GetOwner()->GetChildObjByName(LINK_STRING_WCHAR[LINK_STRING_GROUND_CHECKER]);
	if(pGroundCheckObj)
		m_pGroundChecker = pGroundCheckObj->GetScript<CGroundCheckScript>();

	CGameObject* pLockOnRadarObj = GetOwner()->GetChildObjByName(LINK_STRING_WCHAR[LINK_STRING_LOCKON_RADAR]);
	if(pLockOnRadarObj)
		m_pLockOnRadar = pLockOnRadarObj->GetScript<CLockOnScript>();

	CGameObject* pHPUI = CLevelMgr::GetInst()->GetCurLevel()->FindObjectByName(LINK_STRING_WCHAR[LINK_STRING_UI_HP]);
	if(pHPUI)
		m_pHPUI = pHPUI->GetScript<CUIHeartScript>();

	CGameObject* pCrosshairUI = CLevelMgr::GetInst()->GetCurLevel()->FindObjectByName(LINK_STRING_WCHAR[LINK_STRING_UI_CROSSHAIR]);
	if(pCrosshairUI)
		m_pCrosshairUI = pCrosshairUI->GetScript<CUIScript>();

	m_pSwordObj = GetOwner()->GetChildObjByName(LINK_STRING_WCHAR[LINK_STRING_SWORD]);
	m_pSwordObj = GetOwner()->GetChildObjByName(LINK_STRING_WCHAR[LINK_STRING_SWORD]);
	m_pShieldObj = GetOwner()->GetChildObjByName(LINK_STRING_WCHAR[LINK_STRING_SHIELD]);
	m_pShieldObj = GetOwner()->GetChildObjByName(LINK_STRING_WCHAR[LINK_STRING_SHIELD]);
	Func_SwordEquipOff();

	m_pBowObj = GetOwner()->GetChildObjByName(LINK_STRING_WCHAR[LINK_STRING_BOW]);
	m_pBowObj = GetOwner()->GetChildObjByName(LINK_STRING_WCHAR[LINK_STRING_BOW]);
	Func_BowEquipOff();

	m_tLinkStatus.iHP = 16;
	m_tLinkDamaged.eType = LINK_DAMAGED_TYPE::NONE;
	m_pAnyStateNode = m_mapAnimNode.find(LINK_STRING_WCHAR[LINK_STRING_ANYSTATE_NODE_KEY])->second;

	m_pCurAnimNode = m_mapAnimNode.find(LINK_ANIM_WCHAR[LAT_WAIT])->second;
	m_pAnimator->Play(m_pCurAnimNode->pAnimKey, true);
}

void CLinkAnimScript::tick()
{
	// PlayNextAnim();
	if ((m_pCurAnimNode->iPreferences & LAP_AIR) == 0)
		RigidBody()->SetVelocity(Vec3::Zero);


	SetLinkCond();
	Timer();
	OperateAnimFuncAfter();
	PlayNextAnim();
	OperateAnimFunc();
	ControlUI();
	ClearData();
}

void CLinkAnimScript::OperateAnimFunc()
{
	CalcMoveDirection();
	SelectSpeed();

	// Func that only operate at anim start
	if (m_bOnceAtAnimStart)
	{
		if (m_pCurAnimNode->Func_Start)
		{
			(this->*m_pCurAnimNode->Func_Start)();
		}

		m_bOnceAtAnimStart = false;
	}

	if (m_pCurAnimNode->Func_Steady)
	{
		(this->*m_pCurAnimNode->Func_Steady)();
	}

	// Lower Body Blend 
	if (CalBit(m_pCurAnimNode->iPreferences, LAP_BLEND, BIT_LEAST_ONE))
	{
		Func_LowerBodyBlend();
	}
	else
	{
		m_pCurAnimNodeLower = nullptr;
		m_pAnimator->StopLowerAnim();
	}
}

void CLinkAnimScript::ControlUI()
{
	if (CalBit(m_pCurAnimNode->iPreferences, LAP_BOW_CHARGE, BIT_LEAST_ONE))
	{
		m_pCrosshairUI->Show();
	}
	else
	{
		m_pCrosshairUI->Hide();
	}
}

void CLinkAnimScript::Timer()
{
	if (m_bIsAnimChanged && CalBit(m_pCurAnimNode->iPreferences, LAP_JUST_ATK, BIT_LEAST_ONE))
	{
		// reset
		m_fJustAtkAccTime = 0.f;
		m_bJustAtkEndOnce = true;
	}
	
	// timer each attack
	if (m_bJustAtkEndOnce)
	{
		if (m_fJustAtkAccTime > m_fJustAtkMaxTime)
		{
			Func_JustAtkEnd();
			RemoveBit(m_iCond, LAC_KEY_LBTN_COMBO);
		}
		else
		{
			m_fJustAtkAccTime += FDT;
		}
	}
	
	// JustAttackTimer();
}

void CLinkAnimScript::ClearData()
{
	m_bShieldGuard = false;
	m_bInJustRigid = false;
	m_pInJustRigidObj = nullptr;
}

void CLinkAnimScript::SetLinkCond()
{
	m_iCond = 0;

	// key check
	if (KEY_PRESSED(KEY::W) || KEY_PRESSED(KEY::S) || KEY_PRESSED(KEY::A) || KEY_PRESSED(KEY::D) ||
		KEY_TAP(KEY::W) || KEY_TAP(KEY::S) || KEY_TAP(KEY::A) || KEY_TAP(KEY::D))
		AddBit(m_iCond, LAC_KEY_WSAD);

	if (KEY_PRESSED(KEY::LSHIFT) || KEY_TAP(KEY::LSHIFT))
		AddBit(m_iCond, LAC_KEY_SHIFT);

	if (KEY_PRESSED(KEY::SPACE) || KEY_TAP(KEY::SPACE))
		AddBit(m_iCond, LAC_KEY_SPACE);

	if (KEY_PRESSED(KEY::N_1) || KEY_TAP(KEY::N_1))
		AddBit(m_iCond, LAC_KEY_N1);

	if (KEY_PRESSED(KEY::N_2) || KEY_TAP(KEY::N_2))
		AddBit(m_iCond, LAC_KEY_N2);

	if (KEY_TAP(KEY::N_3))
		AddBit(m_iCond, LAC_KEY_N3);

	if (KEY_TAP(KEY::F))
	{
		if (m_bInJustRigid)
		{
			m_bInvincible = true;
		}
		AddBit(m_iCond, LAC_KEY_F);
	}

	if (KEY_PRESSED(KEY::LBTN) || KEY_TAP(KEY::LBTN))
	{
		if (CalBit(m_pCurAnimNode->iPreferences, LAP_COMBO, BIT_LEAST_ONE))
		{
			m_bComboProgress = true;
			m_fComboAccTime = 0.f;
		}
			
		AddBit(m_iCond, LAC_KEY_LBTN);
	}


	// check combo progress with expanded time
	if (m_bComboProgress)
		AddBit(m_iCond, LAC_KEY_LBTN_COMBO);

	// combo
	if (m_bComboProgress)
	{
		m_fComboAccTime += FDT;
		if (m_fComboMaxTime <= m_fComboAccTime)
		{
			m_fComboAccTime = 0.f;
			m_bComboProgress = false;
		}
	}

	// mode ckeck
	if (KEY_TAP(KEY::R))
	{
		if (CalBit(m_iMode, LINK_MODE_WALK, BIT_INCLUDE))
		{
			RemoveBit(m_iMode, LINK_MODE_WALK);
			AddBit(m_iMode, LINK_MODE_RUN);
		}
		else
		{
			RemoveBit(m_iMode, LINK_MODE_RUN);
			AddBit(m_iMode, LINK_MODE_WALK);
		}
	}

	if (m_pLockOnRadar->GetLockOnTarget())
	{
		if (KEY_TAP(KEY::Q))
		{
			if (CalBit(m_iMode, LINK_MODE_LOCKON, BIT_INCLUDE))
			{
				RemoveBit(m_iMode, LINK_MODE_LOCKON);
				//m_pLinkCamObj->GetScript<CLinkCamScript>()->SetMode(LINK_CAM_MODE::GENERAL_START);
				m_bLockOnRotFinish = false;
			}
			else
			{
				AddBit(m_iMode, LINK_MODE_LOCKON);
				// m_pLinkCamObj->GetScript<CLinkCamScript>()->SetMode(LINK_CAM_MODE::LOCKON_START);
			}
		}
	}
	else
	{
		RemoveBit(m_iMode, LINK_MODE_LOCKON);
		m_bLockOnRotFinish = false;
	}

	if (CalBit(m_pCurAnimNode->iPreferences, LAP_KEEP_LOCKON, BIT_LEAST_ONE))
	{

	}
	else
	{
		if (CalBit(m_iMode, LINK_MODE_LOCKON, BIT_LEAST_ONE))
		{
			// m_pLinkCamObj->GetScript<CLinkCamScript>()->SetMode(LINK_CAM_MODE::GENERAL_START);
			RemoveBit(m_iMode, LINK_MODE_LOCKON);
			m_bLockOnRotFinish = false;
		}
	}

	if(CalBit(m_iMode, (UINT)LINK_MODE::LINK_MODE_RUN, BIT_LEAST_ONE))
		AddBit(m_iCond, LAC_MODE_RUN);
	else if (CalBit(m_iMode, (UINT)LINK_MODE::LINK_MODE_WALK, BIT_LEAST_ONE))
		AddBit(m_iCond, LAC_MODE_WALK);

	if(CalBit(m_iMode, LINK_MODE_LOCKON, BIT_LEAST_ONE))
		AddBit(m_iCond, LAC_MODE_LOCKON);

	// anim check
	if (m_pCurAnimNode->pAnim->IsFinished())
		AddBit(m_iCond, LAC_ANIM_FINISHED);

	// toe front check
	if (LINK_FRONT_TOE::LEFT == GetFrontToe(m_pCurAnimNode->pAnim))
		AddBit(m_iCond, LAC_TOE_L_FRONT);

	// check angle between next dir and cur dir is over 135 degree
	if (ShouldTurnBack())
		AddBit(m_iCond, LAC_TURN_BACK);

	// check ground
	if (IsGround())
	{
		AddBit(m_iCond, LAC_GROUNDED);
	}
	RigidBody()->SetGround(IsGround());

	// check wheter link guarded monster atk 
	if (m_bShieldGuard)
	{
		AddBit(m_iCond, LAC_SHIELD_GUARD);
		m_bShieldGuard = false;
	}

	// Check Damage
	// link invincible
	if (CalBit(m_pCurAnimNode->iPreferences, LAP_INVINCIBLE, BIT_LEAST_ONE) || m_bShieldGuard || m_bInvincible)
	{
		m_tLinkDamaged = {};
	}
	// link not invincible
	else
	{
		if (m_tLinkDamaged.eType != LINK_DAMAGED_TYPE::NONE)
		{
			AddBit(m_iCond, LAC_DAMAGED_BACK);	

			switch (m_tLinkDamaged.eType)
			{
			case LINK_DAMAGED_TYPE::SMALL:
			{
				AddBit(m_iCond, LAC_DAMAGED_SMALL);
			}
			break;

			case LINK_DAMAGED_TYPE::MEDIUM:
			{
				AddBit(m_iCond, LAC_DAMAGED_MEDIUM);
			}
			break;

			case LINK_DAMAGED_TYPE::LARGE:
			{
				AddBit(m_iCond, LAC_DAMAGED_LARGE);
			}
			break;
			}

			// -- add code : sword guard hit
			ApplyDamage();
			if (m_tLinkStatus.iHP <= 0)
				AddBit(m_iCond, LAC_DEAD);
		}
	}

	// check equipment
	if (m_bArrEquip[(UINT)EQUIPMENT_STATE::SWORD])
	{
		AddBit(m_iCond, LAC_EQUIP_SWORD);
	}
	else if (m_bArrEquip[(UINT)EQUIPMENT_STATE::BOW])
	{
		AddBit(m_iCond, LAC_EQUIP_BOW);
	}
	else if (m_bArrEquip[(UINT)EQUIPMENT_STATE::SHIELD])
	{
		AddBit(m_iCond, LAC_EQUIP_SHIELD);
	}


	if (m_bCanJustAttackStart)
	{
		AddBit(m_iCond, LAC_CAN_JUST_ATTACK_START);
	}
}

void CLinkAnimScript::OperateAnimFuncAfter()
{
	if (m_pCurAnimNode->pAnim->IsFinished())
	{
		if (m_pCurAnimNode->Func_End)
		{
			(this->*m_pCurAnimNode->Func_End)();
		}
	}
}

void CLinkAnimScript::PlayNextAnim()
{
	size_t iTranCount = m_pAnyStateNode->vecTranAnim.size();

	for (size_t i = 0; i < iTranCount; ++i)
	{
		UINT iCmpBit = m_iCond;

		RemoveBit(iCmpBit, m_pAnyStateNode->vecTranAnim[i]->iExcludeCond);

		if (CalBit(iCmpBit, m_pAnyStateNode->vecTranAnim[i]->iTranCond, BIT_EQUAL))
		{
			tAnimNode* pNextNode = m_mapAnimNode.find(m_pAnyStateNode->vecTranAnim[i]->pTargetAnimKey)->second;
			if (pNextNode == m_pCurAnimNode)
				break;

			if (m_pCurAnimNode->Func_Exit)
			{
				(this->*m_pCurAnimNode->Func_Exit)();
			}

			m_pPrevAnimNode = m_pCurAnimNode;
			m_pCurAnimNode = m_mapAnimNode.find(m_pAnyStateNode->vecTranAnim[i]->pTargetAnimKey)->second;
			//return;

			if (CalBit(m_pCurAnimNode->iPreferences, LAP_REPEAT, BIT_FUNC_OPT::BIT_LEAST_ONE))
			{
				m_pAnimator->Play(m_pCurAnimNode->pAnimKey, true);

				m_bOnceAtAnimStart = true;
				m_bIsAnimChanged = true;
				return;
			}
			else
			{
				m_pAnimator->Play(m_pCurAnimNode->pAnimKey, false);

				m_bOnceAtAnimStart = true;
				m_bIsAnimChanged = true;
				return;
			}
		}
	}

	iTranCount = m_pCurAnimNode->vecTranAnim.size();

	for (size_t i = 0; i < iTranCount; ++i)
	{
		UINT iCmpBit = m_iCond;

		RemoveBit(iCmpBit, m_pCurAnimNode->vecTranAnim[i]->iExcludeCond);

		if (CalBit(iCmpBit, m_pCurAnimNode->vecTranAnim[i]->iTranCond, BIT_EQUAL))
		{
			if (m_pCurAnimNode->Func_Exit)
			{
				(this->*m_pCurAnimNode->Func_Exit)();
			}


			m_pPrevAnimNode = m_pCurAnimNode;
			m_pCurAnimNode = m_mapAnimNode.find(m_pCurAnimNode->vecTranAnim[i]->pTargetAnimKey)->second;

			if (CalBit(m_pCurAnimNode->iPreferences, LAP_REPEAT, BIT_FUNC_OPT::BIT_LEAST_ONE))
			{
				m_pAnimator->Play(m_pCurAnimNode->pAnimKey, true);

				m_bOnceAtAnimStart = true;
				m_bIsAnimChanged = true;
				return;
			}
			else
			{
				m_pAnimator->Play(m_pCurAnimNode->pAnimKey, false);

  				m_bOnceAtAnimStart = true;
				m_bIsAnimChanged = true;
				return;
			}
		}
	}

	m_bIsAnimChanged = false;
}

void CLinkAnimScript::BeginOverlap(CGameObject* _pOther)
{
}

void CLinkAnimScript::Overlap(CGameObject* _pOther)
{
}

void CLinkAnimScript::EndOverlap(CGameObject* _pOther)
{
}

void CLinkAnimScript::SaveToYAML(YAML::Emitter& _emitter)
{
	CScript::SaveToYAML(_emitter);

	_emitter << YAML::Key << "Link Walk Speed";
	_emitter << YAML::Value << m_fWalkSpeed;

	_emitter << YAML::Key << "Link Run Speed";
	_emitter << YAML::Value << m_fRunSpeed;

	_emitter << YAML::Key << "Link Dash Speed";
	_emitter << YAML::Value << m_fDashSpeed;

	_emitter << YAML::Key << "Link Jump Speed";
	_emitter << YAML::Value << m_fJumpSpeed;

	_emitter << YAML::Key << "Link Angle Per Sec";
	_emitter << YAML::Value << m_fAnglePerSec;

	/*_emitter << YAML::Key << "Link Mode";
	_emitter << YAML::Value << m_iMode;*/

	_emitter << YAML::Key << "Link Combo Max Time";
	_emitter << YAML::Value << m_fComboMaxTime;

	_emitter << YAML::Key << "Link Status HP";
	_emitter << YAML::Value << m_tLinkStatus.iHP;

	_emitter << YAML::Key << "Parrying Max Time";
	_emitter << YAML::Value << m_fParryingMaxTime;

	_emitter << YAML::Key << "Evasion Max Time";
	_emitter << YAML::Value << m_fEvasionMaxTime;

	_emitter << YAML::Key << "JustATK Max Time";
	_emitter << YAML::Value << m_fJustAtkMaxTime;

	_emitter << YAML::Key << "Just Move Force";
	_emitter << YAML::Value << m_fJustMoveForce;

	SaveResourceRefEX(m_pBombPref, _emitter, "Bomb");
	SaveResourceRefEX(m_EffectParrying, _emitter, "Parrying");
	_emitter << YAML::Key << "ParryingOffset";
	_emitter << YAML::Value << m_fParryingOffset;

	SaveResourceRefEX(m_EffectScreenFlash, _emitter, "ScreenFlash");
}

void CLinkAnimScript::LoadFromYAML(YAML::Node& _node)
{
	CScript::LoadFromYAML(_node);

	SAFE_LOAD_FROM_YAML(float, m_fWalkSpeed, _node["Link Walk Speed"]);
	SAFE_LOAD_FROM_YAML(float, m_fRunSpeed, _node["Link Run Speed"]);
	SAFE_LOAD_FROM_YAML(float, m_fDashSpeed, _node["Link Dash Speed"]);
	SAFE_LOAD_FROM_YAML(float, m_fJumpSpeed, _node["Link Jump Speed"]);
	SAFE_LOAD_FROM_YAML(float, m_fAnglePerSec, _node["Link Angle Per Sec"]);
	/*SAFE_LOAD_FROM_YAML(float, m_iMode, _node["Link Mode"]);*/
	SAFE_LOAD_FROM_YAML(float, m_fComboMaxTime, _node["Link Combo Max Time"]);
	SAFE_LOAD_FROM_YAML(int, m_tLinkStatus.iHP, _node["Link Status HP"]);
	SAFE_LOAD_FROM_YAML(float, m_fParryingMaxTime, _node["Parrying Max Time"]);
	SAFE_LOAD_FROM_YAML(float, m_fEvasionMaxTime, _node["Evasion Max Time"]);
	SAFE_LOAD_FROM_YAML(float, m_fJustAtkMaxTime, _node["JustATK Max Time"]);
	SAFE_LOAD_FROM_YAML(float, m_fJustMoveForce, _node["Just Move Force"]);

	LoadResourceRefEX(m_pBombPref, _node, "Bomb");
	LoadResourceRefEX(m_EffectParrying, _node, "Parrying");
	SAFE_LOAD_FROM_YAML(float, m_fParryingOffset, _node["ParryingOffset"]);

	LoadResourceRefEX(m_EffectScreenFlash, _node, "ScreenFlash");
}


