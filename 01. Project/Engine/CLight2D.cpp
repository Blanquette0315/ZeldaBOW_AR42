#include "pch.h"
#include "CLight2D.h"

#include "CRenderMgr.h"
#include "CTransform.h"
CLight2D::CLight2D()
	: CComponent(COMPONENT_TYPE::LIGHT2D)
{
	m_Info.iLightType = LIGHT_TYPE::POINT;
}

CLight2D::~CLight2D()
{
}

#include "CKeyMgr.h"
#include "CTimeMgr.h"

void CLight2D::finaltick()
{
	m_Info.vWorldPos = Transform()->GetWorldPos();
	m_Info.vWorldDir = Transform()->GetWorldDir(DIR::RIGHT);

	CRenderMgr::GetInst()->RegisterLight2D(this);
}



void CLight2D::SaveToYAML(YAML::Emitter& _emitter)
{
	_emitter << YAML::Key << "LIGHT2D";
	_emitter << YAML::Value << YAML::BeginMap;

	_emitter << YAML::Key << "Diff";
	_emitter << YAML::Value << m_Info.vDiff;
	_emitter << YAML::Key << "Spec";
	_emitter << YAML::Value << m_Info.vSpec;
	_emitter << YAML::Key << "Emb";
	_emitter << YAML::Value << m_Info.vEmb;
	_emitter << YAML::Key << "LightType";
	_emitter << YAML::Value << (UINT)m_Info.iLightType;
	_emitter << YAML::Key << "Radius";
	_emitter << YAML::Value << m_Info.fRadius;
	_emitter << YAML::Key << "InAngle";
	_emitter << YAML::Value << m_Info.fInAngle;
	_emitter << YAML::Key << "OutAngle";
	_emitter << YAML::Value << m_Info.fOutAngle;

	_emitter << YAML::EndMap;
}

void CLight2D::LoadFromYAML(YAML::Node& _node)
{
	m_Info.vDiff = _node["LIGHT2D"]["Diff"].as<Vec4>();
	m_Info.vSpec = _node["LIGHT2D"]["Spec"].as<Vec4>();
	m_Info.vEmb = _node["LIGHT2D"]["Emb"].as<Vec4>();
	m_Info.iLightType = (LIGHT_TYPE)(_node["LIGHT2D"]["LightType"].as<UINT>());
	m_Info.fRadius = _node["LIGHT2D"]["Radius"].as<float>();
	m_Info.fInAngle = _node["LIGHT2D"]["InAngle"].as<float>();
	m_Info.fOutAngle = _node["LIGHT2D"]["OutAngle"].as<float>();
}