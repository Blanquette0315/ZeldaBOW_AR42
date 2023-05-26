#include "pch.h"
#include "CDgnBeginScript.h"

#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>

CDgnBeginScript::CDgnBeginScript()
	: CScript(SCRIPT_TYPE::DGNBEGINSCRIPT)
{
}

CDgnBeginScript::~CDgnBeginScript()
{
}

void CDgnBeginScript::begin()
{
	tCamEffectInfo tCamEff = {};
	tCamEff.eEffect = CAM_EFFECT::FADE_IN;
	tCamEff.fAccTime = 0;
	tCamEff.fDuration = 0.8;

	CGameObject* pCam = CLevelMgr::GetInst()->GetCurLevel()->FindObjectByName(L"MainCamera");
	pCam->Camera()->SetCamEffect(tCamEff);
}

void CDgnBeginScript::tick()
{
}

void CDgnBeginScript::BeginOverlap(CGameObject* _pOther)
{
}

void CDgnBeginScript::Overlap(CGameObject* _pOther)
{
}

void CDgnBeginScript::EndOverlap(CGameObject* _pOther)
{
}

void CDgnBeginScript::SaveToYAML(YAML::Emitter& _emitter)
{
}

void CDgnBeginScript::LoadFromYAML(YAML::Node& _node)
{
}
