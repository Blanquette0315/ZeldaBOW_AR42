#include "pch.h"
#include "CBossScript.h"

#include "CBossFireballScript.h"

CBossScript::CBossScript()
	: CMonsterScript(BOSSSCRIPT)
	, m_iAttackMotion(0)
	, m_pFireball(nullptr)
	, m_pFlame(nullptr)
	, m_pFireball_small(nullptr)
	, m_pFireball_big(nullptr)
{
	m_pFireball_small = CResMgr::GetInst()->Load<CPrefab>(L"prefab\\Fireball.pref", L"prefab\\Fireball.pref");
	m_pFireball_big = CResMgr::GetInst()->Load<CPrefab>(L"prefab\\Fireball_Charged.pref", L"prefab\\Fireball_Charged.pref");
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
		Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\boss\\dead.wav");
		pSound->Play(1, MONSTER_VOLUME, true, GetOwner());
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

		if (m_pFireball != nullptr)
		{
			m_pFireball->GetScript<CBossFireballScript>()->Dead();
		}
		if (m_pFlame != nullptr)
		{
			m_pFlame->Destroy();
		}

		GetOwner()->GetChildObject()[1]->Destroy();
	}
	else if (_vPos.z != 10000.f && Head->HeadCheck(_vPos) && m_eCurrentState != Monster_State::GROGGY)
	{
		m_eCurrentState = Monster_State::GROGGY;
		m_fAcctime = 0.f;
		m_iMotion = 0;
		AI->Done(false);
		Animator3D()->Play(L"Attack_Eye_End", false);
		Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\boss\\FantomGanon_Vo_DamageHalfLife.wav");
		pSound->Play(1, MONSTER_VOLUME, true, GetOwner());
		if (m_pFireball != nullptr)
		{
			m_pFireball->GetScript<CBossFireballScript>()->Dead();
		}
		if (m_pFlame != nullptr)
		{
			m_pFlame->Destroy();
		}
	}
	else if (_vPos.y == 10000.f)
	{
		m_eCurrentState = Monster_State::DAMAGED_BIG;
		m_fAcctime = 0.f;
		m_iMotion = 0;
		AI->Done(false);
		Animator3D()->Play(L"Damage_Big", false);
		Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\boss\\damage_big.wav");
		pSound->Play(1, MONSTER_VOLUME, true, GetOwner());
	}
	else if (m_eCurrentState == Monster_State::RUN)
	{
		m_eCurrentState = Monster_State::DAMAGED_SMALL;
		m_fAcctime = 0.f;
		m_iMotion = 0;
		AI->Done(false);
		Animator3D()->Play(L"Damage_Small", false);
		Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\boss\\damage.wav");
		pSound->Play(1, MONSTER_VOLUME, true, GetOwner());
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
		if (m_iAttackMotion >= 5)
		{
			m_iAttackMotion = 0;
		}

		//if (m_iAttackMotion == 0)
		//	Attack_Chemical_Big();
		//else if (m_iAttackMotion == 1)
		//	Attack_Chemical_Big();
		//else if (m_iAttackMotion == 2)
		//	Attack_Chemical_Big();
		//else if (m_iAttackMotion == 3)
		//	Attack_Chemical_Big();
		//else if (m_iAttackMotion == 4)
		//	Attack_Chemical_Big();

		if (m_iAttackMotion == 0)
			Attack_Straight();
		else if (m_iAttackMotion == 1)
			Attack_Cross();
		else if (m_iAttackMotion == 2)
			Attack_Chemical();
		else if (m_iAttackMotion == 3)
			Attack_Rotate();
		else if (m_iAttackMotion == 4)
			Attack_Chemical_Big();

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
	else if (m_eCurrentState == Monster_State::DAMAGED_SMALL)
	{
		m_fAcctime += FDT;
		if (m_fAcctime >= 0.8f)
		{
			AI->Done();
			m_fAcctime = 0;
			m_eCurrentState = Monster_State::IDLE;
		}
	}
	else if (m_eCurrentState == Monster_State::DAMAGED_BIG)
	{
		m_fAcctime += FDT;
		if (m_fAcctime >= 0.8f)
		{
			AI->Done();
			m_fAcctime = 0;
			m_iMotion = 0;
			++m_iAttackMotion;
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
			++m_iAttackMotion;
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

			Animator3D()->Play(L"Miss", false);
			++m_iMotion;
		}
		else if (m_fAcctime >= 2.f && m_iMotion == 1)
		{
			Animator3D()->Play(L"Wait_Battle", true);
			++m_iMotion;
		}
		else if (m_fAcctime >= 3.f)
		{
			AI->Done();
			m_iMotion = 0;
			m_fAcctime = 0;
			++m_iAttackMotion;
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
		Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\boss\\cross.wav");
		pSound->Play(1, MONSTER_VOLUME, true, GetOwner());
		++m_iMotion;
		Transform()->SetRelativeRotation(m_vFront);
	}
	else if (m_fAcctime >= 2.f && m_iMotion == 1)
	{
		Weapon->setReady(true);
		Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\boss\\SiteBossLsword_CrossAttackSwing.wav");
		pSound->Play(1, MONSTER_VOLUME, true, GetOwner());
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
		Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\boss\\rotate.wav");
		pSound->Play(1, MONSTER_VOLUME, true, GetOwner());
		++m_iMotion;
		Transform()->SetRelativeRotation(m_vFront);
	}
	else if (m_fAcctime >= 2.6666f && m_iMotion == 1)
	{
		Weapon->setReady(true);
		Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\boss\\SiteBossLsword_RotateAttackSwing_01.wav");
		pSound->Play(1, MONSTER_VOLUME, true, GetOwner());
		++m_iMotion;
	}
	else if (m_fAcctime >= 3.2f && m_iMotion == 2)
	{
		Weapon->setReady(true);
		Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\boss\\SiteBossLsword_RotateAttackSwing_02.wav");
		pSound->Play(1, MONSTER_VOLUME, true, GetOwner());
		++m_iMotion;
	}
	else if (m_fAcctime >= 3.6666f && m_iMotion == 3)
	{
		Weapon->setReady(true);
		Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\boss\\SiteBossLsword_RotateAttackSwing_03.wav");
		pSound->Play(1, MONSTER_VOLUME, true, GetOwner());
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
		++m_iAttackMotion;
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
		Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\boss\\Straight.wav");
		pSound->Play(1, MONSTER_VOLUME, true, GetOwner());
		++m_iMotion;
		Transform()->SetRelativeRotation(m_vFront);
	}
	else if (m_fAcctime >= 3.f && m_iMotion == 1)
	{
		Weapon->setReady(true);
		Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\boss\\SiteBossLsword_StraightAttackSwing.wav");
		pSound->Play(1, MONSTER_VOLUME, true, GetOwner());
		++m_iMotion;
	}
	else if (m_fAcctime >= 3.1333f && m_iMotion == 2)
	{
		Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\boss\\SiteBoss_SwordHitGround.wav");
		pSound->Play(1, MONSTER_VOLUME * 0.5f, true, GetOwner());
		++m_iMotion;
	}
	else if (m_fAcctime >= 4.3333f && m_iMotion == 3)
	{
		Weapon->setReady(false);
		++m_iMotion;
	}
	else if (m_fAcctime >= 5.5f && m_iMotion == 4)
	{
		Animator3D()->Play(L"Wait_Battle", true);
		++m_iMotion;
	}
	else if (m_fAcctime >= 10.1666f && m_iMotion == 5)
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
		RigidBody()->SetVelocity(vPlayerDir * m_fSpeed * 0.75f);
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
		Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\boss\\flame.wav");
		pSound->Play(1, MONSTER_VOLUME, true, GetOwner());
		++m_iMotion;
		Transform()->SetRelativeRotation(m_vFront);
	}
	else if (m_fAcctime >= 1.26666f && m_iMotion == 1)
	{
		Animator3D()->Play(L"Attack_Chemical_Loop", false);
		m_pFireball = m_pFireball_small->Instantiate();
		Instantiate(m_pFireball, Vec3::Zero, 8);
		AddChild(GetOwner(), m_pFireball);
		Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\boss\\SiteBossLsword_FlameBall_Appear.wav");
		pSound->Play(1, MONSTER_VOLUME, true, GetOwner());
		++m_iMotion;
	}
	else if (m_fAcctime >= 2.26666f && m_iMotion == 2)
	{
		Vec3 vPlayerDir = AI->FindPlayerDir();
		Vec3 vDir = Vec3(vPlayerDir.x, 0, vPlayerDir.z).Normalize();
		if (vDir.x >= 0)
			m_vFront = Vec3(0.f, acosf(vDir.z) + XM_PI, 0.f);
		else
			m_vFront = Vec3(0.f, -acosf(vDir.z) + XM_PI, 0.f);
		Transform()->SetRelativeRotation(m_vFront);
		Animator3D()->Play(L"Attack_Chemical_End", false);
		++m_iMotion;
	}
	else if (m_fAcctime >= 2.75f && m_iMotion == 3)
	{
		m_pFireball->GetScript<CBossFireballScript>()->setBossPos(Transform()->GetWorldPos());
		m_pFireball->GetScript<CBossFireballScript>()->Fire(AI->FindPlayerPos());
		Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\boss\\SiteBossLsword_FlameBall_Throw.wav");
		pSound->Play(1, MONSTER_VOLUME, true, GetOwner());
		++m_iMotion;
	}
	else if (m_fAcctime >= 4.1f && m_iMotion == 4)
	{
		Animator3D()->Play(L"Wait_Battle", true);
		++m_iMotion;
	}
	else if (!IsValid(m_pFireball) && m_iMotion == 5)
	{
		AI->Done();
		m_iMotion = 0;
		m_fAcctime = 0;
		++m_iAttackMotion;
	}
}

