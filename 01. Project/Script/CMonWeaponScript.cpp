#include "pch.h"
#include "CMonWeaponScript.h"

#include "CLinkAnimScript.h"

CMonWeaponScript::CMonWeaponScript()
	: CScript(MONWEAPONSCRIPT)
	, m_iDamage(0)
	, m_bReady(false)
{
}

CMonWeaponScript::~CMonWeaponScript()
{
}

void CMonWeaponScript::tick()
{
}

void CMonWeaponScript::BeginOverlap(CGameObject* _pOther)
{
	if (m_bReady)
	{
		//damage
		CLinkAnimScript* pLinkScr = _pOther->GetScript<CLinkAnimScript>();
		if (pLinkScr == nullptr)
			return;
		tLinkDamaged damage;
		damage.fDamage = m_iDamage;
		damage.eType = LINK_DAMAGED_TYPE::SMALL;
		pLinkScr->SetDamage(damage);
		m_bReady = false;
	}
}

void CMonWeaponScript::Overlap(CGameObject* _pOther)
{
	if (m_bReady)
	{
		//damage
		CLinkAnimScript* pLinkScr = _pOther->GetScript<CLinkAnimScript>();
		if (pLinkScr == nullptr)
			return;
		tLinkDamaged damage;
		damage.fDamage = m_iDamage;
		damage.eType = LINK_DAMAGED_TYPE::SMALL;
		pLinkScr->SetDamage(damage);
		m_bReady = false;
	}
}

void CMonWeaponScript::EndOverlap(CGameObject* _pOther)
{
}

void CMonWeaponScript::SaveToYAML(YAML::Emitter& _emitter)
{
	CScript::SaveToYAML(_emitter);
	_emitter << YAML::Key << "Damage";
	_emitter << YAML::Value << m_iDamage;
}

void CMonWeaponScript::LoadFromYAML(YAML::Node& _node)
{
	CScript::LoadFromYAML(_node);
	SAFE_LOAD_FROM_YAML(UINT, m_iDamage, _node["Damage"]);
}

