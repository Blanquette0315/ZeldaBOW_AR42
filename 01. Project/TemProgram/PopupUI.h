#pragma once
#include "UI.h"

struct tFUNC
{
    string      strName;     // Selectable �̸�

    HEAD_TYPE   eHeadType;  // ������ Ÿ��

    MENU_TYPE   eMenuType;  // Menu Ÿ��
    FUNC_TYPE   eFuncType;  // Func Ÿ��

    FUNC_0      Func_0;      // Func_0 �ּ�
    FUNC_1      Func_1;      // Func_1 �ּ�
    FUNC_2      Func_2;      // Func_2 �ּ�

    DWORD_PTR  lFactor;    // Func_1, Func_2���� ����� ����1 �ּ�
    DWORD_PTR  rFactor;    // Func_2���� ����� ����2 �ּ�
};

class PopupUI :
    public UI
{
private:
    const string                m_strName;
    vector<tFUNC>               m_vecSelectable;

public:
    void AddSelectable(const string& _strName, tFUNC _Func);
    void Clear();

public:
    // OutLinerNode�� ��,
    void Func_NewObj();
    void Func_DeleteObj(DWORD_PTR _lFactor);
    void Func_ChangePrefab(DWORD_PTR _lFactor);
    void Func_NewLevel(DWORD_PTR _lFactor);
    void Func_NewMaterial();

    // ��� ��尡 LevelNode�϶�,
    void Func_NewLevel();

    // Menu �����Լ�
    // Add Component Menu�� ������ִ� �Լ�
    void Create_AddCompoMenu(DWORD_PTR _pTargetObj);
        // �� �޴� �������� ������, ������Ʈ�� �߰� �� �������ִ� �Լ�
    void AddReleaseComponent(COMPONENT_TYPE _type, bool _Select, CGameObject* _Target);

    // Add Script Menu�� ������ִ� �Լ�
    void Create_AddScriptMenu();

public:
    virtual void render_update() override;

public:
    PopupUI(const string& _strName);
    ~PopupUI();
};