#include "pch.h"
#include "CRenderComponent.h"

#include "CTransform.h"
#include "CAnimator3D.h"

CRenderComponent::CRenderComponent(COMPONENT_TYPE _eType)
	: CComponent(_eType)
	, m_bIsDynamicMtrl(false)
	, m_bDynamicShadow(true)
	, m_bIsRender(true)
{
}

CRenderComponent::CRenderComponent(const CRenderComponent& _origin)
	: CComponent(_origin)
	, m_pMesh(_origin.m_pMesh)
	, m_bIsDynamicMtrl(_origin.m_bIsDynamicMtrl)
	, m_bDynamicShadow(_origin.m_bDynamicShadow)
	, m_bIsRender(true)
{
	if (false == _origin.m_vecMtrls.empty())
	{
		m_vecMtrls.resize(_origin.m_vecMtrls.size());
		for (size_t i = 0; i < _origin.m_vecMtrls.size(); ++i)
		{
			SetSharedMaterial(_origin.m_vecMtrls[i].pSharedMtrl, i);
		}
	}
}

CRenderComponent::~CRenderComponent()
{
	for (size_t i = 0; i < m_vecMtrls.size(); ++i)
	{
		m_vecMtrls[i].pDynamicMtrl = nullptr;
	}
}

void CRenderComponent::render_depthmap()
{
	if (m_pMesh == nullptr)
		return;

	Transform()->UpdateData();

	Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"DepthMapMtrl");

	// Animator3D 업데이트
	if (Animator3D())
	{
		Animator3D()->UpdateData();
		pMtrl->SetAnim3D(true);
		pMtrl->SetBoneCount(Animator3D()->GetBoneCount());
	}

	pMtrl->UpdateData();

	UINT iSubsetCount = m_pMesh->GetSubsetCount();
	for (UINT i = 0; i < iSubsetCount; ++i)
	{
		m_pMesh->render(i);
	}

	if (Animator3D())
	{
		Animator3D()->SetFinalMatUpdate(false);
		Animator3D()->ClearData();
		pMtrl->SetAnim3D(false);
		pMtrl->SetBoneCount(0);
	}
}

void CRenderComponent::SetMesh(Ptr<CMesh> _pMesh)
{
	m_pMesh = _pMesh;

	if (!m_vecMtrls.empty())
	{
		m_vecMtrls.clear();
		vector<tMtrlSet> vecMtrls;
		m_vecMtrls.swap(vecMtrls);
	}

	m_vecMtrls.resize(m_pMesh->GetSubsetCount());
}

void CRenderComponent::SetSharedMaterial(Ptr<CMaterial> _pMtrl, UINT _iIdx)
{
	m_vecMtrls[_iIdx].pSharedMtrl = _pMtrl;
	m_vecMtrls[_iIdx].pCurMtrl = _pMtrl;
}

Ptr<CMaterial> CRenderComponent::GetCurMaterial(UINT _iIdx)
{
	if (m_vecMtrls.size() == 0)
	{
		return nullptr;
	}

	if (nullptr == m_vecMtrls[_iIdx].pCurMtrl)
	{
		m_vecMtrls[_iIdx].pCurMtrl = m_vecMtrls[_iIdx].pSharedMtrl;
	}

	return m_vecMtrls[_iIdx].pCurMtrl;
}

Ptr<CMaterial> CRenderComponent::GetSharedMaterial(UINT _iIdx)
{
	m_bIsDynamicMtrl = false;
	m_vecMtrls[_iIdx].pCurMtrl = m_vecMtrls[_iIdx].pSharedMtrl;

	//if (m_vecMtrls[_iIdx].pDynamicMtrl.Get())
	//{
	//	m_vecMtrls[_iIdx].pDynamicMtrl = nullptr;
	//}

	return m_vecMtrls[_iIdx].pSharedMtrl;
}

Ptr<CMaterial> CRenderComponent::GetDynamicMaterial(UINT _iIdx)
{
	m_bIsDynamicMtrl = true;
	
	if (nullptr != m_vecMtrls[_iIdx].pDynamicMtrl)
	{
		m_vecMtrls[_iIdx].pCurMtrl = m_vecMtrls[_iIdx].pDynamicMtrl;
		return m_vecMtrls[_iIdx].pDynamicMtrl;
	}

	m_vecMtrls[_iIdx].pDynamicMtrl = m_vecMtrls[_iIdx].pSharedMtrl->Clone();
	m_vecMtrls[_iIdx].pDynamicMtrl->SetName(m_vecMtrls[_iIdx].pSharedMtrl->GetName() + L"_Clone");
	m_vecMtrls[_iIdx].pCurMtrl = m_vecMtrls[_iIdx].pDynamicMtrl;

	return m_vecMtrls[_iIdx].pCurMtrl;
}

ULONG64 CRenderComponent::GetInstID(UINT _iMtrlIdx)
{
	if (m_pMesh == NULL || m_vecMtrls[_iMtrlIdx].pCurMtrl == NULL)
		return 0;

	uInstID id{ (UINT)m_pMesh->GetID(), (WORD)m_vecMtrls[_iMtrlIdx].pCurMtrl->GetID(), (WORD)_iMtrlIdx };
	return id.llID;
}

void CRenderComponent::SaveToYAML(YAML::Emitter& _emitter)
{
	// 실제로 들고있는 재질은 공유 재질이기 때문에 공유 재질만 저장하면 된다.
	_emitter << YAML::Key << "RenderComponentMesh";
	_emitter << YAML::Value << YAML::BeginMap;
	SaveResourceRef<CMesh>(m_pMesh, _emitter);
	_emitter << YAML::EndMap;

	UINT iMtrlCount = GetMtrlCount();

	_emitter << YAML::Key << "RenderComponentMaterialCount";
	_emitter << YAML::Value << iMtrlCount;

	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		_emitter << YAML::Key << "RenderComponentMaterial" + std::to_string(i);
		_emitter << YAML::Value << YAML::BeginMap;
		SaveResourceRef<CMaterial>(m_vecMtrls[i].pSharedMtrl, _emitter);
		_emitter << YAML::EndMap;
	}

	_emitter << YAML::Key << "RenderComponent_IsDynamicShadow";
	_emitter << YAML::Value << m_bDynamicShadow;
}

void CRenderComponent::LoadFromYAML(YAML::Node& _node)
{
	YAML::Node node = _node["RenderComponentMesh"];
	LoadResourceRef<CMesh>(m_pMesh, node);

	if (true == m_pMesh->IsAnimMesh())
	{
		if(Animator3D())
			Animator3D()->SetBones(m_pMesh->GetBones());
	}

	UINT iMtrlCount;
	SAFE_LOAD_FROM_YAML(UINT, iMtrlCount, _node["RenderComponentMaterialCount"]);
	m_vecMtrls.resize((size_t)iMtrlCount);

	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		node = _node["RenderComponentMaterial" + std::to_string(i)];
		Ptr<CMaterial> pMtrl;
		LoadResourceRef<CMaterial>(pMtrl, node);
		SetSharedMaterial(pMtrl, i);
	}

	m_bDynamicShadow = _node["RenderComponent_IsDynamicShadow"].as<bool>();
}
