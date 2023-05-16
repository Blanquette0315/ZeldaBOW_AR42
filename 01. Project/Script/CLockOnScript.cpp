#include "pch.h"
#include "CLockOnScript.h"

#include <Engine/CGameObject.h>

#include "CLinkAnimScript.h"
#include "FSMNode.h"

#include "CLinkCamScript.h"
#include "CMonsterScript.h"

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
	Collider()->SetColliderType(COLLIDER_TYPE::COLLIDER_SPHERE);
	Collider()->SetRadius(m_fColRadius);
	m_pLink = GetOwner()->GetParent();
	m_fDistFromTarget = m_fColRadius;
}

void CLockOnScript::begin()
{

}

void CLockOnScript::tick()
{
}

void CLockOnScript::BeginOverlap(CGameObject* _pOther)
{
}

void CLockOnScript::Overlap(CGameObject* _pOther)
{
	if (_pOther->GetScript<CMonsterScript>() == nullptr)
		return;

	float fDistFromTarget = (m_pLink->Transform()->GetRelativePos() - _pOther->Transform()->GetRelativePos()).Length();
	if (fDistFromTarget < m_fDistFromTarget)
	{
		m_pLockOnTarget = _pOther;
		m_fDistFromTarget = fDistFromTarget;
	}
}

void CLockOnScript::EndOverlap(CGameObject* _pOther)
{
	if (_pOther->GetScript<CMonsterScript>() == nullptr)
		return;

	if (_pOther == m_pLockOnTarget)
	{
		ClearTarget();
		RemoveBit(m_pLink->GetScript<CLinkAnimScript>()->GetLinkModeRef(), LINK_MODE::LINK_MODE_LOCKON);
		// m_pLink->GetScript<CLinkAnimScript>()->GetLinkCam()->GetScript<CLinkCamScript>()->SetMode(LINK_CAM_MODE::GENERAL_START);
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


