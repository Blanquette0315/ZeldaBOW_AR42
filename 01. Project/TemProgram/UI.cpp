#include "pch.h"
#include "UI.h"

UI::UI(const string& _strName)
	: m_strName(_strName)
	, m_ParentUI(nullptr)
	, m_bOpen(true)
	, m_bModal(false)
	, m_bSeperator(true)
{
}

UI::~UI()
{
	// 부모 UI라서 자식 UI를 지니고 있다면 지워 주어야 한다.
	Safe_Del_Vec(m_vecChildUI);
}

void UI::ResetFocus()
{
	ImGui::SetWindowFocus(nullptr);
}

void UI::update()
{
	// 부모 UI가 update할일을 마치고 나면 자식들의 update를 돌려주어야 한다.
	for (int i = 0; i < m_vecChildUI.size(); ++i)
	{
		m_vecChildUI[i]->update();
	}
}

void UI::render()
{
	if (m_bOpen)
	{
		// 부모 UI
		if (nullptr == m_ParentUI)
		{
			// 해당 UI가 부모 UI고, Modal로 Open된다면
			if (m_bModal)
			{
				// 메인 윈도우의 중앙에서 창이 열리도록 함.
				// SetNextWindowPos의 세번째 인자는 pivot(단순 offset이 아니다)이다.
				ImVec2 vCenter = ImGui::GetMainViewport()->GetCenter();
				ImGui::SetNextWindowPos(vCenter, ImGuiCond_::ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
				ImGui::SetNextWindowSize(m_vSize, ImGuiCond_::ImGuiCond_Appearing);

				// Modal Popup UI 띄우기
				ImGui::OpenPopup(m_strName.c_str());
				// BeginPopupModal은 실패할 수도 있기 때문에 실패하지 않을 때만 EndPopup이 호출되어야 한다.
				// 또 return으로 성공 실패 여부에 따라 bool값을 반환한다.
				if (ImGui::BeginPopupModal(m_strName.c_str(), &m_bOpen))
				{
					render_update();

					for (int i = 0; i < m_vecChildUI.size(); ++i)
					{
						m_vecChildUI[i]->render();

						if (m_vecChildUI[i]->m_bSeperator && i != (m_vecChildUI.size() - 1) && m_vecChildUI[i]->m_bOpen)
							ImGui::Separator();
					}

					ImGui::EndPopup();
				}
			}

			else
			{
				ImGui::Begin(m_strName.c_str(), &m_bOpen);

				render_update();

				for (int i = 0; i < m_vecChildUI.size(); ++i)
				{
					m_vecChildUI[i]->render();

					if (m_vecChildUI[i]->m_bSeperator && i != (m_vecChildUI.size() - 1) && m_vecChildUI[i]->m_bOpen)
						ImGui::Separator();
				}
				ImGui::End();
			}
		}

		// 자식 UI 라면
		else
		{
			// 자식 UI의 경우 ImGui에서 지원해주는 BeginChild를 사용해 만들면 된다.
			// UI의 멤버인 사이즈를 넣어서 해당 자식 UI가 부모 UI내에서 어느정도의 영역을 차지하는지 알려준다.
			ImGui::BeginChild(m_strName.c_str(), m_vSize);
			
			render_update();

			for (int i = 0; i < m_vecChildUI.size(); ++i)
			{
				m_vecChildUI[i]->render();
			}

			ImGui::EndChild();
			//ImGui::Separator();
		}

		if (false == m_bOpen)
		{
			Close();
		}
	}
}

void UI::AddChild(UI* _ChildUI)
{
	_ChildUI->m_ParentUI = this;
	m_vecChildUI.push_back(_ChildUI);
}