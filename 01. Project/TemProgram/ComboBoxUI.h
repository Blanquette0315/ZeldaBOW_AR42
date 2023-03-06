#pragma once
#include "UI.h"

wstring ConvertStrToWstr(const string& _str);
string ConvertWstrToStr(const wstring& _wstr);
vector<wstring> ConvertStrToWstrVec(const vector<string>& _vecStr);
vector<string> ConvertWstrToStrVec(const vector<wstring>& _vecWstr);

template <typename T1, typename T2>
int GetMapIdx(const map<T1, T2>& _map, T2 _key);

template<typename T1, typename T2>
inline int GetMapIdx(const map<T1, T2>& _map, T1 _key)
{
    int idx = 0;
    typename map<T1, T2>::const_iterator iter = _map.cbegin();
    for (; iter != _map.cend(); ++iter)
    {
        if (iter->first == _key)
        {
            return idx;
        }
        ++idx;
    }

    return 0;
}


class ComboBoxUI :
    public UI
{
private:
    vector<string> m_vecItemKey;
    vector<string> m_vecItemName;
    int m_iCurItemIdx;

    FUNC_1  m_SelectedFunc;
    UI* m_SelectedInst;

    FUNC_1 m_SelectedFunc_ReturnInt;
    UI* m_SelectedInst_ReturnInt;

    FUNC_0 m_SelectedNotifyFunc;
    UI* m_SelectedNotifyInst;

    UINT m_ID;
    static UINT ID;

public:
    template<typename T>
    void init_res(int _defaultIdx);

    template<typename T>
    void SetItem();

public:
    void SetCurIdx(int _idx) { m_iCurItemIdx = _idx; }

public:
    void SetPreviewIdx(int _PreviewIdx) { m_iCurItemIdx = _PreviewIdx; }

    void AddSelectedFunc(UI* _inst, FUNC_1 _func)
    {
        m_SelectedInst = _inst;
        m_SelectedFunc = _func;
    }

    void AddSelectedFunc_ReturnInt(UI* _inst, FUNC_1 _func)
    {
        m_SelectedInst_ReturnInt = _inst;
        m_SelectedFunc_ReturnInt = _func;
    }

    void AddSelectedNotify(UI* _inst, FUNC_0 _func)
    {
        m_SelectedNotifyFunc = _func;
        m_SelectedNotifyInst = _inst;
    }

public:
    void init_not_res(vector<string>& _itemList, int _idx);
    void render_update() override;
    void update();

public:
    ComboBoxUI();
    ~ComboBoxUI();
};


template<typename T>
inline void ComboBoxUI::SetItem()
{
    for (size_t i = 0; i < m_vecItemKey.size(); ++i)
    {
        string strKey = m_vecItemKey[i];
        Ptr<T> ptrRes = CResMgr::GetInst()->FindRes<T>(ConvertStrToWstr(m_vecItemKey[i]));
        m_vecItemName.push_back(ConvertWstrToStr(ptrRes.Get()->GetName()));
    }
}

template<typename T>
inline void ComboBoxUI::init_res(int _defaultIdx)
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

    m_iCurItemIdx = _defaultIdx;
}


// --------------------------------------------------------------------------------------


