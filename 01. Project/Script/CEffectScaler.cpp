#include "pch.h"
#include "CEffectScaler.h"

CEffectScaler::CEffectScaler()
	: CScript((int)SCRIPT_TYPE::EFFECTSCALER)
	, m_fAccTime(0.f)
{
	AddScriptParam(SCRIPT_PARAM::INT, "Option", &m_iOption);
	AddScriptParam(SCRIPT_PARAM::INT, "LerpOption", &m_iLerpOption);

	AddScriptParam(SCRIPT_PARAM::VEC3, "Start-Scale ", &m_vScaleStart);
	AddScriptParam(SCRIPT_PARAM::VEC3, "Middle-Scale ", &m_vScaleMiddle);
	AddScriptParam(SCRIPT_PARAM::VEC3, "End-Scale ", &m_vScaleEnd);

	AddScriptParam(SCRIPT_PARAM::FLOAT, "Middle-Time", &m_fMiddleTime);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Middle-Delay-Time", &m_fMiddleDelayTime);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Max-Time", &m_fMaxTime);
}

CEffectScaler::CEffectScaler(const CEffectScaler& _origin)
	: CScript(_origin)
	, m_vScaleStart(_origin.m_vScaleStart)
	, m_vScaleMiddle(_origin.m_vScaleMiddle)
	, m_vScaleEnd(_origin.m_vScaleEnd)
	, m_fMiddleTime(_origin.m_fMiddleTime)
	, m_fMiddleDelayTime(_origin.m_fMiddleDelayTime)
	, m_fMaxTime(_origin.m_fMaxTime)
	, m_iOption(_origin.m_iOption)
	, m_iLerpOption(_origin.m_iLerpOption)

	, m_fAccTime(0.f)
{
	AddScriptParam(SCRIPT_PARAM::INT, "Option", &m_iOption);
	AddScriptParam(SCRIPT_PARAM::INT, "LerpOption", &m_iLerpOption);

	AddScriptParam(SCRIPT_PARAM::VEC3, "Start-Scale ", &m_vScaleStart);
	AddScriptParam(SCRIPT_PARAM::VEC3, "Middle-Scale ", &m_vScaleMiddle);
	AddScriptParam(SCRIPT_PARAM::VEC3, "End-Scale ", &m_vScaleEnd);

	AddScriptParam(SCRIPT_PARAM::FLOAT, "Middle-Time", &m_fMiddleTime);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Middle-Delay-Time", &m_fMiddleDelayTime);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Max-Time", &m_fMaxTime);
}

CEffectScaler::~CEffectScaler()
{
}

void CEffectScaler::begin()
{
	if(m_iOption != 2)
		Transform()->SetRelativeScale(m_vScaleStart);

	m_bDelay = false;
	if (m_iOption == (int)SCALER_OPT::START_END)
		m_bDelayFlag = false;
	else
		m_bDelayFlag = true;
}

void CEffectScaler::tick()
{
	switch (SCALER_OPT(m_iOption))
	{
	case SCALER_OPT::START_END:
		Start_End();
		break;

	case SCALER_OPT::START_MIDDLE_END:
		Start_Middle_End();
		break;
	case SCALER_OPT::ONLY_TIMER:
		Timer();
		break;
	}
}

void CEffectScaler::SelectLerpType(float& _fRatio)
{
	if (m_iLerpOption == 1)
	{
		_fRatio = LerpCos(_fRatio);
	}
	else if (m_iLerpOption == 2)
	{
		_fRatio = LerpCircle(_fRatio);
	}
}

void CEffectScaler::Start_End()
{
	if (m_fMaxTime <= m_fAccTime)
	{
		if (IsValid(GetOwner()))
		{
			Destroy();
			GetOwner()->GetRenderComponent()->SetRender(false);
		}
	}
	else
	{
		float fRatio = m_fAccTime / m_fMaxTime;
		SelectLerpType(fRatio);
		Transform()->SetRelativeScale(Vec3::Lerp(m_vScaleStart, m_vScaleEnd, fRatio));
		m_fAccTime += FDT;
	}
}

