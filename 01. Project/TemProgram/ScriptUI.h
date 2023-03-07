#pragma once
#include "ComponentUI.h"

class CScript;

class ScriptUI :
    public ComponentUI
{
private:
    static UINT g_ScriptUIID;

private:
    CScript* m_pTargetScript;
    const UINT m_ScriptUIID;

    Ptr<CTexture>* m_pParamTex;
    Ptr<CPrefab>* m_pParamPrefab;

public:
    void SetTargetScript(CScript* _pScript)
    {
        m_pTargetScript = _pScript;
    }

    CScript* GetTargetScript()
    {
        return m_pTargetScript;
    }

public:
    void SetTexture(DWORD_PTR _strTexKey);
    void SetPrefab(DWORD_PTR _strTexKey);

public:
    virtual void render_update() override;

public:
    void render_scriptparam();

public:
    ScriptUI();
    ~ScriptUI();
};

