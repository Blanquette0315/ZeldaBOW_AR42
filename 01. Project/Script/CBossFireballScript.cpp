#include "pch.h"
#include "CBossFireballScript.h"

#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>

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
			vScale += Vec3(1.f * FDT);
		}
		else
		{
			vScale += Vec3(0.5f * FDT);
		}
		Transform()->SetRelativeScale(vScale);
	}
	else if (m_eCurrentState == Monster_State::ATTACK)
	{
		if (m_iMotion == 0)
		{
			Transform()->SetRelativePos(Transform()->GetWorldPos());
			if (m_iSize == 0)
			{
				Transform()->SetRelativeScale(10.f, 10.f, 10.f);
			}
			else
			{
				Transform()->SetRelativeScale(20.f, 20.f, 20.f);
			}
			++m_iMotion;
		}
		Vec3 vPos = Transform()->GetRelativePos();
		vPos += m_vDir * 1.f * FDT;
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

		Vec3 vPos = Transform()->GetRelativePos();
		vPos += m_vDir * 1.f * FDT;
		Transform()->SetRelativePos(vPos);
	}
}

void CBossFireballScript::BeginOverlap(CGameObject* _pOther)
{
}

void CBossFireballScript::Overlap(CGameObject* _pOther)
{
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
	MakeParent(GetOwner());
	m_eCurrentState = Monster_State::ATTACK;
}