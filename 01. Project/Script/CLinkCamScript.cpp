#include "pch.h"
#include "CLinkCamScript.h"
 
#include <Engine/CKeyMgr.h>

#include <Engine/CTransform.h>

CLinkCamScript::CLinkCamScript()
	: CCamScript(SCRIPT_TYPE::LINKCAMSCRIPT)
	, m_fDistFromLink(200.f)
	, m_fFrontDistFromLink(100.f)
	, m_eMode(LINK_CAM_MODE::GENERAL)
	, m_pLockOnObj(nullptr)
	, m_fSpeedLockOnZoomIn(10.f)
	, m_bLockOn(false)
	, m_bLockOnStart(false)
{
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Distance", &m_fDistFromLink, 0.f, 1000.f, 0.5f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Distance Min", &m_fDistFromLinkMin, 0.f, 1000.f, 0.5f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Front Distance", &m_fFrontDistFromLink, 0.f, 1000.f, 0.5f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Speed LockOn", &m_fSpeedLockOnZoomIn, 0.f, 1000.f, 0.5f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Speed LockOn Rad", &m_fSpeedLockOnRad, 0.f, 3.14f, 0.01f);
}

CLinkCamScript::CLinkCamScript(const CLinkCamScript& _origin)
	: CCamScript(_origin)
	, m_fDistFromLink(_origin.m_fDistFromLink)
	, m_fFrontDistFromLink(_origin.m_fFrontDistFromLink)
	, m_eMode(LINK_CAM_MODE::GENERAL)
	, m_pLockOnObj(nullptr)
	, m_bLockOn(false)
	, m_bLockOnStart(false)
{
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Distance", &m_fDistFromLink, 0.f, 200.f, 0.5f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Distance Min", &m_fDistFromLinkMin, 0.f, 1000.f, 0.5f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Front Distance", &m_fFrontDistFromLink, 0.f, 200.f, 0.5f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Speed LockOn", &m_fSpeedLockOnZoomIn, 0.f, 1000.f, 0.5f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Speed LockOn Rad", &m_fSpeedLockOnRad, 0.f, 3.14f, 0.01f);
}

CLinkCamScript::~CLinkCamScript()
{
}

void CLinkCamScript::GeneralMove()
{
	if (KEY_PRESSED(KEY::RBTN))
	{
		Vec2 vMouseDir = CKeyMgr::GetInst()->GetMouseDir();
		Vec3 vRot = Transform()->GetRelativeRotation();

		vRot.y += vMouseDir.x * FDT * XM_PIDIV2;
		vRot.x -= vMouseDir.y * FDT * XM_PIDIV2;

		if (vRot.x > XM_PI / 2.f)
			vRot.x = XM_PI / 9.f * 4.f;
		else if (vRot.x < 0.f)
			vRot.x = 0.f;

		Transform()->SetRelativeRotation(vRot);
	}

	if (m_bLockOn && m_pLockOnObj)
	{		
		// operate once -> when key tapped 
		if (m_bLockOnStart)
		{
			// camera move -> back to the link

			m_bLockOnStart = false;
		}

		// zoom in 
		float fDist = (m_pLinkObj->Transform()->GetRelativePos() - m_pLockOnObj->Transform()->GetRelativePos()).Length();
		float vDistFromLink = (m_fDistFromLink - m_fDistFromLinkMin) * (fDist / m_fMaxDistLockOn);

		m_fDistLockonFromLink = vDistFromLink;

		if (m_fDistFromLink != m_fDistLockonFromLink)
		{
			static float fInterpolation = 10.f;
			m_fDistFromLink += (m_fDistLockonFromLink - m_fDistFromLink) / fInterpolation;
		}
	}

	Transform()->finaltick(); // should recalculate dir

	Vec3 vDirToCamFromObj = -Transform()->GetRelativeDir(DIR::FRONT);
	Vec3 vDirToCamFromObjXZ = Vec3(-vDirToCamFromObj.x, 0.f, -vDirToCamFromObj.z).Normalize();
	
	Vec3 vCamPos = m_pLinkObj->Transform()->GetRelativePos() + vDirToCamFromObjXZ * m_fFrontDistFromLink + vDirToCamFromObj * m_fDistFromLink;

	Transform()->SetRelativePos(vCamPos);
}


void CLinkCamScript::init()
{
	m_eCameraType = CAMERA_SELECTION::LINK;
	CCamScript::RegisterCamera();
	m_pLinkObj = FindObjectByName(LINK_STRING_WCHAR[LINK_STRING_LINK]);

	// start : yz 30 degree
	Transform()->SetRelativeRotation(Vec3(XM_PI / 6.f, 0.f, 0.f));
}

void CLinkCamScript::begin()
{

}

void CLinkCamScript::tick()
{
	switch (m_eMode)
	{
	case LINK_CAM_MODE::GENERAL:
	{
		GeneralMove();
	}
		break;
	}
}

void CLinkCamScript::BeginOverlap(CGameObject* _pOther)
{
}

void CLinkCamScript::Overlap(CGameObject* _pOther)
{
}

void CLinkCamScript::EndOverlap(CGameObject* _pOther)
{
}

void CLinkCamScript::SaveToYAML(YAML::Emitter& _emitter)
{
	CCamScript::SaveToYAML(_emitter);

	_emitter << YAML::Key << "DistFromLink";
	_emitter << YAML::Value << m_fDistFromLink;

	_emitter << YAML::Key << "DistFromLinkMin";
	_emitter << YAML::Value << m_fDistFromLinkMin;

	_emitter << YAML::Key << "FrontDistFromLink";
	_emitter << YAML::Value << m_fFrontDistFromLink;

	_emitter << YAML::Key << "Speed lock on zoom in";
	_emitter << YAML::Value << m_fSpeedLockOnZoomIn;

	_emitter << YAML::Key << "Speed lock on radian";
	_emitter << YAML::Value << m_fSpeedLockOnRad;

	_emitter << YAML::Key << "MaxDistFromLockOnObj";
	_emitter << YAML::Value << m_fMaxDistLockOn;
}

void CLinkCamScript::LoadFromYAML(YAML::Node& _node)
{
	CCamScript::LoadFromYAML(_node);

	SAFE_LOAD_FROM_YAML(float, m_fDistFromLink, _node["DistFromLink"]);
	SAFE_LOAD_FROM_YAML(float, m_fDistFromLinkMin, _node["DistFromLinkMin"]);
	SAFE_LOAD_FROM_YAML(float, m_fFrontDistFromLink, _node["FrontDistFromLink"]);
	SAFE_LOAD_FROM_YAML(float, m_fSpeedLockOnZoomIn, _node["Speed lock on zoom in"]);
	SAFE_LOAD_FROM_YAML(float, m_fSpeedLockOnRad, _node["Speed lock on radian"]);
	SAFE_LOAD_FROM_YAML(float, m_fMaxDistLockOn, _node["MaxDistFromLockOnObj"]);
}


