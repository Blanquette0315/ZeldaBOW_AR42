#include "pch.h"
#include "CFlashEffectScript.h"

CFlashEffectScript::CFlashEffectScript()
	: CScript(SCRIPT_TYPE::FLASHEFFECTSCRIPT)
{
	AddScriptParam(SCRIPT_PARAM::FLOAT, "MaxTime", &m_fMaxTime, 0.f, 10.f);
	AddScriptParam(SCRIPT_PARAM::INT, "DestroyOption", &m_iDestroyOption, 0, 10);
}

CFlashEffectScript::CFlashEffectScript(const CFlashEffectScript& _origin)
	: CScript(_origin)
	, m_fMaxTime(_origin.m_fMaxTime)
	, m_iDestroyOption(_origin.m_iDestroyOption)
{
	AddScriptParam(SCRIPT_PARAM::FLOAT, "MaxTime", &m_fMaxTime, 0.f, 10.f);
	AddScriptParam(SCRIPT_PARAM::INT, "DestroyOption", &m_iDestroyOption, 0, 10);
}

CFlashEffectScript::~CFlashEffectScript()
{
}

void CFlashEffectScript::begin()
{
	m_fAccTime = 0.f;
	GetOwner()->GetRenderComponent()->GetDynamicMaterial()->GetScalarParam(SCALAR_PARAM::VEC4_0, &m_vStartColor);
}

void CFlashEffectScript::tick()
{
	if (m_fAccTime >= m_fMaxTime)
	{
		m_fAccTime = m_fMaxTime;
		if (IsValid(GetOwner()))
		{
			if(m_iDestroyOption == 0)
				Destroy();
		}
	}
	else
	{
		m_fAccTime += FDT;
	}
	Vec4 vColor; 
	GetOwner()->GetRenderComponent()->GetDynamicMaterial()->GetScalarParam(SCALAR_PARAM::VEC4_0, &vColor);
	vColor.w = m_vStartColor.w * (1.f - m_fAccTime / m_fMaxTime);
	GetOwner()->GetRenderComponent()->GetDynamicMaterial()->SetScalarParam(SCALAR_PARAM::VEC4_0, &vColor);
}

void CFlashEffectScript::SaveToYAML(YAML::Emitter& _emitter)
{
	CScript::SaveToYAML(_emitter);

	_emitter << YAML::Key << "MaxTime";
	_emitter << YAML::Value << m_fMaxTime;

	_emitter << YAML::Key << "DestroyOption";
	_emitter << YAML::Value << m_iDestroyOption;
}

void CFlashEffectScript::LoadFromYAML(YAML::Node& _node)
{
	CScript::LoadFromYAML(_node);

	SAFE_LOAD_FROM_YAML(float, m_fMaxTime, _node["MaxTime"]);
	SAFE_LOAD_FROM_YAML(int, m_iDestroyOption, _node["DestroyOption"]);
}



