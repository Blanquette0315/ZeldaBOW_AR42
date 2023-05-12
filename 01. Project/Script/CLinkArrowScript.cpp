#include "pch.h"
#include "CLinkArrowScript.h"

CLinkArrowScript::CLinkArrowScript()
	: CScript(SCRIPT_TYPE::LINKARROWSCRIPT)
{
}

CLinkArrowScript::~CLinkArrowScript()
{
}

void CLinkArrowScript::begin()
{

}

void CLinkArrowScript::tick()
{
}

void CLinkArrowScript::BeginOverlap(CGameObject* _pOther)
{
}

void CLinkArrowScript::Overlap(CGameObject* _pOther)
{
}

void CLinkArrowScript::EndOverlap(CGameObject* _pOther)
{
}

void CLinkArrowScript::SaveToYAML(YAML::Emitter& _emitter)
{
	SaveToYAML(_emitter);
}

void CLinkArrowScript::LoadFromYAML(YAML::Node& _node)
{
	LoadFromYAML(_node);
}


