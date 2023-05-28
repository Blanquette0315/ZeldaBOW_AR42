#include "pch.h"
#include "CMonsterScript.h"

#include <Engine\CGameObject.h>

CMonsterScript::CMonsterScript(int _iScriptType)
	: CScript(_iScriptType)
	, m_iHP(10)
	, m_eCurrentState(Monster_State::IDLE)
	, m_fAcctime(0.f)
	, m_fSpeed(1.f)
    , m_iMotion(0)
{
	AddScriptParam(SCRIPT_PARAM::VEC2, "Test Float ", &m_vBurnEmsvCoeff);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Test Float2", &m_fBurnTexIdx);
}

CMonsterScript::~CMonsterScript()
{
}

void CMonsterScript::finaltick()
{
#ifdef _DEBUG
	{
		static bool bTest = false;
		static float fAcctime = 0.f;
		if (fAcctime > 4.5f)
		{
			fAcctime = 0.f;
			bTest = false;
		}

		if (KEY_TAP(KEY::K))
			bTest = true;
		if (bTest)
		{
			fAcctime += FDT;
			for (UINT i = 0; i < MeshRender()->GetMtrlCount(); ++i)
			{
				Ptr<CMaterial> pMaterial = MeshRender()->GetCurMaterial(i);
				int j = 1;
				pMaterial->SetScalarParam(INT_0, &j);
				pMaterial->SetScalarParam(FLOAT_1, &fAcctime);
				float alltime = 4.5f;
				pMaterial->SetScalarParam(FLOAT_2, &alltime);
				pMaterial->SetScalarParam(FLOAT_3, &m_fBurnTexIdx);
				pMaterial->SetScalarParam(VEC2_3, &m_vBurnEmsvCoeff);
			}
		}
	}
#else
#endif
	
	return;
}

void CMonsterScript::SaveToYAML(YAML::Emitter& _emitter)
{
	CScript::SaveToYAML(_emitter);
	_emitter << YAML::Key << "HP";
	_emitter << YAML::Value << m_iHP;
	_emitter << YAML::Key << "Speed";
	_emitter << YAML::Value << m_fSpeed;

	_emitter << YAML::Key << "PaperBurnEmsvCoeff";
	_emitter << YAML::Value << m_fBurnTexIdx;
	_emitter << YAML::Key << "PaperBurnTexIdx";
	_emitter << YAML::Value << m_vBurnEmsvCoeff;
}

void CMonsterScript::LoadFromYAML(YAML::Node& _node)
{
	CScript::LoadFromYAML(_node);
	SAFE_LOAD_FROM_YAML(UINT, m_iHP, _node["HP"]);
	SAFE_LOAD_FROM_YAML(float, m_fSpeed, _node["Speed"]);

	SAFE_LOAD_FROM_YAML(float, m_fBurnTexIdx, _node["PaperBurnEmsvCoeff"]);
	SAFE_LOAD_FROM_YAML(Vec2, m_vBurnEmsvCoeff, _node["PaperBurnTexIdx"]);
}
