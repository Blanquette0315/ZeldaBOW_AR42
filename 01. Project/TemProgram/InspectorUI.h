#pragma once
#include "UI.h"

class CGameObject;
class ComponentUI;
class ScriptUI;

#include <Engine/ptr.h>
#include <Engine/CRes.h>
class ResUI;

#include <Engine/CLevel.h>
class LevelUI;

class InspectorUI :
    public UI
{
private:
    CGameObject*            m_TargetObj;
    ComponentUI*            m_arrComUI[(UINT)COMPONENT_TYPE::END];

    vector<ScriptUI*>       m_vecScriptUI;

    Ptr<CRes>               m_TargetRes;
    ResUI*                  m_arrResUI[(UINT)RES_TYPE::END];

    CGameObject*            m_TargetPrefObj;

    vector<CGameObject*>    m_arrObject;
    vector<string>          m_ObjName;
    int                     m_iCurItem;
    bool                    m_bMouseCheck;

    const wstring*          m_TargetLevelPath;
    LevelUI*                m_pLevelUI;

    bool                    m_bFrustumCul;

public:
    CGameObject* GetTargetObject() { return m_TargetObj; }

    void SetTargetObject(CGameObject* _Target);
    void SetTargetResource(CRes* _Resource);
    void SetTargetbyKey(DWORD_PTR _strKey);
    void SetTargetLevel(const wstring* _LevelPath);
    // void SetTargetPrefObject(CGameObject* _Target);

    UI* FindResUI(RES_TYPE _eType);
    UI* FindComponentUI(COMPONENT_TYPE _eType);

    //prefab
private:
    void SavePrefab(CGameObject* _pParentObj);
    void InstantiatePrefab(CGameObject* _pParentObj);


private:
    virtual void update() override;
    virtual void render_update() override;

public:
    InspectorUI();
    ~InspectorUI();
};

