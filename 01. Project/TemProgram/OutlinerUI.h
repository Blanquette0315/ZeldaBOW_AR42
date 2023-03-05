#pragma once
#include "UI.h"

class TreeUI;
class TreeNode;
class CGameObject;

class OutlinerUI :
    public UI
{
private:
    TreeUI* m_Tree;

private:
    void SetObjectToInspector(DWORD_PTR _Obj);
    void AddGameObjectToTree(TreeNode* _ParentNode, CGameObject* _Object);

    void AddChildObject(DWORD_PTR _ChildObject, DWORD_PTR _ParentObject);

private:
    virtual void update() override;
    virtual void render_update() override;

public:
    void ResetLevel();

public:
    OutlinerUI();
    ~OutlinerUI();
};

