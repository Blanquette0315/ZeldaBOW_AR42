#include "pch.h"
#include "CLinkCamScript.h"
 
#include <Engine/CKeyMgr.h>

#include <Engine/CTransform.h>

#include "CLockOnScript.h"
#include "CLinkAnimScript.h"
#include "CLinkBowScript.h"
#include "FSMNode.h"

CLinkCamScript::CLinkCamScript()
	: CCamScript(SCRIPT_TYPE::LINKCAMSCRIPT)
	, m_fCameraRadiusMax(200.f)
	, m_fFrontDistFromLink(100.f)
	, m_eMode(LINK_CAM_MODE::GENERAL)
	, m_pLockOnObj(nullptr)
	, m_bLockOn(false)
	, m_bLockOnStart(false)
	, m_fLockOnMaxTime(1.f)
	, m_fMaxDistLockOn(100.f)
	, m_fLockOnAccTime(0.f)
	, m_fGeneralAccTime(0.f)
	, m_fBowAccTime(0.f)
	, m_bOnce(true)
	, m_vAddRot()
{
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Distance", &m_fCameraRadiusMax, 0.f, 1000.f, 0.5f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Distance Min", &m_fCameraRadiusMin, 0.f, 1000.f, 0.5f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Distance Charge Min", &m_fCameraRadiusChargeMin, 0.f, 1000.f, 0.5f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Front Distance", &m_fFrontDistFromLink, 0.f, 1000.f, 0.5f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Lock On Time", &m_fLockOnMaxTime, 0.1f, 10.f, 0.01f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "General Time", &m_fGeneralMaxTime, 0.1f, 10.f, 0.01f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Bow Time", &m_fBowMaxTime, 0.1f, 10.f, 0.01f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "PosUp", &m_fPosUp, 0.1f, 10.f, 0.01f);
}

CLinkCamScript::CLinkCamScript(const CLinkCamScript& _origin)
	: CCamScript(_origin)
	, m_fCameraRadiusMax(_origin.m_fCameraRadiusMax)
	, m_fCameraRadiusMin(_origin.m_fCameraRadiusMin)
	, m_fFrontDistFromLink(_origin.m_fFrontDistFromLink)
	, m_eMode(LINK_CAM_MODE::GENERAL)
	, m_pLockOnObj(nullptr)
	, m_bLockOn(false)
	, m_bLockOnStart(false)
	, m_fLockOnMaxTime(_origin.m_fLockOnMaxTime)
	, m_fGeneralMaxTime(_origin.m_fGeneralMaxTime)
	, m_fBowMaxTime(_origin.m_fBowMaxTime)
	, m_fLockOnAccTime(0.f)
	, m_fGeneralAccTime(0.f)
	, m_fBowAccTime(0.f)
	, m_bOnce(true)
	, m_fPosUp(_origin.m_fPosUp)
	, m_vAddRot()
	
{
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Distance", &m_fCameraRadiusMax, 0.f, 200.f, 0.5f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Distance Min", &m_fCameraRadiusMin, 0.f, 1000.f, 0.5f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Distance Charge Min", &m_fCameraRadiusChargeMin, 0.f, 1000.f, 0.5f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Front Distance", &m_fFrontDistFromLink, 0.f, 200.f, 0.5f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Lock On Time", &m_fLockOnMaxTime, 0.1f, 10.f, 0.01f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "General Time", &m_fGeneralMaxTime, 0.1f, 10.f, 0.01f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Bow Time", &m_fBowMaxTime, 0.1f, 10.f, 0.01f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "PosUp", &m_fPosUp, 0.1f, 10.f, 0.01f);
}

CLinkCamScript::~CLinkCamScript()
{
}

