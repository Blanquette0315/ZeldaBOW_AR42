#include "pch.h"
#include "CHinoxScript.h"
#include "CLinkAnimScript.h"

#define Hip GetOwner()->GetChildObject()[1]->GetScript<CMonWeaponScript>()
#define arm_left GetOwner()->GetChildObject()[3]->GetScript<CMonWeaponScript>()
#define arm_right GetOwner()->GetChildObject()[4]->GetScript<CMonWeaponScript>()


CHinoxScript::CHinoxScript()
	: CMonsterScript(HINOXSCRIPT)
	, m_eHinoxState(Hinox_State::SLEEP)
	, m_bGetTree(false)
	, m_fGuardtime(0)
	, m_iGuardMotion(0)
	, m_bEyeGuard(false)
	, m_iMaxHP(0)
	, m_pBossName(nullptr)
	, m_pBossHPUI(nullptr)
	, m_pBossHPMaxUI(nullptr)
{
	m_pBossHP = CResMgr::GetInst()->Load<CPrefab>(L"prefab\\UI_BossHP.pref", L"prefab\\UI_BossHP.pref");
}

//CHinoxScript::CHinoxScript(const CHinoxScript& _origin)
//	: CMonsterScript(HINOXSCRIPT)
//{
//}

CHinoxScript::~CHinoxScript()
{
}

void CHinoxScript::SearchTree()
{
}

void CHinoxScript::GuardReset()
{
	m_bEyeGuard = false;
	Animator3D()->StopLowerAnim();
	m_fGuardtime = 0;
	m_iGuardMotion = 0;
}

void CHinoxScript::Damage(int _iNumber, Vec3 _vPos)
{
	if (m_eHinoxState == Hinox_State::SLEEP)
	{
		m_eHinoxState = Hinox_State::WAKE;
		Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\hinox\\Hinox_Damage.mp3");
		pSound->Play(1, MONSTER_VOLUME, true, GetOwner());
	}
	else if (m_eHinoxState == Hinox_State::NONE)
	{
		if (m_iHP <= 0)
			return;

		m_iHP -= _iNumber;
		m_pBossHPUI->Destroy();
		m_pBossHPUI = m_pBossHP->Instantiate();
		float fCurHPbar = 400.f * m_iHP / m_iMaxHP;
		m_pBossHPUI->Transform()->SetRelativeScale(fCurHPbar, 10, 1);
		Instantiate(m_pBossHPUI, Vec3(-200.f + fCurHPbar / 2.f, 325, 1), 15);

		if (m_iHP <= 0)
		{
			Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\hinox\\Hinox_Run.mp3");
			pSound->Stop();
			m_eCurrentState = Monster_State::DEAD;
			m_fAcctime = 0.f;
			m_iMotion = 0;
			AI->Done(false);
			GuardReset();
			Animator3D()->Play(L"Dead", false);
			pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\hinox\\Hinox_Dead.mp3");
			pSound->Play(1, MONSTER_VOLUME, true, GetOwner());
			m_pBossHPMaxUI->Destroy();
			m_pBossHPUI->Destroy();
			m_pBossName->Destroy();

			for (UINT i = 0; i < MeshRender()->GetMtrlCount(); ++i)
			{
				Ptr<CMaterial> pMaterial = MeshRender()->GetCurMaterial(i);
				int j = 1;
				pMaterial->SetScalarParam(INT_0, &j);
				pMaterial->SetScalarParam(FLOAT_1, &m_fAcctime);
				float alltime = 4.8f;
				pMaterial->SetScalarParam(FLOAT_2, &alltime);
			}
			GetOwner()->GetChildObject()[4]->Destroy();
			GetOwner()->GetChildObject()[3]->Destroy();
			GetOwner()->GetChildObject()[1]->Destroy();
		}
		else if (_vPos.z != 10000.f && Head->HeadCheck(_vPos) && m_eCurrentState != Monster_State::GROGGY && !m_bEyeGuard)
		{
			Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\hinox\\Hinox_Run.mp3");
			pSound->Stop();
			m_eCurrentState = Monster_State::GROGGY;
			m_fAcctime = 0.f;
			m_iMotion = 0;
			AI->Done(false);
			GuardReset();
			m_bEyeGuard = true;
			Animator3D()->Play(L"Damage_Eye_Start", false);
			pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\hinox\\Hinox_DamageEye.mp3");
			pSound->Play(1, MONSTER_VOLUME, true, GetOwner());
		}
		else if (_vPos.y == 10000.f)
		{
			Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\hinox\\Hinox_Run.mp3");
			pSound->Stop();
			m_eCurrentState = Monster_State::DAMAGED_BIG;
			m_fAcctime = 0.f;
			m_iMotion = 0;
			AI->Done(false);
			GuardReset();
			Animator3D()->Play(L"damage", false);
			pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\hinox\\Hinox_Damage.mp3");
			pSound->Play(1, MONSTER_VOLUME, true, GetOwner());
		}
	}
}

