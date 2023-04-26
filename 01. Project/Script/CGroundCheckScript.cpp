#include "pch.h"
#include "CGroundCheckScript.h"

#include <Engine/CRigidbody.h>

CGroundCheckScript::CGroundCheckScript()
	: CScript(SCRIPT_TYPE::GROUNDCHECKSCRIPT)
{
}

CGroundCheckScript::CGroundCheckScript(const CGroundCheckScript& _origin)
	: CScript(_origin)
{
}

CGroundCheckScript::~CGroundCheckScript()
{
}

void CGroundCheckScript::tick()
{
	
}

void CGroundCheckScript::begin()
{
	m_bIsGround = false;
}

void CGroundCheckScript::BeginOverlap(CGameObject* _pOther)
{
	if (IsValid(_pOther))
		m_bIsGround = true;
}

void CGroundCheckScript::Overlap(CGameObject* _pOther)
{
	if(IsValid( _pOther))
		m_bIsGround = true;
}

void CGroundCheckScript::EndOverlap(CGameObject* _pOther)
{
	if (IsValid(_pOther))
		m_bIsGround = false;
}

void CGroundCheckScript::SaveToYAML(YAML::Emitter& _emitter)
{
	CScript::SaveToYAML(_emitter);
}

void CGroundCheckScript::LoadFromYAML(YAML::Node& _node)
{
	CScript::LoadFromYAML(_node);
}