void CLinkCamScript::DetermineRotDir(const Vec3& _vStart, const Vec3& _vEnd, Vec3& _vOutStart, Vec3& _vOutEnd)
{
	_vOutStart = _vStart;
	_vOutEnd = _vEnd;

	Vec3 vDeter = _vStart - _vEnd;

	if (fabsf(vDeter.x) > XM_PI)
	{
		if (_vOutStart.x > _vOutEnd.x)
			_vOutStart.x -= XM_2PI;
		else
			_vOutEnd.x -= XM_2PI;
	}

	if (fabsf(vDeter.y) > XM_PI)
	{
		if (_vOutStart.y > _vOutEnd.y)
			_vOutStart.y -= XM_2PI;
		else
			_vOutEnd.y -= XM_2PI;
	}

	if (fabsf(vDeter.z) > XM_PI)
	{
		if (_vOutStart.z > _vOutEnd.z)
			_vOutStart.z -= XM_2PI;
		else
			_vOutEnd.z -= XM_2PI;
	}
}

void CLinkCamScript::RBTNMove(bool _bRestrictX, float _fMaxX, float _fMinX, bool _bRestrictY, float _fMaxY, float _fMinY)
{
	if (KEY_PRESSED(KEY::RBTN))
	{
		Vec2 vMouseDir = CKeyMgr::GetInst()->GetMouseDir();
		Vec3 vRot = Transform()->GetRelativeRotation();

		vRot.y += vMouseDir.x * FDT * XM_PIDIV2;
		vRot.x -= vMouseDir.y * FDT * XM_PIDIV2;
		
		if (_bRestrictX)
		{
			if (vRot.x > _fMaxX)
				vRot.x = _fMaxX;
			else if (vRot.x < _fMinX)
				vRot.x = _fMinX;
		}

		if (_bRestrictY)
		{
			if (vRot.y > _fMaxY)
				vRot.y = _fMaxY;
			else if (vRot.y < _fMinY)
				vRot.y = _fMinY;
		}

		Transform()->SetRelativeRotation(vRot);
	}

	Transform()->finaltick(); // should recalculate dir
}

void CLinkCamScript::LBTNMove(bool _bRestrictX, float _fMaxX, float _fMinX, bool _bRestrictY, float _fMaxY, float _fMinY)
{
	if (KEY_PRESSED(KEY::LBTN))
	{
		Vec2 vMouseDir = CKeyMgr::GetInst()->GetMouseDir();
		Vec3 vRot;

		vRot.y = vMouseDir.x * FDT * XM_PIDIV2 / 4.f;
		vRot.x = -vMouseDir.y * FDT * XM_PIDIV2 / 4.f;

		m_vAddRot += vRot;

		if (_bRestrictX)
		{
			if (m_vAddRot.x > _fMaxX)
				m_vAddRot.x = _fMaxX;
			else if (m_vAddRot.x < _fMinX)
				m_vAddRot.x = _fMinX;
		}

		if (_bRestrictY)
		{
			if (m_vAddRot.y > _fMaxY)
				m_vAddRot.y = _fMaxY;
			else if (m_vAddRot.y < _fMinY)
				m_vAddRot.y = _fMinY;
		}
	}
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
	m_ePrevMode = LINK_CAM_MODE::NONE;
}

void CLinkCamScript::tick()
{
	if (CalBit(m_pLinkScr->GetCurAnimNode()->iPreferences, LAP_BOW_CHARGE, BIT_LEAST_ONE))
	{
		if (m_eMode != LINK_CAM_MODE::BOW_CHARGE_START && m_eMode != LINK_CAM_MODE::BOW_CHARGE)
		{
			m_eMode = LINK_CAM_MODE::BOW_CHARGE_START;
		}
	}
	else if (CalBit(m_pLinkScr->GetLinkMode(), LINK_MODE::LINK_MODE_LOCKON, BIT_LEAST_ONE))
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

	if (m_eMode != m_ePrevMode)
	{
		m_bOnce = true;
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
	case LINK_CAM_MODE::BOW_CHARGE:
	{
		BowChargeMove();
	}
	break;
	case LINK_CAM_MODE::BOW_CHARGE_START:
	{
		BowChargeStartMove();
	}
	break;
	}

	m_ePrevMode = m_eMode;
}

