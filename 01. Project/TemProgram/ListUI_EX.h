#pragma once
#include "UI.h"
class ListUI_EX :
    public UI
{
private:
    vector<string> m_vecItemKey;
    vector<string> m_vecItemName;
    int m_iSelectIdx;

    UI* m_DBCInst;
    FUNC_1 m_DBCFunc;

    UI* m_SelectInst;
    FUNC_0 m_SelectFunc;

    UI* m_ConfirmInst;
    FUNC_1 m_ConfirmFunc;

    UINT    m_ID;
    static UINT ID;

    bool m_bMultiSelect;
    vector<bool> m_vecMultiSelectIdx; // selected == 1

public:
    void AddDynamicDBClicked(UI* _Inst, FUNC_1 _Func)
    {
        m_DBCInst = _Inst;
        m_DBCFunc = _Func;
    }

    void AddDynamicSelected(UI* _Inst, FUNC_0 _Func)
    {
        m_SelectInst = _Inst;
        m_SelectFunc = _Func;
    }

    void AddDynamicConfirm(UI* _Inst, FUNC_1 _Func)
    {
        m_ConfirmInst = _Inst;
        m_ConfirmFunc = _Func;
    }

    int GetSelectIdx() { return m_iSelectIdx; }

    bool IsMultiSelect() { return m_bMultiSelect; }

    void SetMultiSelect(bool _b) { m_bMultiSelect = _b; }

    // bool* GetMultiSelectIdx() { return m_bMultiSelectIdx; }
    vector<bool>& GetMultiSelectIdx() { return m_vecMultiSelectIdx; }

public:
    template<typename T>
    void InitRes(int _Initial);

    template<typename T>
    void SetItem();

    void InitNotRes(vector<string>& _vecItemName, int _Initial);
    virtual void render_update() override;
    virtual void Close() override;

public:
    ListUI_EX();
    ~ListUI_EX();
};

template<typename T>
inline void ListUI_EX::SetItem()
{
    for (size_t i = 0; i < m_vecItemKey.size(); ++i)
    {
        string strKey = m_vecItemKey[i];
        Ptr<T> ptrRes = CResMgr::GetInst()->FindRes<T>(ConvertStrToWstr(m_vecItemKey[i]));
        m_vecItemName.push_back(ConvertWstrToStr(ptrRes.Get()->GetName()));
    }
}

template<typename T>
inline void ListUI_EX::InitRes(int _Initial)
{
    m_vecItemKey.clear();
    m_vecItemName.clear();

    RES_TYPE eType = GetType<T>();
    const map<wstring, Ptr<CRes>>& mapRes = CResMgr::GetInst()->GetResource(eType);

    map<wstring, Ptr<CRes>>::const_iterator iter = mapRes.cbegin();

    for (; iter != mapRes.cend(); ++iter)
    {
        m_vecItemKey.push_back(ConvertWstrToStr(iter->first));
    }

    SetItem<T>();

    m_iSelectIdx = _Initial;
}

