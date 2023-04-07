#include "pch.h"
#include "CAnimation3D.h"

CAnimation3D::CAnimation3D()
	: m_fClipUpdateTime(0.f)
	, m_dCurTime(0.)
	, m_iFrameCount(30)
	, m_iFrmlimit(0)
	, m_iFrameIdx(0)
	, m_iNextFrameIdx(0)
	, m_fRatio(0.f)
	, m_bFinish(false)
{
}

CAnimation3D::CAnimation3D(const CAnimation3D& _origin)
	: CEntity(_origin)
	, m_pOwner(_origin.m_pOwner)
	, m_sClip(_origin.m_sClip)
	, m_fClipUpdateTime(0.f)
	, m_dCurTime(0.)
	, m_iFrameCount(_origin.m_iFrameCount)
	, m_iFrmlimit(_origin.m_iFrmlimit)
	, m_iFrameIdx(0)
	, m_iNextFrameIdx(0)
	, m_fRatio(0.f)
	, m_bFinish(false)
{
}

CAnimation3D::~CAnimation3D()
{
}

void CAnimation3D::Create(const wstring& _Key, int _iStartFrame, int _iEndFrame, int _iFrameCount)
{
	SetName(_Key);

	m_iFrameCount = _iFrameCount;
	m_sClip.strAnimName = _Key;
	m_sClip.iStartFrame = _iStartFrame;
	m_sClip.iEndFrame = _iEndFrame;
	m_sClip.iFrameLength = m_sClip.iEndFrame - m_sClip.iStartFrame;
	m_sClip.eMode = FbxTime::EMode::eFrames30;
	m_sClip.dStartTime = 0.;
	m_sClip.dEndTime = (double)m_sClip.iFrameLength / (double)m_iFrameCount;
	m_sClip.dTimeLength = m_sClip.dEndTime - m_sClip.dStartTime;
}

void CAnimation3D::Create(const wstring& _Key, const tMTAnimClip* _pClip, int _iFrameCount)
{
	SetName(_Key);

	m_iFrameCount = _iFrameCount;
	m_sClip = *_pClip;
}

void CAnimation3D::ResetClip()
{
	m_sClip.iFrameLength = m_sClip.iEndFrame - m_sClip.iStartFrame;
	m_sClip.dEndTime = (double)m_sClip.iFrameLength / (double)m_iFrameCount;
	m_sClip.dTimeLength = m_sClip.dEndTime - m_sClip.dStartTime;
}

void CAnimation3D::finaltick()
{
	if (m_bFinish)
		return;

	m_dCurTime = 0.f;
	// 현재 재생중인 Clip 의 시간을 진행한다.
	m_fClipUpdateTime += FDT;

	if (m_fClipUpdateTime >= m_sClip.dTimeLength)
	{
		m_bFinish = true;
		return;
	}

	m_dCurTime = m_sClip.dStartTime + m_fClipUpdateTime;

	// 현재 프레임 인덱스 구하기
	double dFrameIdx = m_dCurTime * (double)m_iFrameCount;
	m_iFrameIdx = (int)(dFrameIdx) + m_sClip.iStartFrame;

	if (m_iFrameIdx >= m_iFrmlimit)
	{
		m_iFrameIdx = m_iFrmlimit - 1;
		m_iNextFrameIdx = m_iFrameIdx;
		dFrameIdx = (double)(m_iFrameIdx - m_sClip.iStartFrame);
	}
	// 다음 프레임 인덱스
	else if (m_iFrameIdx >= m_sClip.iStartFrame + m_sClip.iFrameLength - 1)
		m_iNextFrameIdx = m_iFrameIdx;	// 끝이면 현재 인덱스를 유지
	else
		m_iNextFrameIdx = m_iFrameIdx + 1;

	// 프레임간의 시간에 따른 비율을 구해준다.
	m_fRatio = (float)(dFrameIdx - (double)(m_iFrameIdx - m_sClip.iStartFrame));
}

void CAnimation3D::SaveToYAML(YAML::Emitter& _emitter)
{
	CEntity::SaveToYAML(_emitter);
	_emitter << YAML::Key << "StartFrame";
	_emitter << YAML::Value << m_sClip.iStartFrame;
	_emitter << YAML::Key << "EndFrame";
	_emitter << YAML::Value << m_sClip.iEndFrame;
	_emitter << YAML::Key << "FrameCount";
	_emitter << YAML::Value << m_iFrameCount;
	_emitter << YAML::Key << "Frmlimit";
	_emitter << YAML::Value << m_iFrmlimit;
}

void CAnimation3D::LoadFromYAML(YAML::Node& _node)
{
	CEntity::LoadFromYAML(_node);
	SAFE_LOAD_FROM_YAML(int, m_sClip.iStartFrame, _node["StartFrame"]);
	SAFE_LOAD_FROM_YAML(int, m_sClip.iEndFrame, _node["EndFrame"]);
	SAFE_LOAD_FROM_YAML(int, m_iFrameCount, _node["FrameCount"]);
	SAFE_LOAD_FROM_YAML(int, m_iFrmlimit, _node["Frmlimit"]);
	ResetClip();
	m_sClip.strAnimName = GetName();
	m_sClip.eMode = FbxTime::EMode::eFrames30;
	m_sClip.dStartTime = 0.;
}

