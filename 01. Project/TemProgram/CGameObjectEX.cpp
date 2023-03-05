#include "pch.h"
#include "CGameObjectEX.h"

#include <Engine\CComponent.h>

CGameObjectEX::CGameObjectEX()
{
}

CGameObjectEX::~CGameObjectEX()
{
}

void CGameObjectEX::finaltick()
{
	// Component
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr != GetComponent((COMPONENT_TYPE)i))
			GetComponent((COMPONENT_TYPE)i)->finaltick();
	}

	// Child Object
	const vector<CGameObject*>& vecChild = GetChildObject();
	for (size_t i = 0; i < vecChild.size(); ++i)
	{
		vecChild[i]->finaltick();
	}
}