#include "pch.h"
#include "CPlayerScript.h"

#include "CMissileScript.h"
#include <Engine\CMaterial.h>

CPlayerScript::CPlayerScript()
	: CScript(PLAYERSCRIPT)
	, m_fAccTime(0.f)
	, m_fSpeed(100.f)
	, m_iTest(0)
{
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Player MoveSpeed", &m_fSpeed);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Player JumpHeight", &m_fJumpHeight);

	AddScriptParam(SCRIPT_PARAM::INT, "Int Test         ", &m_iTest);
	AddScriptParam(SCRIPT_PARAM::VEC2, "Vec2 Test       ", &m_v2Test);
	AddScriptParam(SCRIPT_PARAM::VEC3, "Vec3 Test       ", &m_v3Test);
	AddScriptParam(SCRIPT_PARAM::VEC4, "Vec4 Test       ", &m_v4Test);

	//m_Prefab = CResMgr::GetInst()->FindRes<CPrefab>(L"prefab\\Test_Prefab_0.pref");
	//AddScriptParam(SCRIPT_PARAM::PREFAB, "Prefab  Test    ", &m_Prefab);

	//m_Prefab1 = CResMgr::GetInst()->FindRes<CPrefab>(L"prefab\\Test_Prefab_0.pref");
	//AddScriptParam(SCRIPT_PARAM::PREFAB, "Prefab  Test    ", &m_Prefab1);

	//m_PTestTex = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\link.png");
	//AddScriptParam(SCRIPT_PARAM::TEXTURE, "Texture Test    ", &m_PTestTex);

	//m_PTestTex1 = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\link.png");
	//AddScriptParam(SCRIPT_PARAM::TEXTURE, "Texture Test    ", &m_PTestTex1);
}

CPlayerScript::~CPlayerScript()
{
}

void CPlayerScript::begin()
{
	int a = 0;
}

void CPlayerScript::tick()
{
	Vec3 vPos = Transform()->GetRelativePos();

	if (KEY_PRESSED(KEY::LEFT))
	{
		vPos.x -= m_fSpeed * FDT;
	}

	if (KEY_PRESSED(KEY::RIGHT))
	{
		vPos.x += m_fSpeed * FDT;
	}

	if (KEY_PRESSED(KEY::UP))
	{
		vPos.y += m_fSpeed * FDT;
	}

	if (KEY_PRESSED(KEY::DOWN))
	{
		vPos.y -= m_fSpeed * FDT;
	}

	if (KEY_PRESSED(KEY::Y))
	{
		Vec3 vRot = Transform()->GetRelativeRotation();
		vRot.z += FDT * XM_PI;
		Transform()->SetRelativeRotation(vRot);

		float fZRot = Collider2D()->GetRotationZ();
		fZRot += FDT * XM_PI;
		Collider2D()->SetRotationZ(fZRot);
	}

	if (vPos != Transform()->GetRelativePos())
		Transform()->SetRelativePos(vPos);

	if (KEY_TAP(KEY::SPACE))
	{
	}
}

void CPlayerScript::BeginOverlap(CCollider2D* _pOther)
{
	_pOther->Destroy();
}

void CPlayerScript::Overlap(CCollider2D* _pOther)
{
}

void CPlayerScript::EndOverlap(CCollider2D* _pOther)
{
}

void CPlayerScript::SaveToYAML(YAML::Emitter& _emitter)
{
	CScript::SaveToYAML(_emitter);
	_emitter << YAML::Key << "Speed";
	_emitter << YAML::Value << m_fSpeed;
	_emitter << YAML::Key << "JumpHeight";
	_emitter << YAML::Value << m_fJumpHeight;
}

void CPlayerScript::LoadFromYAML(YAML::Node& _node)
{
	/*CScript::LoadFromYAML(_node);
	m_fSpeed = _node["Speed"].as<float>();
	m_fJumpHeight = _node["JumpHeight"].as<float>();*/

	CScript::LoadFromYAML(_node);
	SAFE_LOAD_FROM_YAML(float, m_fSpeed, _node["Speed"]);
	SAFE_LOAD_FROM_YAML(float, m_fJumpHeight, _node["JumpHeight"]);
}
