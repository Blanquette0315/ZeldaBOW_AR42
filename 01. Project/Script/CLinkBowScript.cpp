#include "pch.h"
#include "CLinkBowScript.h"

#include <Engine/CGameObject.h>

#include "FSMNode.h"
#include "CLinkAnimScript.h"
#include "CBonesocketScript.h"


CLinkBowScript::CLinkBowScript()
	: CScript(SCRIPT_TYPE::LINKBOWSCRIPT)
	, m_pLinkAnimScr(nullptr)
	, m_pArrowObj(nullptr)
	, m_bOnce(false)
	, m_bOneFrameAfter(false)
{
	AddScriptParam(SCRIPT_PARAM::PREFAB, "ArrowPref", &m_pArrowPref);
}

CLinkBowScript::CLinkBowScript(const CLinkBowScript& _origin)
	: CScript(_origin)
	, m_pLinkAnimScr(nullptr)
	, m_pArrowObj(nullptr)
	, m_pArrowPref(_origin.m_pArrowPref)
	, m_bOnce(false)
	, m_bOneFrameAfter(false)
{
	AddScriptParam(SCRIPT_PARAM::PREFAB, "ArrowPref", &m_pArrowPref);
}

CLinkBowScript::~CLinkBowScript()
{
}

void CLinkBowScript::begin()
{
	m_pLinkAnimScr = GetOwner()->GetParent()->GetScript<CLinkAnimScript>();
}

void CLinkBowScript::tick()
{
	tAnimNode* pCurAnimNode = m_pLinkAnimScr->GetCurAnimNode();

	// Charge : Create Child Arrow
	if (pCurAnimNode->GetKey() == LINK_ANIM_WCHAR[LAT_BOW_ATTACK_CHARGE])
	{
		if (!IsValid(m_pArrowObj))
		{
			m_pArrowObj = m_pArrowPref->Instantiate();
			Instantiate(m_pArrowObj, Vec3::Zero, 0);
			AddChild(GetOwner()->GetParent(), m_pArrowObj);
			m_pArrowObj->GetScript<CBonesocketScript>()->setBoneIdx((UINT)LINK_BONE_STRING::Weapon_R);
			m_bOnce = true;
		}
	}

	// Shoot : 
	else if (pCurAnimNode->GetKey() == LINK_ANIM_WCHAR[LAT_BOW_ATTACK_SHOOT])
	{
		if (m_bOneFrameAfter)
		{
			m_pArrowObj->Transform()->SetRelativeScale(m_vArrowWorldScale);
			m_pArrowObj->Transform()->SetRelativeRotation(m_vArrowWorldRot);
			m_pArrowObj->Transform()->SetRelativePos(m_vArrowWorldPos);
			m_pArrowObj = nullptr;
			m_bOneFrameAfter = false;
		}

		if (m_bOnce)
		{
			m_pArrowObj->GetScript<CBonesocketScript>()->SetDisable(true);
			m_vArrowWorldScale = m_pArrowObj->Transform()->GetWorldScale();
			m_vArrowWorldRot = m_pArrowObj->Transform()->GetWorldRotation();
			m_vArrowWorldPos = m_pArrowObj->Transform()->GetWorldPos();
			MakeParent(m_pArrowObj);
			
			m_bOnce = false;
			m_bOneFrameAfter = true;
		}
	}

	// Else Animation : Delete Arrow
	else
	{
		if (m_pArrowObj)
		{
			m_pArrowObj->Destroy();
			m_pArrowObj = nullptr;
		}
	}


}

void CLinkBowScript::BeginOverlap(CGameObject* _pOther)
{
}

void CLinkBowScript::Overlap(CGameObject* _pOther)
{
}

void CLinkBowScript::EndOverlap(CGameObject* _pOther)
{
}

void CLinkBowScript::SaveToYAML(YAML::Emitter& _emitter)
{
	CScript::SaveToYAML(_emitter);
	SaveResourceRef(m_pArrowPref, _emitter);
}

void CLinkBowScript::LoadFromYAML(YAML::Node& _node)
{
	CScript::LoadFromYAML(_node);
	LoadResourceRef(m_pArrowPref, _node);
}


