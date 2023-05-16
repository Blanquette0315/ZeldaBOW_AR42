#include "pch.h"
#include "CLinkHitScript.h"

CLinkHitScript::CLinkHitScript()
	: CScript(SCRIPT_TYPE::LINKHITSCRIPT)
{
	AddScriptParam(SCRIPT_PARAM::INT, "Hit Type", &m_eType);
}

CLinkHitScript::CLinkHitScript(const CLinkHitScript& _origin)
	: CScript(_origin)
	, m_eType(_origin.m_eType)
{
	AddScriptParam(SCRIPT_PARAM::INT, "Hit Type", &m_eType);
}

CLinkHitScript::~CLinkHitScript()
{
}

void CLinkHitScript::begin()
{
}

void CLinkHitScript::tick()
{

}

void CLinkHitScript::BeginOverlap(CGameObject* _pOther)
{
}

void CLinkHitScript::Overlap(CGameObject* _pOther)
{
}

void CLinkHitScript::EndOverlap(CGameObject* _pOther)
{
}

void CLinkHitScript::SaveToYAML(YAML::Emitter& _emitter)
{
	CScript::SaveToYAML(_emitter);

	_emitter << YAML::Key << "Type";
	_emitter << YAML::Value << m_eType;
}

void CLinkHitScript::LoadFromYAML(YAML::Node& _node)
{
	CScript::LoadFromYAML(_node);

	SAFE_LOAD_FROM_YAML(int, m_eType, _node["Type"]);
}


