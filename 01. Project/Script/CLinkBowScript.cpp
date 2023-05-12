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
{
	AddScriptParam(SCRIPT_PARAM::PREFAB, "ArrowPref", &m_pArrowPref);
}

CLinkBowScript::CLinkBowScript(const CLinkBowScript& _origin)
	: CScript(_origin)
	, m_pLinkAnimScr(nullptr)
	, m_pArrowObj(nullptr)
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
		}
	}

	// Shoot : 
	else if (pCurAnimNode->GetKey() == LINK_ANIM_WCHAR[LAT_BOW_ATTACK_SHOOT])
	{

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


