#include "pch.h"
#include "CLinkCamScript.h"
 
#include <Engine/CKeyMgr.h>

#include <Engine/CTransform.h>

#include "CLockOnScript.h"
#include "CLinkAnimScript.h"

CLinkCamScript::CLinkCamScript()
	: CCamScript(SCRIPT_TYPE::LINKCAMSCRIPT)
	, m_fCameraRadiusMax(200.f)
	, m_fFrontDistFromLink(100.f)
	, m_eMode(LINK_CAM_MODE::GENERAL)
	, m_pLockOnObj(nullptr)
	, m_bLockOn(false)
	, m_bLockOnStart(false)
	, m_fLockOnMaxTime(1.f)
	, m_fLockOnAccTime(0.f)
	, m_fMaxDistLockOn(100.f)
	, m_bOnce(true)
	, m_fGeneralAccTime(0.f)
{
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Distance", &m_fCameraRadiusMax, 0.f, 1000.f, 0.5f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Distance Min", &m_fCameraRadiusMin, 0.f, 1000.f, 0.5f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Front Distance", &m_fFrontDistFromLink, 0.f, 1000.f, 0.5f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Lock On Time", &m_fLockOnMaxTime, 0.1f, 10.f, 0.01f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "General Time", &m_fGeneralMaxTime, 0.1f, 10.f, 0.01f);
}

CLinkCamScript::CLinkCamScript(const CLinkCamScript& _origin)
	: CCamScript(_origin)
	, m_fCameraRadiusMax(_origin.m_fCameraRadiusMax)
	, m_fFrontDistFromLink(_origin.m_fFrontDistFromLink)
	, m_eMode(LINK_CAM_MODE::GENERAL)
	, m_pLockOnObj(nullptr)
	, m_bLockOn(false)
	, m_bLockOnStart(false)
	, m_fLockOnMaxTime(_origin.m_fLockOnMaxTime)
	, m_fLockOnAccTime(0.f)
	, m_bOnce(true)
	, m_fGeneralAccTime(0.f)
{
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Distance", &m_fCameraRadiusMax, 0.f, 200.f, 0.5f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Distance Min", &m_fCameraRadiusMin, 0.f, 1000.f, 0.5f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Front Distance", &m_fFrontDistFromLink, 0.f, 200.f, 0.5f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Lock On Time", &m_fLockOnMaxTime, 0.1f, 10.f, 0.01f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "General Time", &m_fGeneralMaxTime, 0.1f, 10.f, 0.01f);
}

CLinkCamScript::~CLinkCamScript()
{
}

void CLinkCamScript::RBTNMove()
{
	if (KEY_PRESSED(KEY::RBTN))
	{
		Vec2 vMouseDir = CKeyMgr::GetInst()->GetMouseDir();
		Vec3 vRot = Transform()->GetRelativeRotation();

		vRot.y += vMouseDir.x * FDT * XM_PIDIV2;
		vRot.x -= vMouseDir.y * FDT * XM_PIDIV2;

		if (vRot.x > XM_PI / 9.f * 3.f)
			vRot.x = XM_PI / 9.f * 3.f;
		else if (vRot.x < 0.f)
			vRot.x = 0.f;

		Transform()->SetRelativeRotation(vRot);
	}

	Transform()->finaltick(); // should recalculate dir
}

void CLinkCamScript::init()
{
	m_eCameraType = CAMERA_SELECTION::LINK;
	CCamScript::RegisterCamera();
	m_pLinkScr = FindObjectByName(LINK_STRING_WCHAR[LINK_STRING_LINK])->GetScript<CLinkAnimScript>();

	// start : yz 30 degree
	Transform()->SetRelativeRotation(Vec3(XM_PI / 6.f, 0.f, 0.f));
}

void CLinkCamScript::begin()
{

}

void CLinkCamScript::tick()
{
	if (CalBit(m_pLinkScr->GetLinkMode(), LINK_MODE::LINK_MODE_LOCKON, BIT_LEAST_ONE))
	{
		if (m_eMode != LINK_CAM_MODE::LOCKON_START && m_eMode != LINK_CAM_MODE::LOCKON)
		{
			m_eMode = LINK_CAM_MODE::LOCKON_START;
			m_pLockOnObj = m_pLinkScr->GetLockOnRadar()->GetLockOnTarget();
		}
	}
	else
	{
		if (m_eMode != LINK_CAM_MODE::GENERAL && m_eMode != LINK_CAM_MODE::GENERAL_START)
		{
			m_eMode = LINK_CAM_MODE::GENERAL_START;
		}
	}

	switch (m_eMode)
	{
	case LINK_CAM_MODE::GENERAL:
	{
		GeneralMove();
	}
	break;
	case LINK_CAM_MODE::GENERAL_START:
	{
		GeneralStartMove();
	}
	break;
	case LINK_CAM_MODE::LOCKON:
	{
		LockOnMove();
	}
	break;
	case LINK_CAM_MODE::LOCKON_START:
	{
		LockOnStartMove();
	}
	break;
	}
}

