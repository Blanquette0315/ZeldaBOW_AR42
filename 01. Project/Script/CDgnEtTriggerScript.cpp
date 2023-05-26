#include "pch.h"
#include "CDgnEtTriggerScript.h"

#include "CLinkAnimScript.h"

CDgnEtTriggerScript::CDgnEtTriggerScript()
	: CScript(SCRIPT_TYPE::DGNETTRIGGERSCRIPT)
	, m_bAnimPlay(false)
{
}

CDgnEtTriggerScript::~CDgnEtTriggerScript()
{
}

void CDgnEtTriggerScript::begin()
{
}

void CDgnEtTriggerScript::tick()
{
}

void CDgnEtTriggerScript::BeginOverlap(CGameObject* _pOther)
{
	if (_pOther->GetScript<CLinkAnimScript>())
	{
		GetOwner()->GetParent()->Animator3D()->Play(L"Enterance_Open", false);
	}
}

void CDgnEtTriggerScript::Overlap(CGameObject* _pOther)
{
}

void CDgnEtTriggerScript::EndOverlap(CGameObject* _pOther)
{
}

void CDgnEtTriggerScript::SaveToYAML(YAML::Emitter& _emitter)
{
}

void CDgnEtTriggerScript::LoadFromYAML(YAML::Node& _node)
{
}