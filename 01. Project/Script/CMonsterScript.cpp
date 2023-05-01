#include "pch.h"
#include "CMonsterScript.h"

#include <Engine\CGameObject.h>

CMonsterScript::CMonsterScript(int _iScriptType)
	: CScript(_iScriptType)
	, m_iHP(10)
	, m_eCurrentState(Monster_State::IDLE)
	, m_fAcctime(0.f)
	, m_fSpeed(0.01f)
    , m_iMotion(0)
{
}

CMonsterScript::~CMonsterScript()
{
}

void CMonsterScript::SaveToYAML(YAML::Emitter& _emitter)
{
	CScript::SaveToYAML(_emitter);
	_emitter << YAML::Key << "HP";
	_emitter << YAML::Value << m_iHP;
	_emitter << YAML::Key << "Speed";
	_emitter << YAML::Value << m_fSpeed;
}

void CMonsterScript::LoadFromYAML(YAML::Node& _node)
{
	CScript::LoadFromYAML(_node);
	SAFE_LOAD_FROM_YAML(UINT, m_iHP, _node["HP"]);
	SAFE_LOAD_FROM_YAML(float, m_fSpeed, _node["Speed"]);
}