void CLinkCamScript::GeneralMove()
{
	RBTNMove();

	Vec3 vDirToCamFromObj = -Transform()->GetRelativeDir(DIR::FRONT);
	Vec3 vDirToObjFromCamXZ = Vec3(-vDirToCamFromObj.x, 0.f, -vDirToCamFromObj.z).Normalize();

	m_fCameraRadiusCur = m_fCameraRadiusMax;
	Vec3 vCamPos = m_pLinkScr->GetOwner()->Transform()->GetRelativePos() + vDirToObjFromCamXZ * m_fFrontDistFromLink + vDirToCamFromObj * m_fCameraRadiusCur;

	Transform()->SetRelativePos(vCamPos);
	m_bOnce = true;
}

void CLinkCamScript::GeneralStartMove()
{
	if (m_bOnce)
	{
		m_fLerpStartRadius = m_fCameraRadiusCur;

		m_fGeneralAccTime = 0.f;
		m_bOnce = false;
	}

	RBTNMove();

	if (m_fGeneralMaxTime > m_fGeneralAccTime)
	{
		m_fGeneralAccTime += FDT;
	}
	else
	{
		m_fGeneralAccTime = m_fGeneralMaxTime;
	}

	Vec3 vDirToCamFromObj = -Transform()->GetRelativeDir(DIR::FRONT);
	Vec3 vDirToObjFromCamXZ = Vec3(-vDirToCamFromObj.x, 0.f, -vDirToCamFromObj.z).Normalize();

	float fRatio = m_fGeneralAccTime / m_fGeneralMaxTime;
	m_fCameraRadiusCur = lerp<float>(m_fLerpStartRadius, m_fCameraRadiusMax, fRatio);

	Vec3 vCamPos = m_pLinkScr->GetOwner()->Transform()->GetRelativePos() + vDirToObjFromCamXZ * m_fFrontDistFromLink + vDirToCamFromObj * m_fCameraRadiusCur;
	Transform()->SetRelativePos(vCamPos);

	if (m_fGeneralAccTime == m_fGeneralMaxTime)
	{
		m_bOnce = true;
		m_eMode = LINK_CAM_MODE::GENERAL;
	}
}

void CLinkCamScript::LockOnMove()
{
	if (m_pLockOnObj == nullptr)
	{
		m_bOnce = true;
		m_eMode = LINK_CAM_MODE::GENERAL_START;
		return;
	}

	RBTNMove();

	Vec3 vDirToCamFromObj = -Transform()->GetRelativeDir(DIR::FRONT);
	Vec3 vDirToObjFromCamXZ = Vec3(-vDirToCamFromObj.x, 0.f, -vDirToCamFromObj.z).Normalize();

	float fDist = (m_pLinkScr->GetOwner()->Transform()->GetRelativePos() - m_pLockOnObj->Transform()->GetRelativePos()).Length();
	m_fCameraRadiusCur = lerp<float>(m_fCameraRadiusMin, m_fCameraRadiusMax, fDist / m_pLinkScr->GetLockOnRadar()->GetColRadius());

	Vec3 vFinalPos = m_pLinkScr->GetOwner()->Transform()->GetRelativePos() + vDirToObjFromCamXZ * m_fFrontDistFromLink + vDirToCamFromObj * m_fCameraRadiusCur;
	Transform()->SetRelativePos(vFinalPos);
	m_bOnce = true;
}

