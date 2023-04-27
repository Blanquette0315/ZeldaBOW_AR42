#include "pch.h"
#include "LevelUI.h"

#include <Engine/CLevelMgr.h>
#include <Engine/CEventMgr.h>

#include "CImGuiMgr.h"
#include "LayerCollisionUI.h"
#include "CSaveLoadMgr.h"


LevelUI::LevelUI(const string& _UIName)
	: UI(_UIName)
    , m_TargetLevel(nullptr)
    , m_bUpdateOnce(false)
    , m_strLevelName{}
    , m_strLevelPath{}
    , m_bIsCurLevel(false)
{
}

LevelUI::~LevelUI()
{
}

void LevelUI::update()
{
    if (m_TargetLevelPath != nullptr && !m_bUpdateOnce)
    {
        // Set "Print Level Name"
        wstring LevelName = *m_TargetLevelPath;
        size_t LevelNameIdx = LevelName.find(L"\\");
        LevelName.erase(0, LevelNameIdx + 1);
        m_strLevelName = string(LevelName.begin(), LevelName.end());

        // Set "Print Level Path"
        m_strLevelPath = string(m_TargetLevelPath->begin(), m_TargetLevelPath->end());

        m_bUpdateOnce = true;

        if (nullptr != m_TargetLevelPath && CLevelMgr::GetInst()->GetCurLevel()->GetRelativePath() == *m_TargetLevelPath)
        {
            m_TargetLevel = CLevelMgr::GetInst()->GetCurLevel();
            LevelNameIdx = LevelName.find(L".lv");
            LevelName.erase(LevelNameIdx, LevelName.length());
            if (CLevelMgr::GetInst()->GetCurLevel()->GetName() != LevelName)
            {
                m_bIsCurLevel = false;
            }
            else
            {
                m_bIsCurLevel = true;
            }
        }
        else
            m_bIsCurLevel = false;
       
    }
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
   
    ImGui::Text("Level Name         "); ImGui::SameLine();
    ImGui::InputText("##LevelKey", (char*)m_strLevelName.data(), m_strLevelName.length(), ImGuiInputTextFlags_ReadOnly);

    ImGui::Text("Level Relative Path"); ImGui::SameLine();
    ImGui::InputText("##LevelPath", (char*)m_strLevelPath.data(), m_strLevelPath.length(), ImGuiInputTextFlags_ReadOnly);

    if (ImGui::Button("Set Cur Level", ImVec2(100.f, 30.f)))
    {
        // Level Load & Change Level
        // When a level is changed, this level object is deleted.
        m_TargetLevel = CSaveLoadMgr::GetInst()->LoadLevel(*m_TargetLevelPath);
        ChangeLevel(m_TargetLevel);
        m_bIsCurLevel = true;
    }

    if (m_bIsCurLevel)
    {
        // when m_TargetLevel is nullptr then assert
        assert(m_TargetLevel);

        // Create Layer Collision Setting Popup Open Button
        ImGui::Text("Layer Collision Setting"); ImGui::SameLine();
        if (ImGui::Button("##LayerCollsionSettingBtn", ImVec2(18.f, 18.f)))
        {
            LayerCollisionUI* pLayerCollisionUI = dynamic_cast<LayerCollisionUI*>(CImGuiMgr::GetInst()->FindUI("LayerCollisionUI"));
            assert(pLayerCollisionUI);

            pLayerCollisionUI->SetTargetLevel(m_TargetLevel);
            pLayerCollisionUI->ReSet();

            pLayerCollisionUI->Open();
        }

        // Layer Name Setting Part
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
}