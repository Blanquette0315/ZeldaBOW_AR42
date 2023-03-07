#pragma once
#include "UI.h"

struct tFUNC
{
    string      strName;     // Selectable 이름

    HEAD_TYPE   eHeadType;  // 헤드라인 타입

    MENU_TYPE   eMenuType;  // Menu 타입
    FUNC_TYPE   eFuncType;  // Func 타입

    FUNC_0      Func_0;      // Func_0 주소
    FUNC_1      Func_1;      // Func_1 주소
    FUNC_2      Func_2;      // Func_2 주소

    DWORD_PTR  lFactor;    // Func_1, Func_2에서 사용할 인자1 주소
    DWORD_PTR  rFactor;    // Func_2에서 사용할 인자2 주소
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
    // OutLinerNode일 떄,
    void Func_NewObj();
    void Func_DeleteObj(DWORD_PTR _lFactor);
    void Func_ChangePrefab(DWORD_PTR _lFactor);
    void Func_NewLevel(DWORD_PTR _lFactor);
    void Func_NewMaterial();

    // 대상 노드가 LevelNode일때,
    void Func_NewLevel();

    // Menu 생성함수
    // Add Component Menu를 만들어주는 함수
    void Create_AddCompoMenu(DWORD_PTR _pTargetObj);
        // ㄴ 메뉴 아이템이 눌릴시, 컴포넌트를 추가 및 해제해주는 함수
    void AddReleaseComponent(COMPONENT_TYPE _type, bool _Select, CGameObject* _Target);

    // Add Script Menu를 만들어주는 함수
    void Create_AddScriptMenu();

public:
    virtual void render_update() override;

public:
    PopupUI(const string& _strName);
    ~PopupUI();
};