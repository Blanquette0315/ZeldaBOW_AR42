#pragma once
#include "ResUI.h"

class TreeUI;
class TreeNode;
class CPrefab;
class CGameObject;

class PreFabUI :
    public ResUI
{
private:
    // Ptr<CPrefab> m_pTargetPref;
    CGameObject* m_pTargetObj;

private:
    void SavePrefab();
    void InstantiatePrefab();


private:
    virtual void update() override;
    virtual void render_update() override;

public:
    PreFabUI();
    ~PreFabUI();
};