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
    // ���̾� ���� ���� �����´�.
    if (m_bChanged)
    {
        memset(m_vecSelected, false, sizeof(m_vecSelected));
        vector<tLayerCollision> vecCollsion = m_TargetLevel->GetLayerCollsionSet();
        for (size_t i = 0; i < vecCollsion.size(); ++i)
        {
            m_vecSelected[vecCollsion[i].iLeft][vecCollsion[i].iRight] = true;
        }

        //for (int i = 0; i < MAX_LAYER; ++i)
        //{
        //    m_matrix[i] = CCollisionMgr::GetInst()->GetLayerMat(i);
        //}
        //
        //memset(m_vecSelected, false, sizeof(m_vecSelected));
        //
        //// ������ ��Ʈ������ �˸°� bool 2�� ��Ŀ� �־��ֱ�
        //for (int i = 0; i < MAX_LAYER; ++i)
        //{
        //    for (int j = 0; j < MAX_LAYER; ++j)
        //    {
        //        // ���� �ش� ���̾ üũ�Ǿ� �ִٸ�
        //        if (m_matrix[i] & (1 << j))
        //        {
        //            // 2�� ��Ŀ��� üũ�� ���־�� �Ѵ�.
        //            m_vecSelected[i][j] = true;
        //        }
        //    }
        //}

        m_bChanged = false;
    }
}

void LayerCollisionUI::render_update()
{
    // ������ �浹 ���̾�� �����ϴ� ��ư
    ImGui::Text("Set Button  :"); ImGui::SameLine();
    if (ImGui::Button("SET##LCS_SetButton", ImVec2(36.f, 18.f)))
    {
       // ���⿡ �浹���̾� �������� ���� �κ� �־��ָ� �ȴ�.

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
        m_TargetLevel->RefreshCollsionSet();


        //// �浹 ���̾� ���� �ʱ�ȭ
        //CCollisionMgr::GetInst()->clear();

        //// ���õ� ������ �˸°� LayerCheck
        //for (int i = 0; i < MAX_LAYER; ++i)
        //{
        //    for (int j = 0; j < MAX_LAYER; ++j)
        //    {
        //        if (m_vecSelected[i][j])
        //        {
        //            CCollisionMgr::GetInst()->CollisionLayerCheck(i, j);
        //        }
        //    }
        //}

        // ���� update�� ������ �� �� �ֵ��� true�� �������ش�.
        m_bChanged = true;
    }





    // Layer �浹 ������ ������ ���̺�
    ImGui::Text("Layer Collsion Table");
    // Ÿ���� ���� ��ŭ ��� ����
    for (int i = 0; i < MAX_LAYER; ++i)
    {
        ImVec2 cursor = ImGui::GetCursorPos();
        // Layer �̸� �־��ֱ�
        string LayerName = "(";
        LayerName += std::to_string(i);
        LayerName += ") ";

        LayerName += string(m_TargetLevel->GetLayer(i)->GetName().begin(), m_TargetLevel->GetLayer(i)->GetName().end());

        ImGui::Text(LayerName.c_str());

        
        cursor.x += 180.f;
        int LayerCount = MAX_LAYER - i;
        for (int j = 0; j < LayerCount; ++j)
        {
            ImGui::SameLine();

            // Layer ���� ������ ��ư �����
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