#include "pch.h"
#include "CCamScript.h"

#include "CCameraMgr.h"

CCamScript::CCamScript()
	: CScript(SCRIPT_TYPE::CAMSCRIPT)
{
}

CCamScript::CCamScript(SCRIPT_TYPE _type)
	: CScript(_type)
{
}

CCamScript::CCamScript(const CCamScript& _origin)
	: CScript(_origin)
{
}

CCamScript::~CCamScript()
{
	CCameraMgr::GetInst()->DeregisterCamera(GetOwner());
}

void CCamScript::begin()
{
	CCameraMgr::GetInst()->RegisterCamera(GetOwner());
}

void CCamScript::tick()
{
}

void CCamScript::BeginOverlap(CCollider* _pOther)
{
}

void CCamScript::Overlap(CCollider* _pOther)
{
}

void CCamScript::EndOverlap(CCollider* _pOther)
{
}

void CCamScript::SaveToYAML(YAML::Emitter& _emitter)
{
	CScript::SaveToYAML(_emitter);
}

void CCamScript::LoadFromYAML(YAML::Node& _node)
{
	CScript::LoadFromYAML(_node);
}


