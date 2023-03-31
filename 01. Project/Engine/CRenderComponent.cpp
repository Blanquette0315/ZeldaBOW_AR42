#include "pch.h"
#include "CRenderComponent.h"

#include "CTransform.h"

CRenderComponent::CRenderComponent(COMPONENT_TYPE _eType)
	: CComponent(_eType)
	, m_bIsDynamicMtrl(false)
	, m_bDynamicShadow(true)
{
}

CRenderComponent::CRenderComponent(const CRenderComponent& _origin)
	: CComponent(_origin)
	, m_pMesh(_origin.m_pMesh)
	, m_pSharedMtrl(_origin.m_pSharedMtrl)
	, m_pCurMtrl(nullptr)
	, m_pDynamicMtrl(nullptr)
	, m_bIsDynamicMtrl(_origin.m_bIsDynamicMtrl)
	, m_bDynamicShadow(_origin.m_bDynamicShadow)
{
	if (_origin.m_pCurMtrl == _origin.m_pSharedMtrl)
	{
		m_pCurMtrl = m_pSharedMtrl;
	}
	else if (_origin.m_pCurMtrl == _origin.m_pDynamicMtrl)
	{
		GetDynamicMaterial();
	}
}

CRenderComponent::~CRenderComponent()
{
	m_pDynamicMtrl = nullptr;
}

void CRenderComponent::render_depthmap()
{
	Transform()->UpdateData();

	Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"DepthMapMtrl");

	pMtrl->UpdateData();

	m_pMesh->render();
}

Ptr<CMaterial> CRenderComponent::GetSharedMaterial()
{
	m_bIsDynamicMtrl = false;
	m_pCurMtrl = m_pSharedMtrl;

	//if (nullptr != m_pDynamicMtrl)
	//{
	//	m_pDynamicMtrl = nullptr;
	//}


	return m_pSharedMtrl;
}

Ptr<CMaterial> CRenderComponent::GetDynamicMaterial()
{
	m_bIsDynamicMtrl = true;
	
	if (nullptr != m_pDynamicMtrl)
	{
		m_pCurMtrl = m_pDynamicMtrl;
		return m_pDynamicMtrl;
	}

	m_pDynamicMtrl = m_pSharedMtrl->Clone();
	m_pDynamicMtrl->SetName(m_pSharedMtrl->GetName());
	m_pCurMtrl = m_pDynamicMtrl;

	return m_pCurMtrl;
}

void CRenderComponent::SaveToYAML(YAML::Emitter& _emitter)
{
	// 실제로 들고있는 재질은 공유 재질이기 때문에 공유 재질만 저장하면 된다.
	_emitter << YAML::Key << "RenderComponentMesh";
	_emitter << YAML::Value << YAML::BeginMap;
	SaveResourceRef<CMesh>(m_pMesh, _emitter);
	_emitter << YAML::EndMap;
	_emitter << YAML::Key << "RenderComponentMaterial";
	_emitter << YAML::Value << YAML::BeginMap;
	SaveResourceRef<CMaterial>(m_pSharedMtrl, _emitter);
	_emitter << YAML::EndMap;
	_emitter << YAML::Key << "RenderComponent_IsDynamicShadow";
	_emitter << YAML::Value << m_bDynamicShadow;
}

void CRenderComponent::LoadFromYAML(YAML::Node& _node)
{
	YAML::Node node = _node["RenderComponentMesh"];
	LoadResourceRef<CMesh>(m_pMesh, node);
	node = _node["RenderComponentMaterial"];
	LoadResourceRef<CMaterial>(m_pSharedMtrl, node);
	m_pCurMtrl = m_pSharedMtrl;
	m_bDynamicShadow = _node["RenderComponent_IsDynamicShadow"].as<bool>();
}
