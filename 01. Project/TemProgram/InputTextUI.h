#pragma once
#include "UI.h"
class InputTextUI :
    public UI
{
private:
    FUNC_TYPE   m_eFuncType;    // OK버튼 클릭시 호출되어야 할 함수 타입

    FUNC_0      m_pFunc_0;      // Ok 버튼 클릭시 호출되어야 할 함수 포인터
    FUNC_1      m_pFunc_1;      // Ok 버튼 클릭시 호출되어야 할 함수 포인터
    FUNC_2      m_pFunc_2;      // Ok 버튼 클릭시 호출되어야 할 함수 포인터

    UI*         m_pFuncOwner;   // 함수 포인터로 전달한 함수의 소유 UI 포인터
                                // 만약 nullptr일 경우 InputTextUI가 소유한 포인터로 간주

    DWORD_PTR   m_Factor1;      // 함수에서 사용될 인자를 받아두는 멤버

public:
    void SetFunc_0(FUNC_0 _Func0) { m_pFunc_0 = _Func0; m_eFuncType = FUNC_TYPE::FUNC_0; }
    void SetFunc_1(FUNC_1 _Func1) { m_pFunc_1 = _Func1; m_eFuncType = FUNC_TYPE::FUNC_1; }
    void SetFunc_2(FUNC_2 _Func2, DWORD_PTR _Factor1) { m_pFunc_2 = _Func2; m_Factor1 = _Factor1; m_eFuncType = FUNC_TYPE::FUNC_2; }

    void SetFuncOwner(UI* _pOnwerUI) { m_pFuncOwner = m_pFuncOwner; }

public:
    void CreateTextPopup(const string& _strPopupName, const string& _strInputTextExplan);
    void CreateLevel(DWORD_PTR _strLvName);

public:
    virtual void render_update() override;

public:
    InputTextUI(const string& _strName);
    ~InputTextUI();
};