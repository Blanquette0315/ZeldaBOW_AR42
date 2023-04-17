#include "pch.h"
#include "CLinkScript.h"

CLinkScript::CLinkScript()
	: CScript(SCRIPT_TYPE::LINKSCRIPT)
{
}

CLinkScript::~CLinkScript()
{
}

void CLinkScript::begin()
{
	
}

void CLinkScript::tick()
{
}

void CLinkScript::BeginOverlap(CCollider* _pOther)
{
}

void CLinkScript::Overlap(CCollider* _pOther)
{
}

void CLinkScript::EndOverlap(CCollider* _pOther)
{
	
}

void CLinkScript::SaveToYAML(YAML::Emitter& _emitter)
{
	CScript::SaveToYAML(_emitter);
}

void CLinkScript::LoadFromYAML(YAML::Node& _node)
{
	CScript::LoadFromYAML(_node);
}


