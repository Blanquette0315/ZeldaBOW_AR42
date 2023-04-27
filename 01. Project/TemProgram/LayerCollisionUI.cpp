#include "pch.h"
#include "LayerCollisionUI.h"

#include <Engine/CCollisionMgr.h>
#include <Engine/CLevel.h>

LayerCollisionUI::LayerCollisionUI(string _UIName)
	: UI(_UIName.c_str())
    , m_vecSelected{}
    , m_bChanged(false)
{
	Close();
}

LayerCollisionUI::~LayerCollisionUI()
{
}

void LayerCollisionUI::update()
{
    // 레이어 설정 값을 가져온다.
    if (m_bChanged)
    {
        memset(m_vecSelected, false, sizeof(m_vecSelected));
        vector<tLayerCollision> vecCollsion = m_TargetLevel->GetLayerCollsionSet();
        for (size_t i = 0; i < vecCollsion.size(); ++i)
        {
            m_vecSelected[vecCollsion[i].iLeft][vecCollsion[i].iRight] = true;
        }

        m_bChanged = false;
    }
}

void LayerCollisionUI::render_update()
{
    // 지정한 충돌 레이어로 설정하는 버튼
    ImGui::Text("Set Button  :"); ImGui::SameLine();
    if (ImGui::Button("SET##LCS_SetButton", ImVec2(36.f, 18.f)))
    {
       // 여기에 충돌레이어 실질적인 설정 부분 넣어주면 된다.

        m_TargetLevel->ClearLayerCollision();
        for (int i = 0; i < MAX_LAYER; ++i)
        {
            for (int j = 0; j < MAX_LAYER; ++j)
            {
                if (m_vecSelected[i][j])
                {
                    tLayerCollision tTemp = {};
                    tTemp.iLeft = i;
                    tTemp.iRight = j;
                    m_TargetLevel->SetLayerCollsionSet(tTemp);
                }
            }
        }

        m_bChanged = true;
    }





    // Layer 충돌 설정이 가능한 테이블
    ImGui::Text("Layer Collsion Table");
    // 타일의 갯수 만큼 행렬 생성
    for (int i = 0; i < MAX_LAYER; ++i)
    {
        ImVec2 cursor = ImGui::GetCursorPos();
        // Layer 이름 넣어주기
        string LayerName = "(";
        LayerName += std::to_string(i);
        LayerName += ") ";

        LayerName += string(m_TargetLevel->GetLayer(i)->GetName().begin(), m_TargetLevel->GetLayer(i)->GetName().end());

        ImGui::Text(LayerName.c_str());

        
        cursor.x += 180.f;
        int LayerCount = i + 1; // MAX_LAYER - i;
        for (int j = 0; j < LayerCount; ++j)
        {
            ImGui::SameLine();

            // Layer 선택 가능한 버튼 만들기
            string ButtonID = "";
            ButtonID += std::to_string(j);
            ButtonID += "##";
            ButtonID += std::to_string(i * 100 + j);
            ImGui::SetCursorPos(cursor);
            cursor.x += 22.f;
            static bool Testbool = false;
            if (ImGui::Selectable(ButtonID.c_str(), &m_vecSelected[i][j], 0, ImVec2(12.f, 12.f)))
            {
                if (m_vecSelected[i][j])
                {
                    m_vecSelected[i][j] = true;
                }
                else
                {
                    m_vecSelected[i][j] = false;
                }
            }
        }
       

    }
}