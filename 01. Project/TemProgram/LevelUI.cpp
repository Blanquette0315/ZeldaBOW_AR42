#include "pch.h"
#include "LevelUI.h"

#include <Engine/CEventMgr.h>

#include "CImGuiMgr.h"
#include "LayerCollisionUI.h"


LevelUI::LevelUI(const string& _UIName)
	: UI(_UIName)
{
}

LevelUI::~LevelUI()
{
}

void LevelUI::render_update()
{
    ImGui::PushID(0);
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.f, 0.8f, 0.8f));
    ImGui::Button("Level");
    ImGui::PopStyleColor(3);
    ImGui::PopID();
   
    ImGui::Text("Level Name"); ImGui::SameLine();
    string strName = string(m_TargetLevel->GetName().begin(), m_TargetLevel->GetName().end());
    ImGui::InputText("##LevelKey", (char*)strName.data(), strName.length(), ImGuiInputTextFlags_ReadOnly);

    ImGui::Text("Level Path"); ImGui::SameLine();
    string strPath = string(m_TargetLevel->GetRelativePath().begin(), m_TargetLevel->GetRelativePath().end());
    ImGui::InputText("##LevelKey", (char*)strPath.data(), strPath.length(), ImGuiInputTextFlags_ReadOnly);

    if (ImGui::Button("Set Cur Level", ImVec2(100.f, 30.f)))
    {
        ChangeLevel(m_TargetLevel);
    }

    // Layer 충돌 설정 정해주기 버튼
    ImGui::Text("Layer Collision Setting"); ImGui::SameLine();
    if (ImGui::Button("##LayerCollsionSettingBtn", ImVec2(18.f, 18.f)))
    {
        LayerCollisionUI* pLayerCollisionUI = dynamic_cast<LayerCollisionUI*>(CImGuiMgr::GetInst()->FindUI("LayerCollisionUI"));
        assert(pLayerCollisionUI);

        pLayerCollisionUI->SetTargetLevel(m_TargetLevel);
        pLayerCollisionUI->ReSet();

        pLayerCollisionUI->Open();
    }

    // Layer 이름 변경하기
    for (int i = 0; i < MAX_LAYER; ++i)
    {
        ImVec2 CursorPos = ImGui::GetCursorPos();
        string LayerName = {};
        LayerName += "(";
        LayerName += std::to_string(i);
        LayerName += ") ";

        LayerName += string(m_TargetLevel->GetLayer(i)->GetName().begin(), m_TargetLevel->GetLayer(i)->GetName().end());
        ImGui::Text(LayerName.c_str());


        // 오브젝트 이름 변경하기 창띄우기
        // Button, Popup ID
        string BtnID = "##ChangeLayerNameBtn";
        BtnID += std::to_string(i);

        string PopupId = "Change Layer Name?##";
        PopupId += std::to_string(i);

        CursorPos.x += 250;
        ImGui::SetCursorPos(CursorPos);

        if (ImGui::Button(BtnID.c_str(), ImVec2(18.f, 18.f)))
            ImGui::OpenPopup(PopupId.c_str());

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal(PopupId.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            static char TemName[128] = "";
            ImGui::InputText("Input Name##LayerName", TemName, IM_ARRAYSIZE(TemName));

            ImGui::Separator();

            if (ImGui::Button("OK##LayerName", ImVec2(120, 0)))
            {
                string NewName = TemName;
                wstring wNewName = wstring(NewName.begin(), NewName.end());
                m_TargetLevel->GetLayer(i)->SetName(wNewName);
                ImGui::CloseCurrentPopup();
            }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("Cancel##LayerName", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }
    }
}