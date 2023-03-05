#pragma once
#include "UI.h"

class ListUI :
    public UI
{
private:
    vector<string>  m_ItemList;
    int             m_iSelectedIdx;

    UI*             m_DBCInst;
    FUNC_1          m_DBCFunc;

public:
    void SetItemList(const vector<string>& _inList);  // string 문자열 vector를 가져와 m_ItemList에 넣어준다.
    void SetItemList(const vector<wstring>& _inList); // wstring 문자열을 받아서 함수 내부에서 string으로 변환해 넣어준다.

    void AddDynamicDBClicked(UI* _Inst, FUNC_1 _Func)
    {
        m_DBCInst = _Inst;
        m_DBCFunc = _Func;
    }

public:
    virtual void render_update() override;
    virtual void Close() override;

public:
    ListUI();
    ~ListUI();
};

