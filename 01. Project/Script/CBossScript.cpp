#include "pch.h"
#include "CBossScript.h"

CBossScript::CBossScript()
	: CMonsterScript(BOSSSCRIPT)
	, m_iAttackMotion(0)
	, m_pFireball(nullptr)
{
}

CBossScript::~CBossScript()
{
}

void CBossScript::Damage(int _iNumber, Vec3 _vPos)
{
	if (m_iHP <= 0)
		return;

	m_iHP -= _iNumber;

	if (m_iHP <= 0)
	{
		m_eCurrentState = Monster_State::DEAD;
		m_fAcctime = 0.f;
		m_iMotion = 0;
		AI->Done(false);
		Animator3D()->Play(L"Stagger", false);

		for (UINT i = 0; i < MeshRender()->GetMtrlCount(); ++i)
		{
			Ptr<CMaterial> pMaterial = MeshRender()->GetCurMaterial(i);
			int j = 1;
			pMaterial->SetScalarParam(INT_0, &j);
			pMaterial->SetScalarParam(FLOAT_1, &m_fAcctime);
			float alltime = 4.f;
			pMaterial->SetScalarParam(FLOAT_2, &alltime);
		}

		GetOwner()->GetChildObject()[1]->Destroy();
	}
	else if (_vPos.z != 10000.f && Head->HeadCheck(_vPos))
	{
		m_eCurrentState = Monster_State::GROGGY;
		m_fAcctime = 0.f;
		m_iMotion = 0;
		AI->Done(false);
		Animator3D()->Play(L"Attack_Eye_End", false);
	}
	else if (m_eCurrentState == Monster_State::RUN)
	{
		m_eCurrentState = Monster_State::DAMAGED;
		m_fAcctime = 0.f;
		m_iMotion = 0;
		AI->Done(false);
		Animator3D()->Play(L"Damage_Small", false);
	}
}

void CBossScript::begin()
{
	RigidBody()->SetGround(true);
}

void CBossScript::tick()
{
	RigidBody()->SetKeyRelease(true);

	if (m_eCurrentState == Monster_State::IDLE)
	{
		Animator3D()->Play(L"Wait_Battle", true);
	}
	else if (m_eCurrentState == Monster_State::WALK)
	{
		Vec3 vPlayerDir = AI->FindPlayerDir();
		Vec3 vDir = Vec3(vPlayerDir.x, 0, vPlayerDir.z).Normalize();
		if (vDir.x >= 0)
			Transform()->SetRelativeRotation(0.f, acosf(vDir.z) + XM_PI, 0.f);
		else
			Transform()->SetRelativeRotation(0.f, -acosf(vDir.z) + XM_PI, 0.f);

		Animator3D()->Play(L"Wait_Battle", true);
		RigidBody()->SetVelocity(vPlayerDir * m_fSpeed);

	}
	else if (m_eCurrentState == Monster_State::ATTACK)
	{
		if (m_iAttackMotion == 0)
			Attack_Straight();
		else if (m_iAttackMotion == 1)
			Attack_Cross();
		else if (m_iAttackMotion == 2)
			Attack_Rotate();
	}
	else if (m_eCurrentState == Monster_State::RUN)
	{
		Vec3 vPlayerDir = AI->FindPlayerDir();
		Vec3 vDir = Vec3(vPlayerDir.x, 0, vPlayerDir.z).Normalize();
		if (vDir.x >= 0)
			Transform()->SetRelativeRotation(0.f, acosf(vDir.z) + XM_PI, 0.f);
		else
			Transform()->SetRelativeRotation(0.f, -acosf(vDir.z) + XM_PI, 0.f);

		Animator3D()->Play(L"Wait_Battle", true);
		RigidBody()->SetVelocity(vPlayerDir * m_fSpeed);
	}
	else if (m_eCurrentState == Monster_State::FIND)
	{
		AI->Done();
	}
	else if (m_eCurrentState == Monster_State::RETURN)
	{
		AI->Done();
	}
	else if (m_eCurrentState == Monster_State::DAMAGED)
	{
		m_fAcctime += FDT;
		if (m_fAcctime >= 0.8f)
		{
			AI->Done();
			m_fAcctime = 0;
			m_eCurrentState = Monster_State::IDLE;
		}
	}
	else if (m_eCurrentState == Monster_State::GROGGY)
	{
		m_fAcctime += FDT;
		if (m_fAcctime >= 2.0666f && m_iMotion == 0)
		{
			Animator3D()->Play(L"Wait_Battle", true);
			++m_iMotion;
		}
		else if (m_fAcctime >= 6.7332 && m_iMotion == 1)
		{
			AI->Done();
			m_fAcctime = 0;
			m_iMotion = 0;
			m_eCurrentState = Monster_State::IDLE;
		}
	}
	else if (m_eCurrentState == Monster_State::DEAD)
	{
		m_fAcctime += FDT;
		if (m_fAcctime >= 4.f)
		{
			GetOwner()->Destroy();
		}
		else
		{
			for (UINT i = 0; i < MeshRender()->GetMtrlCount(); ++i)
			{
				Ptr<CMaterial> pMaterial = MeshRender()->GetCurMaterial(i);
				pMaterial->SetScalarParam(FLOAT_1, &m_fAcctime);
			}
		}
	}
}

