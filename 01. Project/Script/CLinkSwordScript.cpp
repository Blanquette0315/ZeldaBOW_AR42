#include "pch.h"
#include "CLinkSwordScript.h"

#include "FSMNode.h"
#include "CLinkAnimScript.h"
#include "CMonsterScript.h"

CLinkSwordScript::CLinkSwordScript()
	: CScript(SCRIPT_TYPE::LINKSWORDSCRIPT)
	, m_pLinkAnimScr(nullptr)
{
	AddScriptParam(SCRIPT_PARAM::PREFAB, "Attack Effect", &m_AttackEffectPref);

	AddScriptParam(SCRIPT_PARAM::FLOAT, "Boko-Offset", &m_arrOffset[(UINT)MONSTER_OFFSET::BOKO], 0.f, 100.f, 1.f);
}

CLinkSwordScript::CLinkSwordScript(const CLinkSwordScript& _origin)
	: CScript(_origin)
	, m_AttackEffectPref(_origin.m_AttackEffectPref)
{
	AddScriptParam(SCRIPT_PARAM::PREFAB, "Attack Effect", &m_AttackEffectPref);

	AddScriptParam(SCRIPT_PARAM::FLOAT, "Boko-Offset", &m_arrOffset[(UINT)MONSTER_OFFSET::BOKO], 0.f, 100.f, 1.f);
}

CLinkSwordScript::~CLinkSwordScript()
{
	
}

bool CLinkSwordScript::IsAttackAnim()
{
	return CalBit(m_pLinkAnimScr->GetCurAnimNode()->GetPreference(), LAP_ATTACK, BIT_LEAST_ONE);
}

void CLinkSwordScript::AttackEffect(CMonsterScript* _pMonsterScr)
{
	if (m_AttackEffectPref.Get())
	{
		CTransform* pMonsterTrans = _pMonsterScr->GetOwner()->Transform();
		CGameObject* pEffectObj = m_AttackEffectPref->Instantiate();

		CTransform* pLinkTrans = GetOwner()->GetParent()->Transform();
		Vec3 vRot = pLinkTrans->GetRelativeRotation();

		Vec3 vRandom = Vec3((float)g_random(g_gen) / 400.f, (float)g_random(g_gen) / 400.f, (float)g_random(g_gen) / 400.f);
		Vec3 vDir = (pLinkTrans->GetRelativePos() - pMonsterTrans->GetRelativePos() + vRandom).Normalize();
		Vec3 vInstPos = pMonsterTrans->GetRelativePos() + vDir * m_arrOffset[(UINT)MONSTER_OFFSET::BOKO];

		vInstPos.y += 13.f;
		pEffectObj->Transform()->SetRelativeRotation(vRot);
		Instantiate(pEffectObj, vInstPos, 0);
	}
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
			AttackEffect(pMonsterScr);
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
			pMonsterScr->Damage(1);
			AttackEffect(pMonsterScr);
		}
	}
}

void CLinkSwordScript::EndOverlap(CGameObject* _pOther)
{
}

void CLinkSwordScript::SaveToYAML(YAML::Emitter& _emitter)
{
	CScript::SaveToYAML(_emitter);
	SaveResourceRefEX(m_AttackEffectPref, _emitter, "AttackEffect");
	
	_emitter << YAML::Key << "BokoOffset";
	_emitter << YAML::Value << m_arrOffset[(UINT)MONSTER_OFFSET::BOKO];
}

void CLinkSwordScript::LoadFromYAML(YAML::Node& _node)
{
	CScript::LoadFromYAML(_node);
	LoadResourceRefEX(m_AttackEffectPref, _node, "AttackEffect");

	SAFE_LOAD_FROM_YAML(float, m_arrOffset[(UINT)MONSTER_OFFSET::BOKO], _node["BokoOffset"]);
}


