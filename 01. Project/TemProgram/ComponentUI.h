#pragma once
#include "UI.h"

class ComponentUI :
	public UI
{
private:
	CGameObject* m_TargetObj;
	COMPONENT_TYPE m_eType;

public:
	void SetTarget(CGameObject* _Target) { m_TargetObj = _Target; }
	CGameObject* GetTarget() { return m_TargetObj; }
	COMPONENT_TYPE GetComponentType() { return m_eType; }

	virtual void render_update() override;

public:
	ComponentUI(const string& _UIName, COMPONENT_TYPE _eType);
	~ComponentUI();
};