#include "pch.h"
#include "CComponent.h"

CComponent::CComponent(COMPONENT_TYPE _eType)
	: m_pOwnerObject(nullptr)
	, m_eType(_eType)
	, m_bActive(true)
{
}

CComponent::CComponent(const CComponent& _origin)
	: CEntity(_origin)
	, m_pOwnerObject(nullptr)
	, m_eType(_origin.m_eType)
	, m_bActive(_origin.m_bActive)
{
}

CComponent::~CComponent()
{
}