void CBossScript::BeginOverlap(CGameObject* _pOther)
{
}

void CBossScript::Overlap(CGameObject* _pOther)
{
}

void CBossScript::EndOverlap(CGameObject* _pOther)
{
}

void CBossScript::SaveToYAML(YAML::Emitter& _emitter)
{
	CMonsterScript::SaveToYAML(_emitter);
}

void CBossScript::LoadFromYAML(YAML::Node& _node)
{
	CMonsterScript::LoadFromYAML(_node);
}

void CBossScript::Attack_Cross()
{
	m_fAcctime += FDT;

	if (m_iMotion == 0)
	{
		Vec3 vPlayerDir = AI->FindPlayerDir();
		Vec3 vDir = Vec3(vPlayerDir.x, 0, vPlayerDir.z).Normalize();
		if (vDir.x >= 0)
			m_vFront = Vec3(0.f, acosf(vDir.z) + XM_PI, 0.f);
		else
			m_vFront = Vec3(0.f, -acosf(vDir.z) + XM_PI, 0.f);
		Animator3D()->Play(L"Attack_Lsword_Cross", false);
		++m_iMotion;
		Transform()->SetRelativeRotation(m_vFront);
	}
	else if (m_fAcctime >= 2.f && m_iMotion == 1)
	{
		Weapon->setReady(true);
		++m_iMotion;
	}
	else if (m_fAcctime >= 2.6666f && m_iMotion == 2)
	{
		Weapon->setReady(false);
		++m_iMotion;
	}
	else if (m_fAcctime >= 4.f && m_iMotion == 3)
	{
		Animator3D()->Play(L"Wait_Battle", true);
		++m_iMotion;
	}
	else if (m_fAcctime >= 8.6666f && m_iMotion == 4)
	{
		AI->Done();
		m_iMotion = 0;
		m_fAcctime = 0;
		++m_iAttackMotion;
	}
}

void CBossScript::Attack_Rotate()
{
	m_fAcctime += FDT;

	if (m_iMotion == 0)
	{
		Vec3 vPlayerDir = AI->FindPlayerDir();
		Vec3 vDir = Vec3(vPlayerDir.x, 0, vPlayerDir.z).Normalize();
		if (vDir.x >= 0)
			m_vFront = Vec3(0.f, acosf(vDir.z) + XM_PI, 0.f);
		else
			m_vFront = Vec3(0.f, -acosf(vDir.z) + XM_PI, 0.f);
		Animator3D()->Play(L"Attack_Lsword_Rotate", false);
		++m_iMotion;
		Transform()->SetRelativeRotation(m_vFront);
	}
	else if (m_fAcctime >= 2.6666f && m_iMotion == 1)
	{
		Weapon->setReady(true);
		++m_iMotion;
	}
	else if (m_fAcctime >= 3.2f && m_iMotion == 2)
	{
		Weapon->setReady(true);
		++m_iMotion;
	}
	else if (m_fAcctime >= 3.6666f && m_iMotion == 3)
	{
		Weapon->setReady(true);
		++m_iMotion;
	}
	else if (m_fAcctime >= 4.f && m_iMotion == 4)
	{
		Weapon->setReady(false);
		++m_iMotion;
	}
	else if (m_fAcctime >= 5.2f && m_iMotion == 5)
	{
		Animator3D()->Play(L"Wait_Battle", true);
		++m_iMotion;
	}
	else if (m_fAcctime >= 9.8666f && m_iMotion == 6)
	{
		AI->Done();
		m_iMotion = 0;
		m_fAcctime = 0;
		m_iAttackMotion = 0;
	}
}

