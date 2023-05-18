#include "pch.h"
#include "CLinkBombScript.h"

CLinkBombScript::CLinkBombScript()
	: CScript(SCRIPT_TYPE::LINKBOMBSCRIPT)
	, m_fSpeed(5.f)
	, m_fMaxLifeTime(5.f)
{
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Speed", &m_fSpeed, 0.f, 100.f, 0.1f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Max Life", &m_fMaxLifeTime, 0.f, 100.f, 0.1f);
}

CLinkBombScript::CLinkBombScript(const CLinkBombScript& _origin)
	: CScript(_origin)
	, m_fSpeed(_origin.m_fSpeed)
	, m_fMaxLifeTime(_origin.m_fMaxLifeTime)
{
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Speed", &m_fSpeed, 0.f, 100.f, 0.1f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Max Life", &m_fMaxLifeTime, 0.f, 100.f, 0.1f);
}

CLinkBombScript::~CLinkBombScript()
{
}

void CLinkBombScript::begin()
{
	m_bOnce = true;
	m_fAccLifeTime = 0.f;
}

void CLinkBombScript::tick()
{
	if (GetOwner()->GetParent() == nullptr)
	{
		if (m_bOnce)
		{
			RigidBody()->SetFollowingRigid(false);
			RigidBody()->SetVelocity(m_vDir* m_fSpeed);
			m_bOnce = false;
		}

		if (m_fAccLifeTime > m_fMaxLifeTime)
		{
			Destroy();
		}
		else
		{
			m_fAccLifeTime += FDT;
		}
	}
}

void CLinkBombScript::BeginOverlap(CGameObject* _pOther)
{
}

void CLinkBombScript::Overlap(CGameObject* _pOther)
{
}

void CLinkBombScript::EndOverlap(CGameObject* _pOther)
{
}

void CLinkBombScript::SaveToYAML(YAML::Emitter& _emitter)
{
	CScript::SaveToYAML(_emitter);

	_emitter << YAML::Key << "Speed";
	_emitter << YAML::Value << m_fSpeed;

	_emitter << YAML::Key << "Max Life Time";
	_emitter << YAML::Value << m_fMaxLifeTime;
}

void CLinkBombScript::LoadFromYAML(YAML::Node& _node)
{
	CScript::LoadFromYAML(_node);

	SAFE_LOAD_FROM_YAML(float, m_fSpeed, _node["Speed"]);
	SAFE_LOAD_FROM_YAML(float, m_fMaxLifeTime, _node["Max Life Time"]);
}


