#include "pch.h"
#include "CLinkAnimScript.h"


#include <Engine/CAnimator3D.h>
#include <Engine/CAnimation3D.h>

#include "CCameraMgr.h"
#include "FSMNode.h"


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
	, m_iMode((UINT)LINK_MODE::WALK)
	, m_fWalkSpeed(1.f)
	, m_fRunSpeed(1.5f)
	, m_fDashSpeed(2.f)
{
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
{
}

CLinkAnimScript::~CLinkAnimScript()
{
}


bool CLinkAnimScript::IsCurAnim(LINK_ANIM_TYPE _eLAT)
{
	return m_pCurAnimNode->pAnimKey == LINK_ANIM_WCHAR[_eLAT]; 
}

LINK_FRONT_TOE CLinkAnimScript::GetFrontToe(CAnimation3D* _pCurAnim)
{
	CAnimator3D* pAnimator = Animator3D();
	CAnimation3D* pAnim = nullptr;

	if (pAnimator)
		pAnim = pAnimator->GetCurAnimation();

	if (pAnim)
	{
		int iCurFrame = pAnim->GetCurFrame();
		float fCurTime = iCurFrame / 60.f;
		const tMTBone& tBoneToe_L = pAnimator->GetBoneByName(L"Toe_L");
		const tMTBone& tBoneToe_R = pAnimator->GetBoneByName(L"Toe_R");
		const vector<tMTKeyFrame>& vecKeyFrame_Toe_L = tBoneToe_L.vecKeyFrame;
		const vector<tMTKeyFrame>& vecKeyFrame_Toe_R = tBoneToe_R.vecKeyFrame;

		int iIdxToe_L = vecKeyFrame_Toe_L.size() / 2;
		int iIdxToe_R = vecKeyFrame_Toe_R.size() / 2;
		iIdxToe_L = FindClosestIdx(vecKeyFrame_Toe_L, fCurTime, iIdxToe_L, 0, vecKeyFrame_Toe_L.size());
		iIdxToe_R = FindClosestIdx(vecKeyFrame_Toe_R, fCurTime, iIdxToe_R, 0, vecKeyFrame_Toe_R.size());

		// should consider direction of link
		if (vecKeyFrame_Toe_L[iIdxToe_L].vTranslate.z < vecKeyFrame_Toe_R[iIdxToe_R].vTranslate.z)
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

void CLinkAnimScript::ClearAnimNode()
{
	m_mapAnim.clear();
	Safe_Del_Map(m_mapAnimNode);
}

void CLinkAnimScript::init()
{
	m_pAnimator = Animator3D();
	if (m_bIsAnimNodeLoaded == false)
	{
		m_mapAnim = m_pAnimator->GetMapAnimation();
		CreateAnimNode();
		MakeFSM();

		m_bIsAnimNodeLoaded = true;
	}
}

void CLinkAnimScript::begin()
{
	SetAnimNode(m_pCurAnimNode, LAT_WAIT);
	RigidBody()->SetGround(true);

	m_pLinkCamObj = CCameraMgr::GetInst()->GetCameraObj(CAMERA_SELECTION::LINK);
}

void CLinkAnimScript::tick()
{
	// PlayNextAnim();
	SetLinkCond();
	OperateAnimFuncAfter();
	PlayNextAnim();
	OperateAnimFunc();
}

//void CLinkAnimScript::PlayNextAnim()
//{
//	if (m_pNextAnimNode != nullptr)
//	{
//		if (CalBit(m_pNextAnimNode->iPreferences, LAP_REPEAT, BIT_FUNC_OPT::BIT_LEAST_ONE))
//			m_pAnimator->Play(m_pNextAnimNode->pAnimKey, true);
//		else
//			m_pAnimator->Play(m_pNextAnimNode->pAnimKey, false);
//
//		m_pCurAnimNode = m_pNextAnimNode;
//		m_pNextAnimNode = nullptr;
//	}
//}

void CLinkAnimScript::OperateAnimFunc()
{
	CalcMoveDirection();

	if (IsCurAnim(LAT_WALK) || IsCurAnim(LAT_RUN) || IsCurAnim(LAT_DASH))
	{
		Func_WalkRunDash();
	}
}

void CLinkAnimScript::SetLinkCond()
{
	m_iCond = 0;

	// key check
	if (KEY_PRESSED(KEY::W) || KEY_PRESSED(KEY::S) || KEY_PRESSED(KEY::A) || KEY_PRESSED(KEY::D))
		AddBit(m_iCond, LAC_KEY_WSAD);

	if (KEY_PRESSED(KEY::LSHIFT))
		AddBit(m_iCond, LAC_KEY_SHIFT);

	// mode ckeck
	if (KEY_TAP(KEY::R))
		m_iMode = ( m_iMode == (UINT)LINK_MODE::WALK ) ? (UINT)LINK_MODE::RUN : (UINT)LINK_MODE::WALK;

	if(m_iMode == (UINT)LINK_MODE::RUN)
		AddBit(m_iCond, LAC_MODE_RUN);
	else if (m_iMode == (UINT)LINK_MODE::WALK)
		AddBit(m_iCond, LAC_MODE_WALK);

	// anim check
	if (m_pCurAnimNode->pAnim->IsFinished())
		AddBit(m_iCond, LAC_ANIM_FINISHED);

	// toe front check
	if (LINK_FRONT_TOE::LEFT == GetFrontToe(m_pCurAnimNode->pAnim))
		AddBit(m_iCond, LAC_TOE_L_FRONT);

	if (ShouldTurnBack())
		AddBit(m_iCond, LAC_TURN_BACK);
	
}

void CLinkAnimScript::OperateAnimFuncAfter()
{
	if (m_pCurAnimNode->pAnim->IsFinished())
	{
		if (IsCurAnim(LAT_RUN_BRAKE_L) || IsCurAnim(LAT_RUN_BRAKE_R) || IsCurAnim(LAT_DASH_BRAKE_L) || IsCurAnim(LAT_DASH_BRAKE_R))
		{
			Func_TurnBack();
		}
	}
}

void CLinkAnimScript::PlayNextAnim()
{
	size_t iTranCount = m_pCurAnimNode->vecTranAnim.size();

	for (size_t i = 0; i < iTranCount; ++i)
	{
		UINT iCmpBit = m_iCond;

		RemoveBit(iCmpBit, m_pCurAnimNode->vecTranAnim[i]->iExcludeCond);

		if (CalBit(iCmpBit, m_pCurAnimNode->vecTranAnim[i]->iTranCond, BIT_EQUAL))
		{
			m_pCurAnimNode = m_mapAnimNode.find(m_pCurAnimNode->vecTranAnim[i]->pTargetAnimKey)->second;
			//return;

			if (CalBit(m_pCurAnimNode->iPreferences, LAP_REPEAT, BIT_FUNC_OPT::BIT_LEAST_ONE))
			{
				m_pAnimator->Play(m_pCurAnimNode->pAnimKey, true);
				return;
			}
			else
			{
				m_pAnimator->Play(m_pCurAnimNode->pAnimKey, false);
				return;
			}
		}
	}
}

void CLinkAnimScript::BeginOverlap(CCollider* _pOther)
{
}

void CLinkAnimScript::Overlap(CCollider* _pOther)
{
}

void CLinkAnimScript::EndOverlap(CCollider* _pOther)
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

	_emitter << YAML::Key << "Link Angle Per Sec";
	_emitter << YAML::Value << m_fAnglePerSec;

	_emitter << YAML::Key << "Link Mode";
	_emitter << YAML::Value << m_iMode;
}

void CLinkAnimScript::LoadFromYAML(YAML::Node& _node)
{
	CScript::LoadFromYAML(_node);

	SAFE_LOAD_FROM_YAML(float, m_fWalkSpeed, _node["Link Walk Speed"]);
	SAFE_LOAD_FROM_YAML(float, m_fRunSpeed, _node["Link Run Speed"]);
	SAFE_LOAD_FROM_YAML(float, m_fDashSpeed, _node["Link Dash Speed"]);
	SAFE_LOAD_FROM_YAML(float, m_fAnglePerSec, _node["Link Angle Per Sec"]);
	SAFE_LOAD_FROM_YAML(float, m_iMode, _node["Link Mode"]);
}


