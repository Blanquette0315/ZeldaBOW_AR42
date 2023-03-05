#include "pch.h"
#include "ScriptUI.h"

#include <Engine/CScript.h>
#include <Script/CScriptMgr.h>

#include "ParamUI.h"

#include "CImGuiMgr.h"

UINT ScriptUI::g_ScriptUIID = 0;

ScriptUI::ScriptUI()
	: ComponentUI("##ScriptUI", COMPONENT_TYPE::SCRIPT)
    , m_ScriptUIID(g_ScriptUIID++)
	, m_pTargetScript(nullptr)
{
}

ScriptUI::~ScriptUI()
{
}


void ScriptUI::render_update()
{
    ImGui::PushID(0);
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.f, 0.8f, 0.8f));

    wstring strScriptName = CScriptMgr::GetScriptName(m_pTargetScript);
    ImGui::Button(string(strScriptName.begin(), strScriptName.end()).c_str());

    ImGui::PopStyleColor(3);
    ImGui::PopID();

    render_scriptparam();
}

void ScriptUI::render_scriptparam()
{
    // 아이디가 겹치면 안되는 문제가 발생.
    string ID = "Delete Script##";
    ID += std::to_string(m_ScriptUIID);
    if (ImGui::Button(ID.c_str(), ImVec2(100.f, 30.f)))
    {
        // Target Obj 찾아오기
        CGameObject* TargetObj = GetTarget();

        // 스크립트 제거
        TargetObj->ReleaseComponent(m_pTargetScript);
        m_pTargetScript = nullptr;
        Close();
        return;
    }

    const vector<tScriptParam>& vecParam = m_pTargetScript->GetScriptParam();

    for (size_t i = 0; i < vecParam.size(); ++i)
    {
        switch (vecParam[i].Type)
        {
        case SCRIPT_PARAM::INT:
            ParamUI::Param_Int(vecParam[i].strParamName, (int*)vecParam[i].pParam);
            break;
        case SCRIPT_PARAM::FLOAT:
            ParamUI::Param_Float(vecParam[i].strParamName, (float*)vecParam[i].pParam);
            break;
        case SCRIPT_PARAM::VEC2:
            break;
        case SCRIPT_PARAM::VEC3:
            break;
        case SCRIPT_PARAM::VEC4:
            break;
        case SCRIPT_PARAM::TEXTURE:
            break;
        case SCRIPT_PARAM::MATERIAL:
            break;
        case SCRIPT_PARAM::PREFAB:
            break;
        case SCRIPT_PARAM::SOUND:
            break;
        default:
            break;
        }
    }

}
