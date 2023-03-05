#include "pch.h"
#include "CRenderComponent.h"


CRenderComponent::CRenderComponent(COMPONENT_TYPE _eType)
	: CComponent(_eType)
{
}

CRenderComponent::CRenderComponent(const CRenderComponent& _origin)
	: CComponent(_origin)
	, m_pMesh(_origin.m_pMesh)
	, m_pSharedMtrl(_origin.m_pSharedMtrl)
	, m_pCurMtrl(nullptr)
	, m_pDynamicMtrl(nullptr)
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

Ptr<CMaterial> CRenderComponent::GetSharedMaterial()
{
	m_pCurMtrl = m_pSharedMtrl;


	if (nullptr != m_pDynamicMtrl)
	{
		m_pDynamicMtrl = nullptr;
	}

	return m_pSharedMtrl;
}

Ptr<CMaterial> CRenderComponent::GetDynamicMaterial()
{
	if (nullptr != m_pDynamicMtrl)
		return m_pDynamicMtrl;

	m_pDynamicMtrl = m_pSharedMtrl->Clone();
	m_pDynamicMtrl->SetName(m_pSharedMtrl->GetName() + L"_Clone");
	m_pCurMtrl = m_pDynamicMtrl;

	return m_pCurMtrl;
}







void CRenderComponent::SaveToFile(FILE* _File)
{
	// 해당 함수가 호출되는 것은 각각 자식쪽이기 때문에 부모인 이곳에서 이름을 저장해도 동일하다.
	COMPONENT_TYPE type = GetType();
	fwrite(&type, sizeof(UINT), 1, _File);

	// 실제로 들고있는 재질은 공유 재질이기 때문에 공유 재질만 저장하면 된다.
	SaveResourceRef<CMesh>(m_pMesh, _File);
	SaveResourceRef<CMaterial>(m_pSharedMtrl, _File);
}

void CRenderComponent::LoadFromFile(FILE* _File)
{
	LoadResourceRef<CMesh>(m_pMesh, _File);
	LoadResourceRef<CMaterial>(m_pSharedMtrl, _File);
	m_pCurMtrl = m_pSharedMtrl;
}