void CBossScript::Attack_Chemical_Big()
{
	m_fAcctime += FDT;

	if (m_iMotion <= 1)
	{
		Vec3 vPlayerDir = AI->FindPlayerDir();
		vPlayerDir.x *= -1;
		vPlayerDir.z *= -1;
		RigidBody()->SetVelocity(vPlayerDir * m_fSpeed * 0.75f);
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
		Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\boss\\flame.wav");
		pSound->Play(1, MONSTER_VOLUME, true, GetOwner());
		++m_iMotion;
		Transform()->SetRelativeRotation(m_vFront);
	}
	else if (m_fAcctime >= 1.26666f && m_iMotion == 1)
	{
		Animator3D()->Play(L"Attack_Chemical_Loop", true);
		m_pFireball = m_pFireball_big->Instantiate();
		Instantiate(m_pFireball, Vec3::Zero, 8);
		AddChild(GetOwner(), m_pFireball);
		m_pFlame = CResMgr::GetInst()->Load<CPrefab>(L"prefab\\flamecharge.pref", L"prefab\\flamecharge.pref")->Instantiate();
		Instantiate(m_pFlame, Vec3::Zero, 0);
		AddChild(GetOwner(), m_pFlame);
		Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\boss\\SiteBossLsword_FlameTornade_Charge.wav");
		pSound->Play(2, MONSTER_VOLUME, true, m_pFlame);
		++m_iMotion;
	}
	else if (m_fAcctime >= 6.26666f && m_iMotion == 2)
	{
		Vec3 vPlayerDir = AI->FindPlayerDir();
		Vec3 vDir = Vec3(vPlayerDir.x, 0, vPlayerDir.z).Normalize();
		if (vDir.x >= 0)
			m_vFront = Vec3(0.f, acosf(vDir.z) + XM_PI, 0.f);
		else
			m_vFront = Vec3(0.f, -acosf(vDir.z) + XM_PI, 0.f);
		Transform()->SetRelativeRotation(m_vFront);
		Animator3D()->Play(L"Attack_Chemical_End", false);
		++m_iMotion;
	}
	else if (m_fAcctime >= 6.75f && m_iMotion == 3)
	{
		m_pFireball->GetScript<CBossFireballScript>()->setBossPos(Transform()->GetWorldPos());
		m_pFireball->GetScript<CBossFireballScript>()->Fire(AI->FindPlayerPos());
		Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\boss\\SiteBossLsword_FlameBall_Throw.wav");
		pSound->Play(1, MONSTER_VOLUME, true, GetOwner());
		++m_iMotion;
	}
	else if (m_fAcctime >= 8.1f && m_iMotion == 4)
	{
		Animator3D()->Play(L"Wait_Battle", true);
		++m_iMotion;
	}
	else if (!IsValid(m_pFireball) && m_iMotion == 5)
	{
		AI->Done();
		m_iMotion = 0;
		m_fAcctime = 0;
		++m_iAttackMotion;
	}
}