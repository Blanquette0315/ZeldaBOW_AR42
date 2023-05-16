#include "pch.h"
#include "CBokoblinScript.h"

CBokoblinScript::CBokoblinScript()
	: CMonsterScript(BOKOBLINSCRIPT)
{
}

CBokoblinScript::~CBokoblinScript()
{
}

void CBokoblinScript::Damage(int _iNumber, Vec3 _vPos)
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
		Animator3D()->Play(L"sleep_start", false);

		for (UINT i = 0; i < MeshRender()->GetMtrlCount(); ++i)
		{
			Ptr<CMaterial> pMaterial = MeshRender()->GetCurMaterial(i);
			int j = 1;
			pMaterial->SetScalarParam(INT_0, &j);
			pMaterial->SetScalarParam(FLOAT_1, &m_fAcctime);
			float alltime = 4.5f;
			pMaterial->SetScalarParam(FLOAT_2, &alltime);
		}

		GetOwner()->GetChildObject()[1]->Destroy();
	}
}

void CBokoblinScript::begin()
{
	RigidBody()->SetGround(true);
}

void CBokoblinScript::tick()
{
	RigidBody()->SetKeyRelease(true);

	if (m_eCurrentState == Monster_State::IDLE)
	{
		Animator3D()->Play(L"wait", true);
	}
	else if (m_eCurrentState == Monster_State::WALK)
	{
		Vec3 vPlayerDir = AI->FindPlayerDir();
		Vec3 vDir = Vec3(vPlayerDir.x, 0, vPlayerDir.z).Normalize();
		if (vDir.x >= 0)
			Transform()->SetRelativeRotation(0.f, acosf(vDir.z) + XM_PI, 0.f);
		else
			Transform()->SetRelativeRotation(0.f, -acosf(vDir.z) + XM_PI, 0.f);

		Animator3D()->Play(L"walk", true);
		RigidBody()->SetVelocity(vPlayerDir * m_fSpeed);

	}
	else if (m_eCurrentState == Monster_State::ATTACK)
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
			Animator3D()->Play(L"jump", false);
			++m_iMotion;
		}
		else if (m_fAcctime >= 1.63333f && m_iMotion == 1)
		{
			Vec3 vPlayerDir = AI->FindPlayerDir();
			Vec3 vDir = Vec3(vPlayerDir.x, 0, vPlayerDir.z).Normalize();
			if (vDir.x >= 0)
				m_vFront = Vec3(0.f, acosf(vDir.z) + XM_PI, 0.f);
			else
				m_vFront = Vec3(0.f, -acosf(vDir.z) + XM_PI, 0.f);
			Animator3D()->Play(L"attack_small", false);
			++m_iMotion;
		}
		else if (m_fAcctime >= 1.7f && m_iMotion == 2)
		{
			Weapon->setReady(true);
			++m_iMotion;
		}
		else if (m_fAcctime >= 3.13333f && m_iMotion == 3)
		{
			Weapon->setReady(false);
			Animator3D()->Play(L"wait", false);
			++m_iMotion;
		}
		else if (m_fAcctime >= 4.5f && m_iMotion == 4)
		{
			AI->Done();
			m_iMotion = 0;
			m_fAcctime = 0;
		}
		Transform()->SetRelativeRotation(m_vFront);
	}
	else if (m_eCurrentState == Monster_State::RUN)
	{
		Vec3 vPlayerDir = AI->FindPlayerDir();
		Vec3 vDir = Vec3(vPlayerDir.x, 0, vPlayerDir.z).Normalize();
		if (vDir.x >= 0)
			Transform()->SetRelativeRotation(0.f, acosf(vDir.z) + XM_PI, 0.f);
		else
			Transform()->SetRelativeRotation(0.f, -acosf(vDir.z) + XM_PI, 0.f);

		Animator3D()->Play(L"run", true);
		RigidBody()->SetVelocity(vPlayerDir * m_fSpeed * 1.3f);
	}
	else if (m_eCurrentState == Monster_State::FIND)
	{
		Transform()->SetRelativeRotation(m_vFront);

		m_fAcctime += FDT;
		if (m_iMotion == 0)
		{
			Vec3 vPlayerDir = AI->FindPlayerDir();
			Vec3 vDir = Vec3(vPlayerDir.x, 0, vPlayerDir.z).Normalize();
			if (vDir.x >= 0)
				m_vFront = Vec3(0.f, acosf(vDir.z) + XM_PI, 0.f);
			else
				m_vFront = Vec3(0.f, -acosf(vDir.z) + XM_PI, 0.f);

			Animator3D()->Play(L"jump", false);
			++m_iMotion;
		}
		else if (m_fAcctime >= 1.63333f && m_iMotion == 1)
		{
			Animator3D()->Play(L"wait", true);
			++m_iMotion;
		}
		else if (m_fAcctime >= 2.f)
		{
			AI->Done();
			m_iMotion = 0;
			m_fAcctime = 0;
		}
	}
	else if (m_eCurrentState == Monster_State::RETURN)
	{
		Vec3 vInitialPosDir = AI->FindInitialPosDir();
		Vec3 vDir = Vec3(vInitialPosDir.x, 0, vInitialPosDir.z).Normalize();
		if (vDir.x >= 0)
			Transform()->SetRelativeRotation(0.f, acosf(vDir.z) + XM_PI, 0.f);
		else
			Transform()->SetRelativeRotation(0.f, -acosf(vDir.z) + XM_PI, 0.f);

		Animator3D()->Play(L"run", true);
		RigidBody()->SetVelocity(vInitialPosDir * m_fSpeed * 1.3f);
	}
	else if (m_eCurrentState == Monster_State::DEAD)
	{
		m_fAcctime += FDT;
		if (m_fAcctime >= 4.5f)
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
	else if (m_eCurrentState == Monster_State::MISS)
	{
		m_fAcctime += FDT;
		Transform()->SetRelativeRotation(m_vFront);
		if (m_iMotion == 0)
		{
			Vec3 vPlayerDir = AI->FindPlayerDir();
			Vec3 vDir = Vec3(vPlayerDir.x, 0, vPlayerDir.z).Normalize();
			if (vDir.x >= 0)
				m_vFront = Vec3(0.f, acosf(vDir.z) + XM_PI, 0.f);
			else
				m_vFront = Vec3(0.f, -acosf(vDir.z) + XM_PI, 0.f);

			Animator3D()->Play(L"rebound", false);
			++m_iMotion;
		}
		else if (m_fAcctime >= 1.f && m_iMotion == 1)
		{
			Animator3D()->Play(L"wait", true);
			++m_iMotion;
		}
		else if (m_fAcctime >= 2.f)
		{
			AI->Done();
			m_iMotion = 0;
			m_fAcctime = 0;
		}
	}
}

void CBokoblinScript::BeginOverlap(CGameObject* _pOther)
{
}

void CBokoblinScript::Overlap(CGameObject* _pOther)
{
}

void CBokoblinScript::EndOverlap(CGameObject* _pOther)
{
}

void CBokoblinScript::SaveToYAML(YAML::Emitter& _emitter)
{
	CMonsterScript::SaveToYAML(_emitter);
}

void CBokoblinScript::LoadFromYAML(YAML::Node& _node)
{
	CMonsterScript::LoadFromYAML(_node);
}

