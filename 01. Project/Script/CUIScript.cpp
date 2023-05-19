#include "pch.h"
#include "CUIScript.h"

CUIScript::CUIScript()
	: CScript(SCRIPT_TYPE::UISCRIPT)
{
}

CUIScript::CUIScript(SCRIPT_TYPE _eType)
	: CScript(_eType)
{
}

CUIScript::CUIScript(const CUIScript& _origin)
	: CScript(_origin)
{
}

CUIScript::~CUIScript()
{
}

void CUIScript::Hide()
{
	if(GetOwner()->GetRenderComponent())
		GetOwner()->GetRenderComponent()->SetRender(false);
	else
	{
		for (int i = 0; i < GetOwner()->GetChildObject().size(); ++i)
			GetOwner()->GetChildObject()[i]->GetRenderComponent()->SetRender(false);
	}
}

void CUIScript::Show()
{
	if (GetOwner()->GetRenderComponent())
		GetOwner()->GetRenderComponent()->SetRender(true);
	else
	{
		for (int i = 0; i < GetOwner()->GetChildObject().size(); ++i)
			GetOwner()->GetChildObject()[i]->GetRenderComponent()->SetRender(true);
	}
}

void CUIScript::begin()
{
	Hide();
}

void CUIScript::tick()
{
}

void CUIScript::BeginOverlap(CGameObject* _pOther)
{
}

void CUIScript::Overlap(CGameObject* _pOther)
{
}

void CUIScript::EndOverlap(CGameObject* _pOther)
{
}

void CUIScript::SaveToYAML(YAML::Emitter& _emitter)
{
	CScript::SaveToYAML(_emitter);
}

void CUIScript::LoadFromYAML(YAML::Node& _node)
{
	CScript::LoadFromYAML(_node);
}


