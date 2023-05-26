#include "pch.h"
#include "CBossFireballScript.h"

#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>
#include "CLinkAnimScript.h"
#include "CLinkHitScript.h"

CBossFireballScript::CBossFireballScript()
	: CMonsterScript(BOSSFIREBALLSCRIPT)
	, m_iSize(0)
{
}

CBossFireballScript::~CBossFireballScript()
{
}

void CBossFireballScript::begin()
{
}

void CBossFireballScript::tick()
{
	if (m_eCurrentState == Monster_State::IDLE)
	{
		Vec3 vScale = Transform()->GetRelativeScale();
		if (m_iSize == 0)
		{
			if (vScale.x >= 1.f)
			{
				vScale = Vec3(1.f);
			}
			else
			{
				vScale += Vec3(1.f * FDT);
			}
		}
		else
		{
			if (vScale.x >= 2.f)
			{
				vScale = Vec3(2.f);
			}
			else
			{
				vScale += Vec3(0.4f * FDT);
			}
		}
		Transform()->SetRelativeScale(vScale);
	}
	else if (m_eCurrentState == Monster_State::ATTACK)
	{
		if (m_iMotion == 0)
		{
			++m_iMotion;
		}
		else if (m_iMotion == 1)
		{
			++m_iMotion;
			m_vDir = m_vPlayerPos + Vec3(0.f, 10.f, 0.f) - Transform()->GetWorldPos();
			m_vDir.Normalize();
		}
		Vec3 vPos = Transform()->GetRelativePos();
		vPos += m_vDir * 50.f * FDT;
		Transform()->SetRelativePos(vPos);
	}
	else if (m_eCurrentState == Monster_State::MISS)
	{
		if (m_iMotion == 0)
		{
			CLevelMgr::GetInst()->GetCurLevel()->ChangeObjectLayer(GetOwner(), 4);
			m_vDir = m_vBossPos - Transform()->GetRelativePos();
			m_vDir.Normalize();
			++m_iMotion;
		}
		else if (m_iMotion == 1)
		{
			++m_iMotion;
			Collider()->CreateColliderActor();
		}

		Vec3 vPos = Transform()->GetRelativePos();
		vPos += m_vDir * 50.f * FDT;
		Transform()->SetRelativePos(vPos);
	}
}

void CBossFireballScript::BeginOverlap(CGameObject* _pOther)
{
	if (_pOther->GetLayerIdx() == 5 && m_eCurrentState == Monster_State::MISS)
	{
		CMonsterScript* pMonsterScr = _pOther->GetScript<CMonsterScript>();
		if (pMonsterScr == nullptr)
			return;

		assert(pMonsterScr); // not monster
		// Set Damage to Monster Here
		pMonsterScr->Damage(2, Vec3(10000.f));
		Dead();
	}
	else if(_pOther->GetLayerIdx() == 9 || _pOther->GetLayerIdx() == 2)
	{
		CLinkHitScript* pHitScr = _pOther->GetScript<CLinkHitScript>();
		if (pHitScr == nullptr)
			return;

		CLinkAnimScript* pLinkScr = pHitScr->GetLink()->GetScript<CLinkAnimScript>();

		if (pHitScr->IsThisType(LINK_HITSCR_TYPE::JUST))
		{
			pLinkScr->SetGuardJustSuccess(true, GetOwner());
		}
		else
		{
			if (pLinkScr->IsGuardAnim())
			{
				pLinkScr->SetGuardSuccess(true);
				Dead();
			}
			else
			{
				tLinkDamaged damage;
				damage.iDamage = 2;
				damage.eType = LINK_DAMAGED_TYPE::SMALL;
				pLinkScr->SetDamage(damage);
				Dead();
			}
		}
	}
}

void CBossFireballScript::Overlap(CGameObject* _pOther)
{
	if (_pOther->GetLayerIdx() == 5 && m_eCurrentState == Monster_State::MISS)
	{
		CMonsterScript* pMonsterScr = _pOther->GetScript<CMonsterScript>();
		if (pMonsterScr == nullptr)
			return;

		assert(pMonsterScr); // not monster
		// Set Damage to Monster Here
		pMonsterScr->Damage(2, Vec3(10000.f));
		Dead();
	}
	else if (_pOther->GetLayerIdx() == 9 || _pOther->GetLayerIdx() == 2)
	{
		CLinkHitScript* pHitScr = _pOther->GetScript<CLinkHitScript>();
		if (pHitScr == nullptr)
			return;

		CLinkAnimScript* pLinkScr = pHitScr->GetLink()->GetScript<CLinkAnimScript>();

		if (pHitScr->IsThisType(LINK_HITSCR_TYPE::JUST))
		{
			pLinkScr->SetGuardJustSuccess(true, GetOwner());
		}
		else
		{
			if (pLinkScr->IsGuardAnim())
			{
				pLinkScr->SetGuardSuccess(true);
				Dead();
			}
			else
			{
				tLinkDamaged damage;
				damage.iDamage = 2;
				damage.eType = LINK_DAMAGED_TYPE::SMALL;
				pLinkScr->SetDamage(damage);
				Dead();
			}
		}
	}
	else
	{
		if(m_eCurrentState != Monster_State::IDLE)
			Dead();
	}
}

void CBossFireballScript::EndOverlap(CGameObject* _pOther)
{
}

void CBossFireballScript::SaveToYAML(YAML::Emitter& _emitter)
{
	CMonsterScript::SaveToYAML(_emitter);
	_emitter << YAML::Key << "Size";
	_emitter << YAML::Value << m_iSize;
}

void CBossFireballScript::LoadFromYAML(YAML::Node& _node)
{
	CMonsterScript::LoadFromYAML(_node);
	SAFE_LOAD_FROM_YAML(int, m_iSize, _node["Size"]);
}

void CBossFireballScript::Fire(Vec3 _vPlayerPos)
{
	m_vPlayerPos = _vPlayerPos;
	m_vDir = _vPlayerPos + Vec3(0.f, 5.f, 0.f) - Transform()->GetWorldPos();
	m_vDir.Normalize();
	MakeParentReserve(GetOwner());
	m_eCurrentState = Monster_State::ATTACK;
}

void CBossFireballScript::Dead()
{
	Destroy();
	Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\boss\\SiteBossLsword_BigFlameBall_Explosion.wav");
	pSound->Play(1, MONSTER_VOLUME, true, nullptr, Transform()->GetWorldPos());
	CGameObject* pflame = CResMgr::GetInst()->Load<CPrefab>(L"prefab\\flame.pref", L"prefab\\flame.pref")->Instantiate();
	Instantiate(pflame, Transform()->GetWorldPos(), 0);
}
