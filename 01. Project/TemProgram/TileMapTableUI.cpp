#include "pch.h"
#include "TileMapTableUI.h"

TileMapTableUI::TileMapTableUI(string _UIName)
	: UI(_UIName.c_str())
{
    Close();
}

TileMapTableUI::~TileMapTableUI()
{
}

void TileMapTableUI::update()
{
}

void TileMapTableUI::render_update()
{
    ImGui::Text("Set Button  :"); ImGui::SameLine();
    if(ImGui::Button("SET##TIMT_SetButton", ImVec2(36.f, 18.f)))
    {
        m_SelectedIdx.clear();

        vector<bool>::iterator iter = m_selected.begin();
        for (int i = 0; i < m_selected.size(); ++i)
        {
            //  참이 들어있다면 선택 되었다는 뜻으로 해당 인덱스를 벡터에 넣어준다.
            if (m_selected[i])
            {
                m_SelectedIdx.push_back(i);
                (m_SBCInst->*m_SBCFunc)((DWORD_PTR)i);
            }
        }
    }

    ImGui::Text("TileMap Table");
    // 타일의 갯수 만큼 행렬 생성
    for (int y = 0; y < m_vIdxCount.y; y++)
        for (int x = 0; x < m_vIdxCount.x; x++)
        {
            if (x > 0)
                ImGui::SameLine();

            ImGui::PushID(y * m_vIdxCount.x + x);
            string _Name = "";
            char szIdx[50] = "";
            sprintf_s(szIdx, 50, "[%d][%d]", y, x);
            _Name += szIdx;
            _Name += "##TIMT_Select";

            ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f,0.5f));
            if (ImGui::Selectable(_Name.c_str(), m_selected[y * m_vIdxCount.x + x], 0, ImVec2(50, 50)))
            {
                // 눌렸으면, 해당 인덱스 타일을 지금 설정해둔 타일로 변경해주기
                if (m_selected[y * m_vIdxCount.x + x])
                {
                    m_selected[y * m_vIdxCount.x + x] = false;
                }
                else
                {
                    m_selected[y * m_vIdxCount.x + x] = true;
                }

            }
            
            ImGui::PopStyleVar();

            ImGui::PopID();
        }
}


void TileMapTableUI::SetIdxCount(Vec2 _IdxCount)
{
    m_vIdxCount = _IdxCount;

    m_selected.clear();
    m_selected.resize(_IdxCount.x * _IdxCount.y);

    int i = 0;
}