void CEffectScaler::Start_Middle_End()
{
	if (!m_bDelay)
	{
		if (m_fAccTime <= m_fMiddleTime)
		{
			float fRatio = m_fAccTime / m_fMiddleTime;
			SelectLerpType(fRatio);
			Transform()->SetRelativeScale(Vec3::Lerp(m_vScaleStart, m_vScaleMiddle, fRatio));
			m_fAccTime += FDT;
		}
		else if (m_fAccTime <= m_fMaxTime)
		{
			if (m_bDelayFlag)
			{
				m_bDelay = true;
				m_bDelayFlag = false;
				return;
			}

			float fRatio = (m_fAccTime - m_fMiddleTime) / (m_fMaxTime - m_fMiddleTime);
			SelectLerpType(fRatio);
			Transform()->SetRelativeScale(Vec3::Lerp(m_vScaleMiddle, m_vScaleEnd, fRatio));
			m_fAccTime += FDT;
		}
		else
		{
			if (IsValid(GetOwner()))
			{
				GetOwner()->GetRenderComponent()->SetRender(false);
				Destroy();
			}
		}
	}

	if (m_bDelay)
	{
		if (m_fMiddleDelayTime <= m_fDelayAccTIme)
		{
			m_bDelay = false;
			m_fDelayAccTIme = 0.f;
		}
		else
		{
			m_fDelayAccTIme += FDT;
		}
	}
}

void CEffectScaler::Timer()
{
	if (m_fMaxTime <= m_fAccTime)
	{
		if (IsValid(GetOwner()))
		{
			Destroy();
			if(GetOwner()->GetRenderComponent())
				GetOwner()->GetRenderComponent()->SetRender(false);
		}
	}
	else
	{
		m_fAccTime += FDT;
	}
}

void CEffectScaler::SaveToYAML(YAML::Emitter& _emitter)
{
	CScript::SaveToYAML(_emitter);

	_emitter << YAML::Key << "Option";
	_emitter << YAML::Value << m_iOption;

	_emitter << YAML::Key << "LerpOption";
	_emitter << YAML::Value << m_iLerpOption;

	_emitter << YAML::Key << "ScaleStart";
	_emitter << YAML::Value << m_vScaleStart;

	_emitter << YAML::Key << "ScaleMiddle";
	_emitter << YAML::Value << m_vScaleMiddle;

	_emitter << YAML::Key << "ScaleEnd";
	_emitter << YAML::Value << m_vScaleEnd;

	_emitter << YAML::Key << "Middle Time";
	_emitter << YAML::Value << m_fMiddleTime;

	_emitter << YAML::Key << "Middle Delay";
	_emitter << YAML::Value << m_fMiddleDelayTime;

	_emitter << YAML::Key << "MaxTime";
	_emitter << YAML::Value << m_fMaxTime;
}

void CEffectScaler::LoadFromYAML(YAML::Node& _node)
{
	CScript::LoadFromYAML(_node);


	SAFE_LOAD_FROM_YAML(int, m_iOption, _node["Option"]);
	SAFE_LOAD_FROM_YAML(int, m_iLerpOption, _node["LerpOption"]);
	SAFE_LOAD_FROM_YAML(Vec3, m_vScaleStart, _node["ScaleStart"]);
	SAFE_LOAD_FROM_YAML(Vec3, m_vScaleMiddle, _node["ScaleMiddle"]);
	SAFE_LOAD_FROM_YAML(Vec3, m_vScaleEnd, _node["ScaleEnd"]);

	SAFE_LOAD_FROM_YAML(float, m_fMiddleTime, _node["Middle Time"]);
	SAFE_LOAD_FROM_YAML(float, m_fMiddleDelayTime, _node["Middle Delay"]);
	SAFE_LOAD_FROM_YAML(float, m_fMaxTime, _node["MaxTime"]);
}




