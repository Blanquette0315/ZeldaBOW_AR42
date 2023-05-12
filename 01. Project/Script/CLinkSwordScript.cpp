#include "pch.h"
#include "CLinkSwordScript.h"

#include "FSMNode.h"
#include "CLinkAnimScript.h"
#include "CMonsterScript.h"

CLinkSwordScript::CLinkSwordScript()
	: CScript(SCRIPT_TYPE::LINKSWORDSCRIPT)
	, m_pLinkAnimScr(nullptr)
{
}

CLinkSwordScript::~CLinkSwordScript()
{
}

bool CLinkSwordScript::IsAttackAnim()
{
	return CalBit(m_pLinkAnimScr->GetCurAnimNode()->GetPreference(), LAP_ATTACK, BIT_LEAST_ONE);
}

void CLinkSwordScript::begin()
{
	m_pLinkAnimScr = GetOwner()->GetParent()->GetScript<CLinkAnimScript>();
}

void CLinkSwordScript::tick()
{
	if (m_pLinkAnimScr->IsAnimChanged())
	{
		m_vecObjHit.clear();
	}
}

void CLinkSwordScript::BeginOverlap(CGameObject* _pOther)
{
	if (IsAttackAnim())
	{
		CMonsterScript* pMonsterScr = _pOther->GetScript<CMonsterScript>();
		if (pMonsterScr == nullptr)
			return;
		assert(pMonsterScr); // not monster

		bool bCheck = true;

		for (int i = 0; i < m_vecObjHit.size(); ++i)
		{
			if (m_vecObjHit[i] == pMonsterScr->GetOwner())
			{
				bCheck = false;
				break;
			}
		}

		if (bCheck)
		{
			m_vecObjHit.push_back(_pOther);

			// Set Damage to Monster Here
			pMonsterScr->Damage(2);
		}
	}
}

void CLinkSwordScript::Overlap(CGameObject* _pOther)
{
	if (IsAttackAnim())
	{
		CMonsterScript* pMonsterScr = _pOther->GetScript<CMonsterScript>();
		if (pMonsterScr == nullptr)
			return;

		assert(pMonsterScr); // not monster

		bool bCheck = true;

		for (int i = 0; i < m_vecObjHit.size(); ++i)
		{
			if (m_vecObjHit[i] == pMonsterScr->GetOwner())
			{
				bCheck = false;
				break;
			}
		}

		if (bCheck)
		{
			m_vecObjHit.push_back(_pOther);

			// Set Damage to Monster Here
			pMonsterScr->Damage(2);
		}
	}
}

void CLinkSwordScript::EndOverlap(CGameObject* _pOther)
{

}

void CLinkSwordScript::SaveToYAML(YAML::Emitter& _emitter)
{
	CScript::SaveToYAML(_emitter);
}

void CLinkSwordScript::LoadFromYAML(YAML::Node& _node)
{
	CScript::LoadFromYAML(_node);
}


