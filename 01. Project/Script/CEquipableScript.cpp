#include "pch.h"
#include "CEquipableScript.h"

#include <Engine/CGameObject.h>

CEquipableScript::CEquipableScript()
	: CScript(SCRIPT_TYPE::EQUIPABLESCRIPT)
	, m_iOption(0)
{
}

CEquipableScript::CEquipableScript(const CEquipableScript& _origin)
	: CScript(_origin)
	, m_iOption(0)
{
}

CEquipableScript::~CEquipableScript()
{

}

void CEquipableScript::begin()
{
	m_pOwner = GetOwner()->GetParent();
	assert(m_pOwner); // Owenr should be child
}

void CEquipableScript::tick()
{

}

void CEquipableScript::finaltick()
{
	
}

void CEquipableScript::BeginOverlap(CGameObject* _pOther)
{
}

void CEquipableScript::Overlap(CGameObject* _pOther)
{
}

void CEquipableScript::EndOverlap(CGameObject* _pOther)
{
}

void CEquipableScript::SaveToYAML(YAML::Emitter& _emitter)
{
}

void CEquipableScript::LoadFromYAML(YAML::Node& _node)
{
}


