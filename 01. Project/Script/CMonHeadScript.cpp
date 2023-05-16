#include "pch.h"
#include "CMonHeadScript.h"

CMonHeadScript::CMonHeadScript()
	: CScript(MONHEADSCRIPT)
	, m_fRadius(0.f)
{
	AddScriptParam(SCRIPT_PARAM::FLOAT, "RADIUS         ", &m_fRadius);
}

CMonHeadScript::CMonHeadScript(const CMonHeadScript& _origin)
	: CScript(MONHEADSCRIPT)
	, m_fRadius(_origin.m_fRadius)
{
	AddScriptParam(SCRIPT_PARAM::FLOAT, "RADIUS         ", &m_fRadius);
}

CMonHeadScript::~CMonHeadScript()
{
}

bool CMonHeadScript::HeadCheck(Vec3 _vPos)
{
	Vec3 vHeadPos = Transform()->GetWorldPos();
	if (m_fRadius >= Vec3::Distance(vHeadPos, _vPos))
		return true;
	else
		return false;
}

void CMonHeadScript::SaveToYAML(YAML::Emitter& _emitter)
{
	CScript::SaveToYAML(_emitter);
	_emitter << YAML::Key << "Radius";
	_emitter << YAML::Value << m_fRadius;
}

void CMonHeadScript::LoadFromYAML(YAML::Node& _node)
{
	CScript::LoadFromYAML(_node);
	SAFE_LOAD_FROM_YAML(UINT, m_fRadius, _node["Radius"]);
}


