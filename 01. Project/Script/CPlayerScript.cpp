#include "pch.h"
#include "CPlayerScript.h"

#include "CMissileScript.h"
#include <Engine\CMaterial.h>
#include <Engine/CRigidbody.h>
#include "CRayCastScript.h"

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

	//m_Prefab = CResMgr::GetInst()->FindRes<CPrefab>(L"prefab\\TestArrow.pref");
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
	//delete m_pWallRay;
}

void CPlayerScript::begin()
{
	// GroundRay Setting
	m_pRayScript = (CRayCastScript*)GetOwner()->GetScript(RAYCASTSCRIPT);
	m_pRayScript->CreateRay(L"GroundRay");
	Vec3 vStartPos = Transform()->GetWorldPos();
	m_pRayScript->SetRayStartPoint(L"GroundRay", vStartPos);
	m_pRayScript->SetRayDir(L"GroundRay", Vec3(0.f, -1.f, 0.f).Normalize());
	m_pRayScript->SetMaxDistance(L"GroundRay", 50.f / 100.f);
	m_pRayScript->SetRayFilter(L"GroundRay", FILTER_GROUP::eGround);
}

void CPlayerScript::tick()
{
	m_pRayScript->SetRayStartPoint(L"GroundRay", Transform()->GetWorldPos());
	m_pRayScript->RayCast(L"GroundRay");
	PhysRayCast* pRay = m_pRayScript->FindRay(L"GroundRay");

	if (KEY_RELEASE(KEY::LEFT))
	{
		m_bKeyRelease = true;
	}

	if (KEY_RELEASE(KEY::RIGHT))
	{
		m_bKeyRelease = true;
	}

	if (KEY_RELEASE(KEY::UP))
	{
		m_bKeyRelease = true;
	}

	if (KEY_RELEASE(KEY::DOWN))
	{
		m_bKeyRelease = true;
	}

	if (m_bGround)
	{
		if (m_bKeyRelease)
		{
			RigidBody()->SetKeyRelease(true);
			m_bKeyRelease = false;
		}

		if (!m_bJump)
		{
			if (KEY_PRESSED(KEY::LEFT))
			{
				Vec3 HitNormal = m_pRayScript->FindRay(L"GroundRay")->Hit.HitNormal;
				Vec3 vMove = Vec3(-1.f, 0.f, 0.f).Normalize();

				Vec3 vRealMove = vMove + (HitNormal * ((-vMove).Dot(HitNormal)));
				vRealMove *= 2.f;

				RigidBody()->AddVelocity(vRealMove);
			}

			if (KEY_PRESSED(KEY::RIGHT))
			{
				Vec3 HitNormal = m_pRayScript->FindRay(L"GroundRay")->Hit.HitNormal;
				Vec3 vMove = Vec3(1.f, 0.f, 0.f).Normalize();

				Vec3 vRealMove = vMove + (HitNormal * ((-vMove).Dot(HitNormal)));
				vRealMove *= 2.f;

				RigidBody()->AddVelocity(vRealMove);
			}

			if (KEY_PRESSED(KEY::UP))
			{
				Vec3 HitNormal = m_pRayScript->FindRay(L"GroundRay")->Hit.HitNormal;
				Vec3 vMove = Vec3(0.f, 0.f, 1.f).Normalize();

				Vec3 vRealMove = vMove + (HitNormal * ((-vMove).Dot(HitNormal)));
				vRealMove *= 2.f;
				RigidBody()->AddVelocity(vRealMove);
			}

			if (KEY_PRESSED(KEY::DOWN))
			{
				Vec3 HitNormal = m_pRayScript->FindRay(L"GroundRay")->Hit.HitNormal;
				Vec3 vMove = Vec3(0.f, 0.f, -1.f).Normalize();

				Vec3 vRealMove = vMove + (HitNormal * ((-vMove).Dot(HitNormal)));
				vRealMove *= 2.f;
				RigidBody()->AddVelocity(vRealMove);
			}
		}
		

		if (KEY_TAP(KEY::SPACE))
		{
			RigidBody()->AddForce(Vec3(0.f, 20000.f, 0.f));
			m_bJump = true;
		}
	}
}

void CPlayerScript::BeginOverlap(CCollider* _pOther)
{
	m_bJump = false;
	RigidBody()->SetGravityOption(false);
}

void CPlayerScript::Overlap(CCollider* _pOther)
{
	m_bGround = true;
	RigidBody()->SetGround(m_bGround);
	RigidBody()->SetGravityOption(false);
}

void CPlayerScript::EndOverlap(CCollider* _pOther)
{
	m_bGround = false;
	RigidBody()->SetGravityOption(true);
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