void CHinoxScript::begin()
{
	Animator3D()->ResizeVecBone();
	Animator3D()->SetBoneUpperAndElseLower(9, 22);
	Animator3D()->CreateBoneCheckBuffer();
	Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\hinox\\Hinox_Sleep.mp3");
	pSound->Play(0, MONSTER_VOLUME, false, GetOwner());
	//RigidBody()->SetGround(true);
}

void CHinoxScript::tick()
{
	RigidBody()->SetKeyRelease(true);
	switch (m_eHinoxState)
	{
	case Hinox_State::NONE:
		break;
	case Hinox_State::SLEEP:
	{
		Animator3D()->Play(L"Sleep_Loop", true);
		return;
	}
		break;
	case Hinox_State::WAKE:
	{
		m_fAcctime += FDT;

		if (m_iMotion == 0)
		{
			Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\hinox\\Hinox_Sleep.mp3");
			pSound->Stop();
			pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\hinox\\Hinox_DownRecover.mp3");
			pSound->Play(1, MONSTER_VOLUME * 0.8f, true, GetOwner());
			Animator3D()->Play(L"Sleep_End", false);
			++m_iMotion;
		}
		else if (m_fAcctime >= 4.73333f && m_iMotion == 1)
		{
			Animator3D()->Play(L"cry", false);
			Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\hinox\\Hinox_Cry.mp3");
			pSound->Play(1, MONSTER_VOLUME, true, GetOwner());
			++m_iMotion;
		}
		else if (m_fAcctime >= 9.23333f && m_iMotion == 2)
		{
			m_iMaxHP = m_iHP;
			m_pBossHPMaxUI = CResMgr::GetInst()->FindRes<CPrefab>(L"prefab\\UI_BossHPMax.pref")->Instantiate();
			m_pBossName = CResMgr::GetInst()->FindRes<CPrefab>(L"prefab\\UI_Hinox.pref")->Instantiate();
			m_pBossHPUI = m_pBossHP->Instantiate();
			Instantiate(m_pBossHPMaxUI, Vec3(0, 325, 2), 15);
			Instantiate(m_pBossName, Vec3(0, 370, 1), 15);
			Instantiate(m_pBossHPUI, Vec3(0, 325, 1), 15);
			Animator3D()->Play(L"Wait_Battle", true);
			m_iMotion = 0;
			m_fAcctime = 0;
			AI->Done();
			m_eHinoxState = Hinox_State::NONE;
		}

		return;
	}
		break;
	case Hinox_State::SEARCH_TREE:
	{
		SearchTree();
		return;
	}
		break;
	}

	if (m_eCurrentState == Monster_State::IDLE)
	{
		if(m_bGetTree)
			Animator3D()->Play(L"Wait_Battle_Sword", true);
		else
			Animator3D()->Play(L"Wait_Battle", true);
	}
	else if (m_eCurrentState == Monster_State::RUN)
	{
		Vec3 vPlayerDir = AI->FindPlayerDir();
		Vec3 vDir = Vec3(vPlayerDir.x, 0, vPlayerDir.z).Normalize();
		if (vDir.x >= 0)
			Transform()->SetRelativeRotation(0.f, acosf(vDir.z) + XM_PI, 0.f);
		else
			Transform()->SetRelativeRotation(0.f, -acosf(vDir.z) + XM_PI, 0.f);

		RigidBody()->SetVelocity(vPlayerDir * m_fSpeed);

		if (m_iGuardMotion >= 1)
		{
			m_fGuardtime += FDT;

			if (!AI->GetPlayer()->GetScript<CLinkAnimScript>()->IsBowAtkAnim() && m_fGuardtime >= 1.6666f)
			{
				if (m_iGuardMotion == 3 || m_iGuardMotion == 2)
				{
					m_fGuardtime = 5.f;
					m_iGuardMotion = 4;
				}
			}

			if (m_iGuardMotion == 1)
			{
				Animator3D()->PlayLowerAnim(L"run", true, false);
				Animator3D()->Play(L"Guard_Eye_Start", false);
				Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\hinox\\Hinox_Guard.mp3");
				pSound->Play(1, MONSTER_VOLUME, true, GetOwner());
				++m_iGuardMotion;
			}
			else if (m_fGuardtime >= 1.6666f && m_iGuardMotion == 2)
			{
				m_bEyeGuard = true;
				Animator3D()->Play(L"Guard_Eye_Loop", false);
				++m_iGuardMotion;
			}

			if (m_fGuardtime >= 6.f && m_iGuardMotion == 4)
			{
				Animator3D()->Play(L"Guard_Eye_End", false);
				m_bEyeGuard = false;
				++m_iGuardMotion;
			}
			else if (m_fGuardtime >= 6.6666f && m_iGuardMotion == 5)
			{
				Animator3D()->Play(L"run", true, false);
				Animator3D()->StopLowerAnim();
				m_iGuardMotion = 0;
				m_fGuardtime = 0;
			}
		}
		else
		{
			Animator3D()->Play(L"run", true);
			Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\hinox\\Hinox_Run.mp3");
			pSound->Play(0, MONSTER_VOLUME * 0.5f, false, GetOwner());
		}

		if (AI->GetPlayer()->GetScript<CLinkAnimScript>()->IsBowAtkAnim() && m_iGuardMotion == 0)
		{
			m_fGuardtime += FDT;
			if (m_fGuardtime >= 1.2f)
				++m_iGuardMotion;
		}
		else if(!AI->GetPlayer()->GetScript<CLinkAnimScript>()->IsBowAtkAnim() && m_iGuardMotion == 0)
		{
			m_fGuardtime = 0;
		}

		//m_fAcctime += FDT;
		
		//if (m_fAcctime > 2.5f && !m_bGetTree)
		//{
		//	AI->Done(false);
		//	m_fAcctime = 0.f;
		//	m_eHinoxState = Hinox_State::SEARCH_TREE;
		//}
	}
	else if (m_eCurrentState == Monster_State::ATTACK)
	{
		m_fAcctime += FDT;

		if (m_bGetTree)
		{
			if (m_iMotion == 0)
			{
				Vec3 vPlayerDir = AI->FindPlayerDir();
				Vec3 vDir = Vec3(vPlayerDir.x, 0, vPlayerDir.z).Normalize();
				if (vDir.x >= 0)
					m_vFront = Vec3(0.f, acosf(vDir.z) + XM_PI, 0.f);
				else
					m_vFront = Vec3(0.f, -acosf(vDir.z) + XM_PI, 0.f);
				Animator3D()->Play(L"Wait_Battle_Sword", true);
				++m_iMotion;
			}
			else if (m_fAcctime >= 0.6f && m_iMotion == 1)
			{
				if (AI->GetPlayerDistance() <= 20.f)
				{
					Vec3 vPlayerDir = AI->FindPlayerDir();
					Vec3 vDir = Vec3(vPlayerDir.x, 0, vPlayerDir.z).Normalize();
					if (vDir.x >= 0)
						m_vFront = Vec3(0.f, acosf(vDir.z) + XM_PI, 0.f);
					else
						m_vFront = Vec3(0.f, -acosf(vDir.z) + XM_PI, 0.f);
					Animator3D()->Play(L"Attack_Tree", false);
					++m_iMotion;
				}
				else
				{
					Vec3 vPlayerDir = AI->FindPlayerDir();
					Vec3 vDir = Vec3(vPlayerDir.x, 0, vPlayerDir.z).Normalize();
					if (vDir.x >= 0)
						m_vFront = Vec3(0.f, acosf(vDir.z) + XM_PI, 0.f);
					else
						m_vFront = Vec3(0.f, -acosf(vDir.z) + XM_PI, 0.f);
					Animator3D()->Play(L"Attack_Tree_Throw", false);
					++m_iMotion;
				}
			}
			else if (m_fAcctime >= 4.6f && m_iMotion == 2)
			{
				Animator3D()->Play(L"Wait_Battle_Sword", true);
				++m_iMotion;
			}
			else if (m_fAcctime >= 5.1f && m_iMotion == 3)
			{
				AI->Done();
				m_iMotion = 0;
				m_fAcctime = 0;
			}
		}
		else
		{
			if (m_iMotion == 0)
			{
				Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\hinox\\Hinox_Run.mp3");
				pSound->Stop();
				Vec3 vPlayerDir = AI->FindPlayerDir();
				Vec3 vDir = Vec3(vPlayerDir.x, 0, vPlayerDir.z).Normalize();
				if (vDir.x >= 0)
					m_vFront = Vec3(0.f, acosf(vDir.z) + XM_PI, 0.f);
				else
					m_vFront = Vec3(0.f, -acosf(vDir.z) + XM_PI, 0.f);
				GuardReset();
				m_bEyeGuard = false;
				Animator3D()->Play(L"Wait_Battle", true);
				++m_iMotion;
			}
			else if (m_fAcctime >= 0.6f && m_iMotion == 1)
			{
				if (AI->GetPlayerDistance() <= 68.f)
				{
					Animator3D()->Play(L"Attack_Hipdrop", false);
					Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\hinox\\Hinox_HipDrop.mp3");
					pSound->Play(1, MONSTER_VOLUME, true, GetOwner());
					++m_iMotion;
				}
				else
				{
					Vec3 vPlayerDir = AI->FindPlayerDir();
					Vec3 vDir = Vec3(vPlayerDir.x, 0, vPlayerDir.z).Normalize();
					if (vDir.x >= 0)
						m_vFront = Vec3(0.f, acosf(vDir.z) + XM_PI, 0.f);
					else
						m_vFront = Vec3(0.f, -acosf(vDir.z) + XM_PI, 0.f);
					Animator3D()->Play(L"Attack_Clap", false);
					Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\hinox\\Hinox_AttackHand.mp3");
					pSound->Play(1, MONSTER_VOLUME, true, GetOwner());
					m_iMotion = 5;
				}
			}
			else if (m_fAcctime >= 1.6f && m_iMotion == 5)
			{
				Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\hinox\\Hinox_AttackHandClap.mp3");
				pSound->Play(1, MONSTER_VOLUME * 0.8f, true, GetOwner());
				++m_iMotion;
			}
			else if (m_fAcctime >= 1.8f && m_iMotion == 6)
			{
				arm_left->setReady(true);
				arm_right->setReady(true);
				++m_iMotion;
			}
			else if (m_fAcctime >= 1.9f && m_iMotion == 7)
			{
				Vec3 vClapPos = Transform()->GetRelativePos();
				vClapPos.y += 8.f;
				vClapPos.x += Transform()->GetRelativeDir(DIR::FRONT).x * 70.f;
				vClapPos.z += Transform()->GetRelativeDir(DIR::FRONT).z * 70.f;
				Instantiate(CResMgr::GetInst()->Load<CPrefab>(L"prefab\\Dust_Particle_clap.pref", L"prefab\\Dust_Particle_clap.pref")->Instantiate(), vClapPos, 0);
				++m_iMotion;
			}
			else if (m_fAcctime >= 2.1f && m_iMotion == 8)
			{
				arm_left->setReady(false);
				arm_right->setReady(false);
				++m_iMotion;
			}
			else if (m_fAcctime >= 5.1f && m_iMotion == 9)
			{
				Animator3D()->Play(L"Wait_Battle", true);
				++m_iMotion;
			}
			else if (m_fAcctime >= 2.9 && m_iMotion == 2)
			{
				Hip->setReady(true);
				Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\hinox\\Hinox_DownHip.mp3");
				Instantiate(CResMgr::GetInst()->Load<CPrefab>(L"prefab\\Dust_Particle.pref", L"prefab\\Dust_Particle.pref")->Instantiate(), Transform()->GetRelativePos(), 0);
				pSound->Play(1, MONSTER_VOLUME * 0.8f, true, GetOwner());
				++m_iMotion;
			}
			else if (m_iMotion == 3)
			{
				Hip->setReady(false);
				++m_iMotion;
			}
			else if (m_fAcctime >= 6.3666f && m_iMotion == 4)
			{
				Animator3D()->Play(L"Wait_Battle", true);
				m_iMotion = 10;
			}
			else if (m_fAcctime >= 7.5f && m_iMotion == 10)
			{
				AI->Done();
				m_fAcctime = 0;
				m_iMotion = 0;
			}
		}
		
		Transform()->SetRelativeRotation(m_vFront);
	}
	else if (m_eCurrentState == Monster_State::FIND)
	{
		AI->Done();
	}
	else if (m_eCurrentState == Monster_State::RETURN)
	{
		AI->Done();
	}
	else if (m_eCurrentState == Monster_State::DAMAGED_BIG)
	{
		m_fAcctime += FDT;
		if (m_fAcctime >= 1.4f)
		{
			AI->Done();
			m_fAcctime = 0;
			m_iMotion = 0;
			m_eCurrentState = Monster_State::IDLE;
		}
	}
	else if (m_eCurrentState == Monster_State::GROGGY)
	{
		m_fAcctime += FDT;
		if (m_fAcctime >= 2.f && m_iMotion == 0)
		{
			Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\hinox\\Hinox_DamageEyeDown.mp3");
			pSound->Play(1, MONSTER_VOLUME * 0.8f, true, GetOwner());
			++m_iMotion;
		}
		else if (m_fAcctime >= 2.3666f && m_iMotion == 1)
		{
			Animator3D()->Play(L"Damage_Eye_Loop", true);
			++m_iMotion;
		}
		else if (m_fAcctime >= 4.8666 && m_iMotion == 2)
		{
			Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\hinox\\Hinox_DownRecover.mp3");
			pSound->Play(1, MONSTER_VOLUME * 0.8f, true, GetOwner());
			Animator3D()->Play(L"Damage_Eye_End", false);
			++m_iMotion;
		}
		else if (m_fAcctime >= 7.9332 && m_iMotion == 3)
		{
			AI->Done();
			m_fAcctime = 0;
			m_iMotion = 0;
			m_bEyeGuard = false;
			m_eCurrentState = Monster_State::IDLE;
		}
	}
	else if (m_eCurrentState == Monster_State::DEAD)
	{
		m_fAcctime += FDT;
		if (m_fAcctime >= 4.8f)
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

void CHinoxScript::BeginOverlap(CGameObject* _pOther)
{
}

void CHinoxScript::Overlap(CGameObject* _pOther)
{
}

void CHinoxScript::EndOverlap(CGameObject* _pOther)
{
}

void CHinoxScript::SaveToYAML(YAML::Emitter& _emitter)
{
	CMonsterScript::SaveToYAML(_emitter);
}

void CHinoxScript::LoadFromYAML(YAML::Node& _node)
{
	CMonsterScript::LoadFromYAML(_node);
}