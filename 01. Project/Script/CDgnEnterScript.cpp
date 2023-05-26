#include "pch.h"
#include "CDgnEnterScript.h"

#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>
#include "CLinkAnimScript.h"

LOAD_LEVEL CDgnEnterScript::Load_Level_Func = nullptr;

CDgnEnterScript::CDgnEnterScript()
	: CScript(SCRIPT_TYPE::DGNENTERSCRIPT)
	, m_fAccTime(0.f)
	, m_bNextLevel(false)
{
}

CDgnEnterScript::~CDgnEnterScript()
{
}

void CDgnEnterScript::begin()
{
}

void CDgnEnterScript::tick()
{
	if (m_bNextLevel)
	{
		m_fAccTime += FDT;
		float fRatio = m_fAccTime / 0.8f;

		if (1.f <= fRatio)
		{
			CLevel* pNextLevel = nullptr;
			pNextLevel = Load_Level_Func(L"level\\Dgn80.lv");
			ChangeLevel(pNextLevel);
		}
	}
}

void CDgnEnterScript::BeginOverlap(CGameObject* _pOther)
{
	if (_pOther->GetScript<CLinkAnimScript>())
	{
		tCamEffectInfo tCamEff = {};
		tCamEff.eEffect = CAM_EFFECT::FADE_OUT;
		tCamEff.fAccTime = 0;
		tCamEff.fDuration = 0.8;
		
		CGameObject* pCam = CLevelMgr::GetInst()->GetCurLevel()->FindObjectByName(L"MainCamera");
		pCam->Camera()->SetCamEffect(tCamEff);

		m_bNextLevel = true;
	}
}

void CDgnEnterScript::Overlap(CGameObject* _pOther)
{
}

void CDgnEnterScript::EndOverlap(CGameObject* _pOther)
{
}

void CDgnEnterScript::SaveToYAML(YAML::Emitter& _emitter)
{
}

void CDgnEnterScript::LoadFromYAML(YAML::Node& _node)
{
}