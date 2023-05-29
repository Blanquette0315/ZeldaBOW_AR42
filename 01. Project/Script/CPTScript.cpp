#include "pch.h"
#include "CPTScript.h"

#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>

CPTScript::CPTScript()
	: CScript(PTSCRIPT)
	, m_pMainCam(nullptr)
	, m_bPT(true)
	, m_bToneMapping(true)
	, m_bGamma(true)
	, m_bToonShading(true)
	, m_bShadow(true)
{
}

CPTScript::~CPTScript()
{
}

void CPTScript::begin()
{
	m_pMainCam = CLevelMgr::GetInst()->GetCurLevel()->FindObjectByName(L"MainCamera");
}

void CPTScript::tick()
{
	//if (KEY_TAP(KEY::P))
	//{
	//	m_bPT = !m_bPT;
	//	m_pMainCam->Camera()->SetPTOption_PT(m_bPT);
	//}

	if (m_bPT)
	{
		// All Off
		if (KEY_TAP(KEY::NUM_8))
		{
			m_bToneMapping = !m_bToneMapping;
			m_pMainCam->Camera()->SetPTOption_ToneMapping(m_bToneMapping);
			m_bGamma = !m_bGamma;
			m_pMainCam->Camera()->SetPTOption_Gamma(m_bGamma);
			m_bToonShading = !m_bToonShading;
			m_pMainCam->Camera()->SetPTOption_ToonShading(m_bToonShading);
			m_bShadow = !m_bShadow;
			m_pMainCam->Camera()->SetPTOption_Shadow(m_bShadow);
		}

		// ToneMapping
		if (KEY_TAP(KEY::NUM_0))
		{
			m_bToneMapping = !m_bToneMapping;
			m_pMainCam->Camera()->SetPTOption_ToneMapping(m_bToneMapping);
		}
		// Gamma
		else if (KEY_TAP(KEY::NUM_1))
		{
			m_bGamma = !m_bGamma;
			m_pMainCam->Camera()->SetPTOption_Gamma(m_bGamma);
		}
		// ToonShading
		else if (KEY_TAP(KEY::NUM_2))
		{
			m_bToonShading = !m_bToonShading;
			m_pMainCam->Camera()->SetPTOption_ToonShading(m_bToonShading);
		}
		// Shadow
		else if (KEY_TAP(KEY::NUM_3))
		{
			m_bShadow = !m_bShadow;
			m_pMainCam->Camera()->SetPTOption_Shadow(m_bShadow);
		}
	}
}

void CPTScript::BeginOverlap(CGameObject* _pOther)
{
}

void CPTScript::Overlap(CGameObject* _pOther)
{
}

void CPTScript::EndOverlap(CGameObject* _pOther)
{
}

void CPTScript::SaveToYAML(YAML::Emitter& _emitter)
{
}

void CPTScript::LoadFromYAML(YAML::Node& _node)
{
}
