#include "pch.h"
#include "CUIHeartScript.h"

#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>

#include "CLinkAnimScript.h"

CUIHeartScript::CUIHeartScript()
	: CUIScript(SCRIPT_TYPE::UIHEARTSCRIPT)
{
}

CUIHeartScript::CUIHeartScript(const CUIHeartScript& _origin)
	: CUIScript(_origin)
{
}

CUIHeartScript::~CUIHeartScript()
{
}

void CUIHeartScript::ReflectHP(int _iHP)
{
	int iHeartIdx = _iHP / 4;
	int iHeartRemain = _iHP % 4;

	for (int i = iHeartIdx + 1; i < m_vecHeartObj.size(); ++i)
	{
		static int iFullDamaged = 4;
		m_vecHeartObj[i]->MeshRender()->GetDynamicMaterial(0)->SetScalarParam(SCALAR_PARAM::INT_0, &iFullDamaged);
	}

	if(iHeartRemain != 0)
		m_vecHeartObj[iHeartIdx]->MeshRender()->GetDynamicMaterial(0)->SetScalarParam(SCALAR_PARAM::INT_0, &iHeartRemain);
}

void CUIHeartScript::begin()
{
	//CUIScript::begin();
	CGameObject* pLinkObj = CLevelMgr::GetInst()->GetCurLevel()->FindObjectByName(LINK_STRING_WCHAR[LINK_STRING_LINK]);
	m_pLink = pLinkObj->GetScript<CLinkAnimScript>();
	m_vecHeartObj = GetOwner()->GetChildObject();
	Show();
}

void CUIHeartScript::tick()
{
	CUIScript::tick();
	// if HP chnaged
	if (m_bReflect)
	{
		ReflectHP(m_pLink->GetLinkStatus().iHP);
		m_bReflect = false;
	}
}

void CUIHeartScript::BeginOverlap(CGameObject* _pOther)
{
}

void CUIHeartScript::Overlap(CGameObject* _pOther)
{
}

void CUIHeartScript::EndOverlap(CGameObject* _pOther)
{
}

void CUIHeartScript::SaveToYAML(YAML::Emitter& _emitter)
{
	CUIScript::SaveToYAML(_emitter);
}

void CUIHeartScript::LoadFromYAML(YAML::Node& _node)
{
	CUIScript::LoadFromYAML(_node);
}


