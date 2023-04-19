#include "pch.h"
#include "CLinkAnimScript.h"

#include <Engine/CAnimator3D.h>
#include <Engine/CAnimation3D.h>

#include "FSMNode.h"


map<wstring, CAnimation3D*> CLinkAnimScript::m_mapAnim = {};
map<wstring, tAnimNode*> CLinkAnimScript::m_mapAnimNode = {};
bool CLinkAnimScript::m_bIsAnimNodeLoaded = false;

CLinkAnimScript::CLinkAnimScript()
	: CScript(SCRIPT_TYPE::LINKANIMSCRIPT)
	, m_pAnimator(nullptr)
	, m_pCurAnimNode(nullptr)
	, m_pNextAnimNode(nullptr)
	, m_iCond(0)
	, m_fAnglePerSec(XM_2PI)
{
}

CLinkAnimScript::CLinkAnimScript(const CLinkAnimScript& _origin)
	: CScript(_origin)
	, m_pAnimator(nullptr)
	, m_pCurAnimNode(nullptr)
	, m_pNextAnimNode(nullptr)
	, m_iCond(0)
	, m_fAnglePerSec(_origin.m_fAnglePerSec)
{
}

CLinkAnimScript::~CLinkAnimScript()
{
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
}

void CLinkAnimScript::tick()
{
	PlayNextAnim();
	OperateAnimFunc();
	SetLinkCond();
	SetNextAnim();
}

void CLinkAnimScript::PlayNextAnim()
{
	if (m_pNextAnimNode != nullptr)
	{
		if (CalBit(m_pNextAnimNode->iPreferences, LAP_REPEAT, BIT_FUNC_OPT::BIT_LEAST_ONE))
			m_pAnimator->Play(m_pNextAnimNode->pAnimKey, true);
		else
			m_pAnimator->Play(m_pNextAnimNode->pAnimKey, false);

		m_pCurAnimNode = m_pNextAnimNode;
		m_pNextAnimNode = nullptr;
	}
}

void CLinkAnimScript::OperateAnimFunc()
{
	if (m_pCurAnimNode->pAnimKey == LINK_ANIM_WCHAR[LAT_WALK])
		Func_Walk();
}

void CLinkAnimScript::SetLinkCond()
{
	m_iCond = 0;

	if (KEY_PRESSED(KEY::W) || KEY_PRESSED(KEY::S) || KEY_PRESSED(KEY::A) || KEY_PRESSED(KEY::D))
		AddBit(m_iCond, LAC_KEY_WSAD);
}

void CLinkAnimScript::SetNextAnim()
{
	size_t iTranCount = m_pCurAnimNode->vecTranAnim.size();

	for (size_t i = 0; i < iTranCount; ++i)
	{
		UINT iCmpBit = m_iCond;

		RemoveBit(iCmpBit, m_pCurAnimNode->vecTranAnim[i]->iExcludeCond);

		if (CalBit(iCmpBit, m_pCurAnimNode->vecTranAnim[i]->iTranCond, BIT_EQUAL))
		{
			m_pNextAnimNode = m_mapAnimNode.find(m_pCurAnimNode->vecTranAnim[i]->pTargetAnimKey)->second;
			return;
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
}

void CLinkAnimScript::LoadFromYAML(YAML::Node& _node)
{
	CScript::LoadFromYAML(_node);
}


