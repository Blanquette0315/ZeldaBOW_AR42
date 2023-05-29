#include "pch.h"
#include "CTrailEffectScript.h"

#include <Engine/CTrailEffect.h>
#include <Engine/CKeyMgr.h>
#include <Engine/CAnimation3D.h>

#include "CLinkAnimScript.h"
#include "FSMNode.h"

CTrailEffectScript::CTrailEffectScript()
	: CScript(SCRIPT_TYPE::TRAILEFFECTSCRIPT)
{
	AddScriptParam(SCRIPT_PARAM::FLOAT, "FirstOffsetY", &(m_vFirstDotOffset.y), -100.f, 100.f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "SecondOffsetY", &(m_vSecondDotOffset.y), -100.f, 100.f);
}

CTrailEffectScript::CTrailEffectScript(const CTrailEffectScript& _origin)
	: CScript(_origin)
{
	AddScriptParam(SCRIPT_PARAM::FLOAT, "FirstOffsetY", &(m_vFirstDotOffset.y), -100.f, 100.f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "SecondOffsetY", &(m_vSecondDotOffset.y), -100.f, 100.f);
}

CTrailEffectScript::~CTrailEffectScript()
{
}

void CTrailEffectScript::begin()
{
	m_pLinkAnimScr = GetOwner()->GetParent()->GetParent()->GetScript<CLinkAnimScript>();
}

void CTrailEffectScript::tick()
{
	if (CalBit(m_pLinkAnimScr->GetCurAnimNode()->iPreferences, LAP_ATTACK, BIT_LEAST_ONE) 
		&& !m_pLinkAnimScr->IsAnimChanged())
	{
		tTwoDot tTD;
		tTD.vFirstDot = (Vec3)XMVector3TransformCoord(m_vFirstDotOffset, GetOwner()->Transform()->GetWorldMat());
		tTD.vSecondDot = (Vec3)XMVector3TransformCoord(m_vSecondDotOffset, GetOwner()->Transform()->GetWorldMat());
		TrailEffect()->PushData(tTD);
		float a = m_pLinkAnimScr->GetCurAnimNode()->pAnim->GetRatioEX();
		TrailEffect()->SetRatio(m_pLinkAnimScr->GetCurAnimNode()->pAnim->GetRatioEX());
		
		
	}
	else
	{
		TrailEffect()->Clear();
	}
}

void CTrailEffectScript::SaveToYAML(YAML::Emitter& _emitter)
{
	CScript::SaveToYAML(_emitter);
	_emitter << YAML::Key << "FarOffset";
	_emitter << YAML::Value << m_vFirstDotOffset;

	_emitter << YAML::Key << "NearOffset";
	_emitter << YAML::Value << m_vSecondDotOffset;
}

void CTrailEffectScript::LoadFromYAML(YAML::Node& _node)
{
	CScript::LoadFromYAML(_node);

	SAFE_LOAD_FROM_YAML(Vec3, m_vFirstDotOffset, _node["FarOffset"]);
	SAFE_LOAD_FROM_YAML(Vec3, m_vSecondDotOffset, _node["NearOffset"]);
}



