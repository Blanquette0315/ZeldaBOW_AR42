#include "pch.h"
#include "ComboBoxUI.h"

#include <Engine/CResMgr.h>

UINT ComboBoxUI::ID = 0;

ComboBoxUI::ComboBoxUI()
    : UI("##ComboBox" + std::to_string(ID))
    , m_iCurItemIdx(0)
    , m_SelectedFunc(nullptr)
    , m_SelectedInst(nullptr)
    , m_SelectedFunc_ReturnInt(nullptr)
    , m_SelectedInst_ReturnInt(nullptr)
    , m_SelectedNotifyFunc(nullptr)
    , m_SelectedNotifyInst(nullptr)
{
    ++ID;
}

ComboBoxUI::~ComboBoxUI()
{
}

void ComboBoxUI::init_not_res(vector<string>& _itemList, int _idx)
{
    m_vecItemKey.clear();
    m_vecItemName.clear();

    m_vecItemKey = _itemList;
    m_vecItemName = _itemList;
    m_iCurItemIdx = _idx;
}

void ComboBoxUI::render_update()
{
    int flags = 0;

    if (!m_vecItemKey.empty())
    {
        if (ImGui::BeginCombo(GetName().c_str(), m_vecItemName[m_iCurItemIdx].c_str(), flags))
        {
            for (int i = 0; i < m_vecItemKey.size(); i++)
            {
                const bool is_selected = (m_iCurItemIdx == i);
                if (ImGui::Selectable(m_vecItemName[i].c_str(), is_selected))
                    m_iCurItemIdx = i;

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();

                if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                {
                    if (m_SelectedFunc && m_SelectedInst)
                    {
                        (m_SelectedInst->*m_SelectedFunc)((DWORD_PTR)m_vecItemKey[i].c_str());
                    }

                    if (m_SelectedFunc_ReturnInt && m_SelectedInst_ReturnInt)
                    {
                        (m_SelectedInst_ReturnInt->*m_SelectedFunc_ReturnInt)((DWORD_PTR)i);
                    }

                    if (m_SelectedNotifyFunc && m_SelectedNotifyInst)
                    {
                        (m_SelectedNotifyInst->*m_SelectedNotifyFunc)();
                    }
                }
            }
            ImGui::EndCombo();
        }
    }
}

void ComboBoxUI::update()
{
}

inline wstring ConvertStrToWstr(const string& _str)
{
    return wstring(_str.begin(), _str.end());
}

inline string ConvertWstrToStr(const wstring& _wstr)
{
    return string(_wstr.begin(), _wstr.end());
}

vector<wstring> ConvertStrToWstrVec(const vector<string>& _vecStr)
{
    vector<wstring> vecWstr;

    for (size_t i = 0; i < _vecStr.size(); ++i)
    {
        vecWstr.push_back(ConvertStrToWstr(_vecStr[i]));
    }
    return vecWstr;
}

vector<string> ConvertWstrToStrVec(const vector<wstring>& _vecWstr)
{
    vector<string> vecStr;

    for (size_t i = 0; i < _vecWstr.size(); ++i)
    {
        vecStr.push_back(ConvertWstrToStr(_vecWstr[i]));
    }
    return vecStr;
}

