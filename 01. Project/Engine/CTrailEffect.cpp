#include "pch.h"
#include "CTrailEffect.h"

#include "CTransform.h"
#include "CStructuredBuffer.h"
#include "CTrailUpdateShader.h"

CTrailEffect::CTrailEffect()
	: CRenderComponent(COMPONENT_TYPE::TRAILEFFECT)
	, m_bTrail(true)
{
	SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"TrailMesh"));
	if (m_vecMtrls.empty())
		m_vecMtrls.resize(1);
	SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"material\\Link_Effect\\Link_Effect_Trail.mtrl"));

	m_VecPosBuffer = new CStructuredBuffer;
	m_VecPosBuffer->Create(sizeof(tTwoDot), 120, SB_TYPE::UAV_INC, nullptr, true);

	m_OutputBuffer = new CStructuredBuffer;
	m_OutputBuffer->Create(sizeof(UINT), 120, SB_TYPE::UAV_INC, nullptr, true);

	m_pUpdateShader = (CTrailUpdateShader*)CResMgr::GetInst()->FindRes<CComputeShader>(L"TrailUpdateShader").Get();
}

CTrailEffect::CTrailEffect(const CTrailEffect& _origin)
	: CRenderComponent(_origin)
	, m_pUpdateShader(_origin.m_pUpdateShader)
	, m_bTrail(true)
{
	m_VecPosBuffer = new CStructuredBuffer;
	m_VecPosBuffer->Create(sizeof(tTwoDot), 120, SB_TYPE::UAV_INC, nullptr, true);

	m_OutputBuffer = new CStructuredBuffer;
	m_OutputBuffer->Create(sizeof(UINT), 120, SB_TYPE::UAV_INC, nullptr, true);
}

CTrailEffect::~CTrailEffect()
{
	delete m_VecPosBuffer;
	delete m_OutputBuffer;
}

void CTrailEffect::PushData(tTwoDot _twoDot)
{
	// size max = 120
	if (m_vecPosData.size() < 120)
	{
		m_vecPosData.push_back(_twoDot);
	}
	//else
	//{
	//	m_vecPosData.clear();
	//}
}

void CTrailEffect::SetMesh(Ptr<CMesh> _pMesh)
{
	m_pMesh = _pMesh;
}

void CTrailEffect::tick()
{

}

void CTrailEffect::finaltick()
{
}

void CTrailEffect::render()
{
	if (m_vecPosData.size() < 1)
		return;

	m_VecPosBuffer->SetData(m_vecPosData.data(), m_vecPosData.size());
	m_VecPosBuffer->UpdateData(16, ALL_STAGE);

	Transform()->UpdateData();

	GetCurMaterial()->UpdateData();

	UINT IdxCount = (m_vecPosData.size() - 1) * 6;

	UINT StartIdx = IdxCount * m_fRatio;
	StartIdx = (StartIdx / 3) * 3;
	GetMesh()->render_trail(IdxCount, StartIdx);

	m_VecPosBuffer->Clear();
	CMaterial::Clear();
}

void CTrailEffect::EditIdxBuffer()
{
	m_pUpdateShader->SetVecPosBuffer(m_VecPosBuffer);
	m_pUpdateShader->SetOutputBuffer(m_OutputBuffer);

	m_VecPosBuffer->SetData(m_vecPosData.data(), m_vecPosData.size());

	m_pUpdateShader->SetSize(m_vecPosData.size());

	m_pUpdateShader->UpdateData();
	m_pUpdateShader->Execute();
}

void CTrailEffect::render(UINT _iSubset)
{
	if (!m_bTrail)
		return;

	EditIdxBuffer();

	Transform()->UpdateData();

	m_OutputBuffer->UpdateData(16, ALL_STAGE);
	int size = m_vecPosData.size();
	GetCurMaterial()->SetScalarParam(INT_0, &size);

	if (nullptr != GetCurMaterial(_iSubset))
	{
		GetCurMaterial(_iSubset)->UpdateData();
		GetMesh()->render(_iSubset);
	}
}

void CTrailEffect::SaveToYAML(YAML::Emitter& _emitter)
{
	_emitter << YAML::Key << "TRAILEFFECT";
	_emitter << YAML::Value << YAML::BeginMap;
	CRenderComponent::SaveToYAML(_emitter);
	_emitter << YAML::EndMap;
}

void CTrailEffect::LoadFromYAML(YAML::Node& _node)
{
	YAML::Node node = _node["TRAILEFFECT"];
	CRenderComponent::LoadFromYAML(node);
}


