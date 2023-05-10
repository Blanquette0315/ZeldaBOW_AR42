#include "pch.h"
#include "CMonsterAIScript.h"

#include <Engine/CNavMgr.h>

#include "CMonsterScript.h"

#define MONSTER GetOwner()->GetParent()->GetScript<CMonsterScript>() 

CMonsterAIScript::CMonsterAIScript()
	: CScript(MONSTERAISCRIPT)
	, m_fAttackRange(50.f)
    , m_fFindOutRange(150.f)
	, m_pPlayer(nullptr)
	, m_bFind(false)
	, m_bActDone(false)
{
}

CMonsterAIScript::~CMonsterAIScript()
{
}

void CMonsterAIScript::begin()
{
	CNavMgr::GetInst()->init(L"mesh\\all_tiles_navmesh.bin");
	m_vInitialPos = Transform()->GetWorldPos();
}

void CMonsterAIScript::tick()
{
	if (!m_bActDone)
		return;

	if (m_pPlayer != nullptr)
	{
		Vec3 MonsterPos = GetOwner()->GetParent()->Transform()->GetRelativePos();
		Vec3 PlayerPos = m_pPlayer->Transform()->GetRelativePos();

		if (!m_bFind && m_fFindOutRange >= Vec3::Distance(MonsterPos, PlayerPos))
		{
			m_bFind = true;
			m_bActDone = false;
			MONSTER->setState(Monster_State::FIND);
		}
		else if (m_bFind)
		{
			if (m_fAttackRange < Vec3::Distance(MonsterPos, PlayerPos))
			{
				MONSTER->setState(Monster_State::RUN);
			}
			else
			{
				m_bActDone = false;
				MONSTER->setState(Monster_State::ATTACK);
			}
		}
	}
	else
	{
		MONSTER->setState(Monster_State::RETURN);
		m_bFind = false;
		m_bActDone = false;
	}
}

Vec3 CMonsterAIScript::FindPlayerDir()
{
	Vec3 MonsterPos = GetOwner()->GetParent()->Transform()->GetRelativePos();
	Vec3 PlayerPos = m_pPlayer->Transform()->GetRelativePos();
	Vec3 vDir;
	if (!CNavMgr::GetInst()->FindNextMoveDir(MonsterPos, PlayerPos, vDir))
	{
		return Vec3::Zero;
	}
	return vDir;
}

Vec3 CMonsterAIScript::FindInitialPosDir()
{
	Vec3 MonsterPos = GetOwner()->GetParent()->Transform()->GetRelativePos();
	Vec3 vDir;
	if (!CNavMgr::GetInst()->FindNextMoveDir(MonsterPos, m_vInitialPos, vDir))
	{
		return Vec3::Zero;
	}
	return vDir;
}

void CMonsterAIScript::BeginOverlap(CGameObject* _pOther)
{
	if (_pOther->GetName() == L"Link")
	{
		m_pPlayer = _pOther;
		m_bActDone = true;
	}
}

void CMonsterAIScript::Overlap(CGameObject* _pOther)
{
}

void CMonsterAIScript::EndOverlap(CGameObject* _pOther)
{
	if (_pOther->GetName() == L"Link")
	{
		m_pPlayer = nullptr;
	}
}

void CMonsterAIScript::SaveToYAML(YAML::Emitter& _emitter)
{
	CScript::SaveToYAML(_emitter);
	_emitter << YAML::Key << "AttackRange";
	_emitter << YAML::Value << m_fAttackRange;
	_emitter << YAML::Key << "FindOutRange";
	_emitter << YAML::Value << m_fFindOutRange;
}

void CMonsterAIScript::LoadFromYAML(YAML::Node& _node)
{
	CScript::LoadFromYAML(_node);
	SAFE_LOAD_FROM_YAML(float, m_fAttackRange, _node["AttackRange"]);
	SAFE_LOAD_FROM_YAML(float, m_fFindOutRange, _node["FindOutRange"]);
}