#include "pch.h"
#include "CLinkCamScript.h"
 
#include <Engine/CKeyMgr.h>

#include <Engine/CTransform.h>

CLinkCamScript::CLinkCamScript()
	: CCamScript(SCRIPT_TYPE::LINKCAMSCRIPT)
	, m_fDistFromLink(200.f)
{
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Distance", &m_fDistFromLink, 0.f, 200.f, 0.5f);
}

CLinkCamScript::CLinkCamScript(const CLinkCamScript& _origin)
	: CCamScript(_origin)
	, m_fDistFromLink(_origin.m_fDistFromLink)
{
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Distance", &m_fDistFromLink, 0.f, 200.f, 0.5f);
}

CLinkCamScript::~CLinkCamScript()
{
}

void CLinkCamScript::Move()
{
	if (KEY_PRESSED(KEY::RBTN))
	{
		Vec2 vMouseDir = CKeyMgr::GetInst()->GetMouseDir();
		Vec3 vRot = Transform()->GetRelativeRotation();

		vRot.y += vMouseDir.x * FDT * XM_PI;
		vRot.x -= vMouseDir.y * FDT * XM_PI;

		if (vRot.x > XM_PI / 2.f)
			vRot.x = XM_PI / 9.f * 4.f;
		else if (vRot.x < 0.f)
			vRot.x = 0.f;

		Transform()->SetRelativeRotation(vRot);
	}

	Transform()->finaltick(); // should recalculate world-dir

	Vec3 vDirToCamFromObj = -Transform()->GetWorldDir(DIR::FRONT);
	Vec3 vCamPos = m_pLinkObj->Transform()->GetRelativePos() + vDirToCamFromObj * m_fDistFromLink;

	Transform()->SetRelativePos(vCamPos);
}

void CLinkCamScript::init()
{
	m_eCameraType = CAMERA_SELECTION::LINK;
	CCamScript::RegisterCamera();
	m_pLinkObj = FindObjectByName(L"Link");

	// start : yz 30 degree
	Transform()->SetRelativeRotation(Vec3(XM_PI / 6.f, 0.f, 0.f));
}

void CLinkCamScript::begin()
{

}

void CLinkCamScript::tick()
{
	Move();
}

void CLinkCamScript::BeginOverlap(CCollider* _pOther)
{
}

void CLinkCamScript::Overlap(CCollider* _pOther)
{
}

void CLinkCamScript::EndOverlap(CCollider* _pOther)
{
}

void CLinkCamScript::SaveToYAML(YAML::Emitter& _emitter)
{
	CCamScript::SaveToYAML(_emitter);

	_emitter << YAML::Key << "DistFromLink";
	_emitter << YAML::Value << m_fDistFromLink;
}

void CLinkCamScript::LoadFromYAML(YAML::Node& _node)
{
	CCamScript::LoadFromYAML(_node);

	SAFE_LOAD_FROM_YAML(float, m_fDistFromLink, _node["DistFromLink"]);
}


