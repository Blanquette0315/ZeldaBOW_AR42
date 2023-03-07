#include "pch.h"
#include "CEntity.h"

UINT CEntity::g_ID = 0;

CEntity::CEntity()
	: m_ID(g_ID++)
{

}

CEntity::CEntity(const CEntity& _other)
	: m_ID(g_ID++)
	, m_strName(_other.m_strName)
{

}

CEntity::~CEntity()
{

}

void CEntity::SaveToYAML(YAML::Emitter& _emitter)
{
	_emitter << YAML::Key << "name";
	_emitter << YAML::Value << WStringToString(m_strName);
}

void CEntity::LoadFromYAML(YAML::Node& _node)
{
	if (_node["name"].IsDefined())
	{
		m_strName = StringToWString(_node["name"].as<string>());
	}
}