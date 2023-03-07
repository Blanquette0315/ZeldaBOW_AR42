#pragma once
#include "UI.h"
class InputTextUI :
    public UI
{
private:
    FUNC_TYPE   m_eFuncType;    // OK��ư Ŭ���� ȣ��Ǿ�� �� �Լ� Ÿ��

    FUNC_0      m_pFunc_0;      // Ok ��ư Ŭ���� ȣ��Ǿ�� �� �Լ� ������
    FUNC_1      m_pFunc_1;      // Ok ��ư Ŭ���� ȣ��Ǿ�� �� �Լ� ������
    FUNC_2      m_pFunc_2;      // Ok ��ư Ŭ���� ȣ��Ǿ�� �� �Լ� ������

    UI*         m_pFuncOwner;   // �Լ� �����ͷ� ������ �Լ��� ���� UI ������
                                // ���� nullptr�� ��� InputTextUI�� ������ �����ͷ� ����

    DWORD_PTR   m_Factor1;      // �Լ����� ���� ���ڸ� �޾Ƶδ� ���

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