void CLinkCamScript::LockOnStartMove()
{
	if (m_pLockOnObj == nullptr)
	{
		m_fLockOnAccTime = 0.f;
		m_bOnce = true;

		m_eMode = LINK_CAM_MODE::GENERAL_START;
		return;
	}

	if (m_bOnce)
	{
		m_vLerpStartPos = Transform()->GetRelativePos();
		m_vLerpStartRot = Transform()->GetRelativeRotation();
		m_fLerpStartRadius = m_fCameraRadiusCur;
		m_fLockOnAccTime = 0.f;
		m_bOnce = false;
	}
	
	if (m_fLockOnMaxTime > m_fLockOnAccTime)
	{
		m_fLockOnAccTime += FDT;
	}
	else
	{
		m_fLockOnAccTime = m_fLockOnMaxTime;
	}

	float fRatio = m_fLockOnAccTime / m_fLockOnMaxTime;

	// rotate
	Vec3 vDirToCamFromObj = (m_pLinkScr->GetOwner()->Transform()->GetRelativePos() - m_pLockOnObj->Transform()->GetRelativePos()).Normalize();
	Vec3 vDirToObjFromCamXZ = Vec3(-vDirToCamFromObj.x, 0.f, -vDirToCamFromObj.z).Normalize();
	vDirToCamFromObj.y = 1.f; // 45 degree
	vDirToCamFromObj.Normalize();

	// zoom in 
	float fDist = (m_pLinkScr->GetOwner()->Transform()->GetRelativePos() - m_pLockOnObj->Transform()->GetRelativePos()).Length();
	float fRadius = lerp<float>(m_fCameraRadiusMin, m_fCameraRadiusMax, fDist / m_pLinkScr->GetLockOnRadar()->GetColRadius());

	m_fCameraRadiusCur = lerp<float>(m_fLerpStartRadius, fRadius, fRatio);
	Vec3 vFinalPos = m_pLinkScr->GetOwner()->Transform()->GetRelativePos() + vDirToObjFromCamXZ * m_fFrontDistFromLink + vDirToCamFromObj * m_fCameraRadiusCur;
	
	Vec3 vSetPos = Vec3::Lerp(m_vLerpStartPos, vFinalPos, fRatio);
	Vec3 vFinalRot = GetEulerAngleFromDirection(-vDirToCamFromObj);

	Vec3 vSetRot = Vec3::Lerp(m_vLerpStartRot, vFinalRot, fRatio);
	
	Transform()->SetRelativeRotation(vSetRot);
	Transform()->SetRelativePos(vSetPos);

	if (m_fLockOnAccTime == m_fLockOnMaxTime)
	{
		m_bOnce = true;
		m_eMode = LINK_CAM_MODE::LOCKON;
	}
}

void CLinkCamScript::BowChargeMove()
{

}

void CLinkCamScript::BowChargeStartMove()
{
	if (m_bOnce)
	{
		m_vLerpStartPos = Transform()->GetRelativePos();
		m_vLerpStartRot = Transform()->GetRelativeRotation();
		m_fLerpStartRadius = m_fCameraRadiusCur;
		m_fBowAccTime = 0.f;
		m_bOnce = false;
	}

	if (m_fBowMaxTime > m_fBowAccTime)
	{
		m_fBowAccTime += FDT;
	}
	else
	{
		m_fBowAccTime = m_fBowMaxTime;
	}

	float fRatio = m_fBowAccTime / m_fBowMaxTime;



	if (m_fBowAccTime == m_fBowMaxTime)
	{
		m_bOnce = true;
		m_eMode = LINK_CAM_MODE::BOW_CHARGE;
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
	_emitter << YAML::Value << m_fCameraRadiusMax;

	_emitter << YAML::Key << "DistFromLinkMin";
	_emitter << YAML::Value << m_fCameraRadiusMin;

	_emitter << YAML::Key << "FrontDistFromLink";
	_emitter << YAML::Value << m_fFrontDistFromLink;


	_emitter << YAML::Key << "MaxDistFromLockOnObj";
	_emitter << YAML::Value << m_fMaxDistLockOn;

	_emitter << YAML::Key << "LockOnMaxTime";
	_emitter << YAML::Value << m_fLockOnMaxTime;

	_emitter << YAML::Key << "GeneralMaxTime";
	_emitter << YAML::Value << m_fGeneralMaxTime;
}

void CLinkCamScript::LoadFromYAML(YAML::Node& _node)
{
	CCamScript::LoadFromYAML(_node);

	SAFE_LOAD_FROM_YAML(float, m_fCameraRadiusMax, _node["DistFromLink"]);
	SAFE_LOAD_FROM_YAML(float, m_fCameraRadiusMin, _node["DistFromLinkMin"]);
	SAFE_LOAD_FROM_YAML(float, m_fFrontDistFromLink, _node["FrontDistFromLink"]);
	SAFE_LOAD_FROM_YAML(float, m_fMaxDistLockOn, _node["MaxDistFromLockOnObj"]);
	SAFE_LOAD_FROM_YAML(float, m_fLockOnMaxTime, _node["LockOnMaxTime"]);
	SAFE_LOAD_FROM_YAML(float, m_fGeneralMaxTime, _node["GeneralMaxTime"]);
}


