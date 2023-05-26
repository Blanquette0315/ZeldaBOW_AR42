#include "pch.h"
#include "CTrailEffect.h"

CTrailEffect::CTrailEffect()
	: CRenderComponent(COMPONENT_TYPE::TRAILEFFECT)
{
}

CTrailEffect::~CTrailEffect()
{
}

void CTrailEffect::tick()
{
}

void CTrailEffect::finaltick()
{
}

void CTrailEffect::render()
{
	UINT iSubsetCount = GetMesh()->GetSubsetCount();

	for (int i = 0; i < iSubsetCount; ++i)
	{
		if (nullptr != GetCurMaterial(i))
		{
			GetCurMaterial(i)->UpdateData();
			GetMesh()->render(i);
		}
	}

	CMaterial::Clear();
}

void CTrailEffect::render(UINT _iSubset)
{
	if (nullptr != GetCurMaterial(_iSubset))
	{
		GetCurMaterial(_iSubset)->UpdateData();
		GetMesh()->render(_iSubset);
	}
}

void CTrailEffect::SaveToYAML(YAML::Emitter& _emitter)
{
	_emitter << YAML::Key << "TRAILEFFECT";
	CRenderComponent::SaveToYAML(_emitter);
	_emitter << YAML::EndMap;
}

void CTrailEffect::LoadFromYAML(YAML::Node& _node)
{
	YAML::Node node = _node["TRAILEFFECT"];
	CRenderComponent::LoadFromYAML(node);
}