void CLinkCamScript::GeneralMove()
{
	RBTNMove();

	Vec3 vDirToCamFromObj = -Transform()->GetRelativeDir(DIR::FRONT);
	Vec3 vDirToObjFromCamXZ = Vec3(-vDirToCamFromObj.x, 0.f, -vDirToCamFromObj.z).Normalize();

	m_fCameraRadiusCur = m_fCameraRadiusMax;
	Vec3 vCamPos = m_pLinkScr->GetOwner()->Transform()->GetRelativePos() + vDirToObjFromCamXZ * m_fFrontDistFromLink + vDirToCamFromObj * m_fCameraRadiusCur;
	vCamPos.y += m_fPosUp;
	Transform()->SetRelativePos(vCamPos);
	m_bOnce = true;
}

void CLinkCamScript::GeneralStartMove()
{
	if (m_bOnce)
	{
		m_vLerpStartPos = Transform()->GetRelativePos();
		m_fLerpStartRadius = m_fCameraRadiusCur;
		m_vLerpStartRot = Transform()->GetRelativeRotation();
		m_vLerpEndRot = m_vLerpStartRot - m_vAddRot;
		m_vAddRot = Vec3(0.f, 0.f, 0.f);

		m_fGeneralAccTime = 0.f;
		m_bOnce = false;
	}

	// RBTNMove();

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
	vCamPos.y += m_fPosUp;
	vCamPos = Vec3::Lerp(m_vLerpStartPos, vCamPos, fRatio);

	Transform()->SetRelativePos(vCamPos);
	Vec3 vLerpStartRot;
	Vec3 vLerpEndRot;

	DetermineRotDir(m_vLerpStartRot, m_vLerpEndRot, vLerpStartRot, vLerpEndRot);

	Vec3 vCamRot = Vec3::Lerp(vLerpStartRot, vLerpEndRot, fRatio);
	Transform()->SetRelativeRotation(vCamRot);

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
	m_fCameraRadiusCur = lerp<float>(m_fCameraRadiusMin, m_fCameraRadiusMax, LerpCos(fDist / m_pLinkScr->GetLockOnRadar()->GetColRadius()));

	Vec3 vFinalPos = m_pLinkScr->GetOwner()->Transform()->GetRelativePos() + vDirToObjFromCamXZ * m_fFrontDistFromLink + vDirToCamFromObj * m_fCameraRadiusCur;
	vFinalPos.y += m_fPosUp;
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
	vDirToCamFromObj.y = tanf(XM_PI / 12.f); // 15 degree
	vDirToCamFromObj.Normalize();

	// zoom in 
	float fDist = (m_pLinkScr->GetOwner()->Transform()->GetRelativePos() - m_pLockOnObj->Transform()->GetRelativePos()).Length();
	float fRadius = lerp<float>(m_fCameraRadiusMin, m_fCameraRadiusMax, LerpCos(fDist / m_pLinkScr->GetLockOnRadar()->GetColRadius()));

	m_fCameraRadiusCur = lerp<float>(m_fLerpStartRadius, fRadius, fRatio);
	Vec3 vFinalPos = m_pLinkScr->GetOwner()->Transform()->GetRelativePos() + vDirToObjFromCamXZ * m_fFrontDistFromLink + vDirToCamFromObj * m_fCameraRadiusCur;
	vFinalPos.y += m_fPosUp;


	Vec3 vSetPos = Vec3::Lerp(m_vLerpStartPos, vFinalPos, fRatio);
	Vec3 vFinalRot = GetEulerAngleFromDirection(-vDirToCamFromObj);

	Vec3 vLerpStartRot;
	Vec3 vLerpEndRot;

	DetermineRotDir(m_vLerpStartRot, vFinalRot, vLerpStartRot, vLerpEndRot);

	Vec3 vSetRot = Vec3::Lerp(vLerpStartRot, vLerpEndRot, fRatio);

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
	CGameObject* pArrowObj = m_pLinkScr->GetLinkBow()->GetScript<CLinkBowScript>()->GetArrow();

	if (pArrowObj == nullptr || pArrowObj->GetParent() == nullptr)
		return;

	Vec3 vArrowPos = pArrowObj->Transform()->GetWorldPos();
	Vec3 vDir = m_pLinkScr->Transform()->GetRelativeDir(DIR::FRONT);
	Vec3 vRot = GetEulerAngleFromDirection(vDir);
	Transform()->SetRelativeRotation(vRot);
	LBTNMove(true, XM_PI / 12.f, -XM_PI / 12.f, true, XM_PI / 12.f, -XM_PI / 12.f);

	Transform()->AddRelativeRotation(m_vAddRot);

	Vec3 vDirToCamFromObj = -Transform()->GetRelativeDir(DIR::FRONT);
	Vec3 vDirToObjFromCamXZ = Vec3(-vDirToCamFromObj.x, 0.f, -vDirToCamFromObj.z).Normalize();

	Vec3 vFinalPos = vArrowPos + vDirToObjFromCamXZ * m_fFrontDistFromLink + vDirToCamFromObj * m_fCameraRadiusCur;
	vFinalPos.y += m_fPosUp;

	Transform()->SetRelativePos(vFinalPos);
}

