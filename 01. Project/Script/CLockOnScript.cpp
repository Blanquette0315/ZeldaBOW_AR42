#include "pch.h"
#include "CLockOnScript.h"

#include <Engine/CGameObject.h>

CLockOnScript::CLockOnScript()
	: CScript(SCRIPT_TYPE::LOCKONSCRIPT)
	, m_fColRadius(400.f)
{
}

CLockOnScript::CLockOnScript(const CLockOnScript& _origin)
	: CScript(_origin)
	, m_fColRadius(_origin.m_fColRadius)
{
}

CLockOnScript::~CLockOnScript()
{
}

void CLockOnScript::init()
{
}

void CLockOnScript::begin()
{
	Collider()->SetColliderType(COLLIDER_TYPE::COLLIDER_SPHERE);
	Collider()->SetRadius(m_fColRadius);
	m_pLink = GetOwner()->GetParent();
}

void CLockOnScript::tick()
{
}

void CLockOnScript::BeginOverlap(CGameObject* _pOther)
{

}

void CLockOnScript::Overlap(CGameObject* _pOther)
{
	if(IsValid(_pOther))
	{
		 float fDistFromTarget = (m_pLink->Transform()->GetRelativePos() - _pOther->Transform()->GetRelativePos()).Length();
		 if (fDistFromTarget < m_fDistFromTarget)
		 {
			 m_pLockOnTarget = _pOther;
			 m_fDistFromTarget = fDistFromTarget;
		 }
	}
}

void CLockOnScript::EndOverlap(CGameObject* _pOther)
{
	if (IsValid(_pOther))
	{
		if (_pOther == m_pLockOnTarget)
			ClearTarget();
	}
}

void CLockOnScript::SaveToYAML(YAML::Emitter& _emitter)
{
	CScript::SaveToYAML(_emitter);

	_emitter << YAML::Key << "Collider Radius";
	_emitter << YAML::Value << m_fColRadius;
}

void CLockOnScript::LoadFromYAML(YAML::Node& _node)
{
	CScript::LoadFromYAML(_node);

	SAFE_LOAD_FROM_YAML(float, m_fColRadius, _node["Collider Radius"]);
}


