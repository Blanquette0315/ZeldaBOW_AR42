#include "pch.h"
#include "CLinkArrowScript.h"


CLinkArrowScript::CLinkArrowScript()
	: CScript(SCRIPT_TYPE::LINKARROWSCRIPT)
	, m_fGravity(9.8f)
	, m_fMaxGravity(20.f)
	, m_vDir(Vec3::Zero)
{
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Gravity", &m_fGravity, 0.f, 100.f, 0.1f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "MaxGravity", &m_fMaxGravity, 0.f, 100.f, 0.1f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Speed", &m_vVelocity, 0.f, 1000.f, 1.f);
}

CLinkArrowScript::CLinkArrowScript(const CLinkArrowScript& _origin)
	: CScript(_origin)
	, m_fGravity(_origin.m_fGravity)
	, m_fMaxGravity(20.f)
	, m_vVelocity(_origin.m_vVelocity)
	, m_vDir(Vec3::Zero)
{
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Gravity", &m_fGravity, 0.f, 100.f, 0.1f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Speed", &m_vVelocity, 0.f, 1000.f, 1.f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "MaxGravity", &m_fMaxGravity, 0.f, 100.f, 0.1f);
}



CLinkArrowScript::~CLinkArrowScript()

{
}

void CLinkArrowScript::begin()
{

}

void CLinkArrowScript::tick()
{
	if (GetOwner()->GetParent() == nullptr)
	{

		Vec3 vPos = Transform()->GetRelativePos();
		Vec3 vPosSave = vPos;
		vPos += m_vDir * m_vVelocity * FDT;

		m_fCurGravity += m_fGravity * FDT;
		if (m_fCurGravity > m_fMaxGravity)
		{
			m_fCurGravity = m_fMaxGravity;
		}

		vPos.y -= m_fCurGravity * FDT;
		Transform()->SetRelativePos(vPos);
		Vec3 vRot = GetEulerAngleFromDirection((vPos - vPosSave).Normalize());
		vRot.x += XM_PIDIV2;
		
		Transform()->SetRelativeRotation(vRot);
	}
}

void CLinkArrowScript::BeginOverlap(CGameObject* _pOther)
{
}

void CLinkArrowScript::Overlap(CGameObject* _pOther)
{
}

void CLinkArrowScript::EndOverlap(CGameObject* _pOther)
{
}

void CLinkArrowScript::SaveToYAML(YAML::Emitter& _emitter)
{
	CScript::SaveToYAML(_emitter);
	_emitter << YAML::Key << "Gravity";
	_emitter << YAML::Value << m_fGravity;

	_emitter << YAML::Key << "Speed";
	_emitter << YAML::Value << m_vVelocity;

	_emitter << YAML::Key << "MaxGravity";
	_emitter << YAML::Value << m_fMaxGravity;
}

void CLinkArrowScript::LoadFromYAML(YAML::Node& _node)
{
	CScript::LoadFromYAML(_node);

	SAFE_LOAD_FROM_YAML(float, m_fGravity, _node["Gravity"]);
	SAFE_LOAD_FROM_YAML(float, m_vVelocity, _node["Speed"]);
	SAFE_LOAD_FROM_YAML(float, m_fMaxGravity, _node["MaxGravity"]);
}