void CLinkCamScript::BowChargeStartMove()
{
	CGameObject* pArrowObj = m_pLinkScr->GetLinkBow()->GetScript<CLinkBowScript>()->GetArrow();

	if (pArrowObj == nullptr || pArrowObj->GetParent() == nullptr)
		return;

	if (m_bOnce)
	{
		m_vLerpStartPos = Transform()->GetRelativePos();
		m_vLerpStartRot = Transform()->GetRelativeRotation();
		m_fLerpStartRadius = m_fCameraRadiusCur;
		m_vAddRot = Vec3::Zero;
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

	Vec3 vDir = m_pLinkScr->Transform()->GetRelativeDir(DIR::FRONT);
	Vec3 vRot = GetEulerAngleFromDirection(vDir);

	Vec3 vDirToObjFromCamXZ = Vec3(vDir.x, 0.f, vDir.z).Normalize();
	vDir *= -1.f;

	m_fCameraRadiusCur = lerp<float>(m_fLerpStartRadius, m_fCameraRadiusChargeMin, fRatio);
	Vec3 vFinalPos = pArrowObj->Transform()->GetWorldPos() + vDirToObjFromCamXZ * m_fFrontDistFromLink + vDir * m_fCameraRadiusCur;
	vFinalPos.y += m_fPosUp;
	vFinalPos = Vec3::Lerp(m_vLerpStartPos, vFinalPos, fRatio);


	Vec3 vLerpStartRot;
	Vec3 vLerpEndRot;

	DetermineRotDir(m_vLerpStartRot, vRot, vLerpStartRot, vLerpEndRot);

	Vec3 vFinalRot = Vec3::Lerp(vLerpStartRot, vLerpEndRot, fRatio);

	Transform()->SetRelativePos(vFinalPos);


	Transform()->SetRelativeRotation(vFinalRot);

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

	_emitter << YAML::Key << "BowMaxTime";
	_emitter << YAML::Value << m_fBowMaxTime;

	_emitter << YAML::Key << "PosUp";
	_emitter << YAML::Value << m_fPosUp;

	_emitter << YAML::Key << "DistFromLinkChargeMin";
	_emitter << YAML::Value << m_fCameraRadiusChargeMin;
}

void CLinkCamScript::LoadFromYAML(YAML::Node& _node)
{
	CCamScript::LoadFromYAML(_node);

	SAFE_LOAD_FROM_YAML(float, m_fCameraRadiusMax, _node["DistFromLink"]);
	SAFE_LOAD_FROM_YAML(float, m_fCameraRadiusMin, _node["DistFromLinkMin"]);
	SAFE_LOAD_FROM_YAML(float, m_fCameraRadiusChargeMin, _node["DistFromLinkChargeMin"]);
	SAFE_LOAD_FROM_YAML(float, m_fFrontDistFromLink, _node["FrontDistFromLink"]);
	SAFE_LOAD_FROM_YAML(float, m_fMaxDistLockOn, _node["MaxDistFromLockOnObj"]);
	SAFE_LOAD_FROM_YAML(float, m_fLockOnMaxTime, _node["LockOnMaxTime"]);
	SAFE_LOAD_FROM_YAML(float, m_fGeneralMaxTime, _node["GeneralMaxTime"]);
	SAFE_LOAD_FROM_YAML(float, m_fBowMaxTime, _node["BowMaxTime"]);
	SAFE_LOAD_FROM_YAML(float, m_fPosUp, _node["PosUp"]);
}


