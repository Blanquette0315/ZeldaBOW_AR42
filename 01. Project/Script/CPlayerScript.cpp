#include "pch.h"
#include "CPlayerScript.h"

#include "CMissileScript.h"
#include <Engine\CMaterial.h>
#include <Engine/CRigidbody.h>

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
	if (RigidBody()->RayCast())//(!RigidBody()->IsAir())
	{
		if (KEY_RELEASE(KEY::LEFT))
		{
			RigidBody()->SetKeyRelease(true);
		}

		if (KEY_RELEASE(KEY::RIGHT))
		{
			RigidBody()->SetKeyRelease(true);
		}

		if (KEY_RELEASE(KEY::UP))
		{
			RigidBody()->SetKeyRelease(true);
		}

		if (KEY_RELEASE(KEY::DOWN))
		{
			RigidBody()->SetKeyRelease(true);
		}


		if (KEY_PRESSED(KEY::LEFT))
		{
			Vec3 HitNormal = RigidBody()->GetHitNormal().Normalize();
			Vec3 vMove = Vec3(-1.f, 0.f, 0.f).Normalize();

			Vec3 vRealMove = vMove + (HitNormal * ((-vMove).Dot(HitNormal)));
			vRealMove *= 2.f;

			RigidBody()->AddVelocity(vRealMove);
		}

		if (KEY_PRESSED(KEY::RIGHT))
		{
			Vec3 HitNormal = RigidBody()->GetHitNormal().Normalize();
			Vec3 vMove = Vec3(1.f, 0.f, 0.f).Normalize();

			Vec3 vRealMove = vMove + (HitNormal * ((-vMove).Dot(HitNormal)));
			vRealMove *= 2.f;

			RigidBody()->AddVelocity(vRealMove);
		}

		if (KEY_PRESSED(KEY::UP))
		{
			Vec3 HitNormal = RigidBody()->GetHitNormal().Normalize();
			Vec3 vMove = Vec3(0.f, 0.f, 1.f).Normalize();

			Vec3 vRealMove = vMove + (HitNormal * ((-vMove).Dot(HitNormal)));
			vRealMove *= 2.f;
			RigidBody()->AddVelocity(vRealMove);
		}

		if (KEY_PRESSED(KEY::DOWN))
		{
			Vec3 HitNormal = RigidBody()->GetHitNormal().Normalize();
			Vec3 vMove = Vec3(0.f, 0.f, -1.f).Normalize();

			Vec3 vRealMove = vMove + (HitNormal * ((-vMove).Dot(HitNormal)));
			vRealMove *= 2.f;
			RigidBody()->AddVelocity(vRealMove);
		}

		if (KEY_TAP(KEY::SPACE))
		{
			//RigidBody()->SetVelocity(Vec3(0.f, 0.f, 5.f));
			RigidBody()->AddForce(Vec3(0.f, 50000.f, 0.f));
		}
	}

	
	

	//if (RigidBody()->GetPxVeloctiy().y == 0.f)
	//{
	//	if (KEY_PRESSED(KEY::LEFT))
	//	{
	//		RigidBody()->AddVelocity(Vec3(-5.f, 0.f, 0.f));
	//	}

	//	if (KEY_PRESSED(KEY::RIGHT))
	//	{
	//		RigidBody()->AddVelocity(Vec3(5.f, 0.f, 0.f));
	//	}

	//	if (KEY_PRESSED(KEY::UP))
	//	{
	//		RigidBody()->AddVelocity(Vec3(0.f, 0.f, 5.f));
	//	}

	//	if (KEY_PRESSED(KEY::DOWN))
	//	{
	//		RigidBody()->AddVelocity(Vec3(0.f, 0.f, -5.f));
	//	}
	//	

	//	if (m_fAccTime == 0.f)
	//	{
	//		if (KEY_PRESSED(KEY::SPACE))
	//		{
	//			m_fAccTime += FDT;
	//			
	//			//RigidBody()->SetVelocity(Vec3(0.f, 0.f, 5.f));
	//			RigidBody()->AddForce(Vec3(0.f, 50000.f, 0.f));
	//		}
	//	}
	//	else if (m_fAccTime != 0.f)
	//	{
	//		if (m_fAccTime > 5.f * FDT)
	//		{
	//			m_fAccTime = 0.f;
	//		}
	//		else
	//		{
	//			m_fAccTime += FDT;
	//		}
	//	}
	//}
}

void CPlayerScript::BeginOverlap(CCollider* _pOther)
{
	int a = 0;
}

void CPlayerScript::Overlap(CCollider* _pOther)
{
	int a = 0;
}

void CPlayerScript::EndOverlap(CCollider* _pOther)
{
	int a = 0;
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