void CBossScript::Attack_Straight()
{
	m_fAcctime += FDT;

	if (m_iMotion == 0)
	{
		Vec3 vPlayerDir = AI->FindPlayerDir();
		Vec3 vDir = Vec3(vPlayerDir.x, 0, vPlayerDir.z).Normalize();
		if (vDir.x >= 0)
			m_vFront = Vec3(0.f, acosf(vDir.z) + XM_PI, 0.f);
		else
			m_vFront = Vec3(0.f, -acosf(vDir.z) + XM_PI, 0.f);
		Animator3D()->Play(L"Attack_Lsword_Straight", false);
		++m_iMotion;
		Transform()->SetRelativeRotation(m_vFront);
	}
	else if (m_fAcctime >= 3.f && m_iMotion == 1)
	{
		Weapon->setReady(true);
		++m_iMotion;
	}
	else if (m_fAcctime >= 4.3333f && m_iMotion == 2)
	{
		Weapon->setReady(false);
		++m_iMotion;
	}
	else if (m_fAcctime >= 5.5f && m_iMotion == 3)
	{
		Animator3D()->Play(L"Wait_Battle", true);
		++m_iMotion;
	}
	else if (m_fAcctime >= 10.1666f && m_iMotion == 4)
	{
		AI->Done();
		m_iMotion = 0;
		m_fAcctime = 0;
		++m_iAttackMotion;
	}
}

void CBossScript::Attack_Chemical()
{
	m_fAcctime += FDT;

	if (m_iMotion <= 1)
	{
		Vec3 vPlayerDir = AI->FindPlayerDir();
		vPlayerDir.x *= -1;
		vPlayerDir.z *= -1;
		RigidBody()->SetVelocity(vPlayerDir * m_fSpeed * 0.5f);
	}

	if (m_iMotion == 0)
	{
		Vec3 vPlayerDir = AI->FindPlayerDir();
		Vec3 vDir = Vec3(vPlayerDir.x, 0, vPlayerDir.z).Normalize();
		if (vDir.x >= 0)
			m_vFront = Vec3(0.f, acosf(vDir.z) + XM_PI, 0.f);
		else
			m_vFront = Vec3(0.f, -acosf(vDir.z) + XM_PI, 0.f);
		Animator3D()->Play(L"Attack_Chemical_Start", false);
		++m_iMotion;
		Transform()->SetRelativeRotation(m_vFront);
	}
	else if (m_fAcctime >= 1.3f && m_iMotion == 1)
	{
		/*Ptr<CPrefab> pFireball =
		m_pFireball = m_pArrowPref->Instantiate();
		Instantiate(m_pArrowObj, Vec3::Zero, 0);
		AddChild(GetOwner()->GetParent(), m_pArrowObj);
		m_pArrowObj->GetScript<CBonesocketScript>()->setBoneIdx((UINT)LINK_BONE_STRING::Weapon_R);
		m_bOnce = true;*/
		++m_iMotion;
	}
	else if (m_fAcctime >= 4.3333f && m_iMotion == 2)
	{
		Weapon->setReady(false);
		++m_iMotion;
	}
	else if (m_fAcctime >= 5.5f && m_iMotion == 3)
	{
		Animator3D()->Play(L"Wait_Battle", true);
		++m_iMotion;
	}
	else if (m_fAcctime >= 10.1666f && m_iMotion == 4)
	{
		AI->Done();
		m_iMotion = 0;
		m_fAcctime = 0;
		++m_iAttackMotion;
	}
}

void CBossScript::Attack_Chemical_Big()
{
}