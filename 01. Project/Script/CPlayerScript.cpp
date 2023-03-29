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
	//, m_pWallRay(new PhysRayCast)
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
	int a = 0;

	// WallRay Setting
	Vec3 vPos = Transform()->GetRelativePos();
	vPos.z += 64.f;
	vPos /= 100.f;
	//m_pWallRay->SetStartOrigin(vPos.x, vPos.y, vPos.z);
	Vec3 vDir = Vec3(0.f, 0.f, 1.f).Normalize();
	//m_pWallRay->SetDirection(vDir.x, vDir.y, vDir.z);
	//m_pWallRay->SetMaxDistance(25.f / 100.f);
	//m_pWallRay->SetQueryFilterData0(FILTER_GROUP::eWall);
}

void CPlayerScript::tick()
{
	// WallRay Refresh
	//Vec3 vPos = Transform()->GetRelativePos();
	//vPos.z += 64.f;
	//vPos /= 100.f;
	//m_pWallRay->SetStartOrigin(vPos.x, vPos.y, vPos.z);

	//if (PhysX_RayCast(m_pWallRay))
	//{
	//	m_bWall = true;
	//	RigidBody()->SetGravityOption(false);
	//	m_bJump = false;
	//}
	//else
	//{
	//	m_bWall = false;
	//	RigidBody()->SetGravityOption(true);
	//}

	//if (KEY_RELEASE(KEY::LEFT))
	//{
	//	m_bKeyRelease = true;
	//	//RigidBody()->SetKeyRelease(true);
	//}

	//if (KEY_RELEASE(KEY::RIGHT))
	//{
	//	m_bKeyRelease = true;
	//	//RigidBody()->SetKeyRelease(true);
	//}

	//if (KEY_RELEASE(KEY::UP))
	//{
	//	m_bKeyRelease = true;
	//	//RigidBody()->SetKeyRelease(true);
	//}

	//if (KEY_RELEASE(KEY::DOWN))
	//{
	//	m_bKeyRelease = true;
	//	//RigidBody()->SetKeyRelease(true);
	//}

	//if (m_bGround && !m_bWall)//(RigidBody()->IsGround())
	//{
	//	if (m_bKeyRelease)
	//	{
	//		RigidBody()->SetKeyRelease(true);
	//		m_bKeyRelease = false;
	//	}

	//	if (!m_bJump)
	//	{
	//		if (KEY_PRESSED(KEY::LEFT))
	//		{
	//			Vec3 HitNormal = RigidBody()->GetHitNormal().Normalize();
	//			Vec3 vMove = Vec3(-1.f, 0.f, 0.f).Normalize();

	//			Vec3 vRealMove = vMove + (HitNormal * ((-vMove).Dot(HitNormal)));
	//			vRealMove *= 2.f;

	//			RigidBody()->AddVelocity(vRealMove);
	//		}

	//		if (KEY_PRESSED(KEY::RIGHT))
	//		{
	//			Vec3 HitNormal = RigidBody()->GetHitNormal().Normalize();
	//			Vec3 vMove = Vec3(1.f, 0.f, 0.f).Normalize();

	//			Vec3 vRealMove = vMove + (HitNormal * ((-vMove).Dot(HitNormal)));
	//			vRealMove *= 2.f;

	//			RigidBody()->AddVelocity(vRealMove);
	//		}

	//		if (KEY_PRESSED(KEY::UP))
	//		{
	//			Vec3 HitNormal = RigidBody()->GetHitNormal().Normalize();
	//			Vec3 vMove = Vec3(0.f, 0.f, 1.f).Normalize();

	//			Vec3 vRealMove = vMove + (HitNormal * ((-vMove).Dot(HitNormal)));
	//			vRealMove *= 2.f;
	//			RigidBody()->AddVelocity(vRealMove);
	//		}

	//		if (KEY_PRESSED(KEY::DOWN))
	//		{
	//			Vec3 HitNormal = RigidBody()->GetHitNormal().Normalize();
	//			Vec3 vMove = Vec3(0.f, 0.f, -1.f).Normalize();

	//			Vec3 vRealMove = vMove + (HitNormal * ((-vMove).Dot(HitNormal)));
	//			vRealMove *= 2.f;
	//			RigidBody()->AddVelocity(vRealMove);
	//		}
	//	}
	//	

	//	if (KEY_TAP(KEY::SPACE))
	//	{
	//		RigidBody()->AddForce(Vec3(0.f, 20000.f, 0.f));
	//		m_bJump = true;
	//	}
	//}
	//else if (m_bWall)
	//{
	//	if (m_bKeyRelease)
	//	{
	//		RigidBody()->SetKeyRelease(true);
	//		m_bKeyRelease = false;
	//	}

	//	if (!m_bJump)
	//	{
	//		if (KEY_PRESSED(KEY::LEFT))
	//		{
	//			Vec3 HitNormal = m_pWallRay->Hit.HitNormal.Normalize();
	//			Vec3 vMove = Vec3(-1.f, 0.f, 0.f).Normalize();

	//			Vec3 vRealMove = vMove + (HitNormal * ((-vMove).Dot(HitNormal)));
	//			vRealMove *= 2.f;

	//			RigidBody()->AddVelocity(vRealMove);
	//		}

	//		if (KEY_PRESSED(KEY::RIGHT))
	//		{
	//			Vec3 HitNormal = m_pWallRay->Hit.HitNormal.Normalize();
	//			Vec3 vMove = Vec3(1.f, 0.f, 0.f).Normalize();

	//			Vec3 vRealMove = vMove + (HitNormal * ((-vMove).Dot(HitNormal)));
	//			vRealMove *= 2.f;

	//			RigidBody()->AddVelocity(vRealMove);
	//		}

	//		if (KEY_PRESSED(KEY::UP))
	//		{
	//			Vec3 HitNormal = m_pWallRay->Hit.HitNormal.Normalize();
	//			Vec3 vMove = Vec3(0.f, 1.f, 0.f).Normalize();

	//			Vec3 vRealMove = vMove + (HitNormal * ((-vMove).Dot(HitNormal)));
	//			vRealMove *= 2.f;
	//			RigidBody()->AddVelocity(vRealMove);
	//		}

	//		if (KEY_PRESSED(KEY::DOWN))
	//		{
	//			Vec3 HitNormal = m_pWallRay->Hit.HitNormal.Normalize();
	//			Vec3 vMove = Vec3(0.f, -1.f, 0.f).Normalize();

	//			Vec3 vRealMove = vMove + (HitNormal * ((-vMove).Dot(HitNormal)));
	//			vRealMove *= 2.f;
	//			RigidBody()->AddVelocity(vRealMove);
	//		}
	//	}


	//	if (KEY_TAP(KEY::SPACE))
	//	{
	//		Vec3 HitNormal = m_pWallRay->Hit.HitNormal.Normalize();
	//		HitNormal *= 20000.f;
	//		RigidBody()->AddForce(HitNormal);
	//		m_bJump = true;
	//	}
	//}
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
