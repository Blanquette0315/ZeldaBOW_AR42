#include "pch.h"
#include "CCamScript.h"

#include "CCameraMgr.h"

CCamScript::CCamScript()
	: CScript(SCRIPT_TYPE::CAMSCRIPT)
	, m_eCameraType(CAMERA_SELECTION::END)
{
}

CCamScript::CCamScript(SCRIPT_TYPE _type)
	: CScript(_type)
	, m_eCameraType(CAMERA_SELECTION::END)
{
}

CCamScript::CCamScript(const CCamScript& _origin)
	: CScript(_origin)
	, m_eCameraType(_origin.m_eCameraType)
{
}

CCamScript::~CCamScript()
{
	CCameraMgr::GetInst()->DeregisterCamera(GetOwner(), m_eCameraType);
}

void CCamScript::RegisterCamera()
{
	CCameraMgr::GetInst()->RegisterCamera(GetOwner(), m_eCameraType);
}

void CCamScript::begin()
{
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

	_emitter << YAML::Key << "CameraType";
	_emitter << YAML::Value << (UINT)m_eCameraType;
}

void CCamScript::LoadFromYAML(YAML::Node& _node)
{
	CScript::LoadFromYAML(_node);

	UINT iCamType = (UINT)CAMERA_SELECTION::END;
	SAFE_LOAD_FROM_YAML(UINT, iCamType, _node["CameraType"]);
	m_eCameraType = (CAMERA_SELECTION)iCamType;
}


