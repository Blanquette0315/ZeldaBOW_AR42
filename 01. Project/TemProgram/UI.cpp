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
	// �θ� UI�� �ڽ� UI�� ���ϰ� �ִٸ� ���� �־�� �Ѵ�.
	Safe_Del_Vec(m_vecChildUI);
}

void UI::ResetFocus()
{
	ImGui::SetWindowFocus(nullptr);
}

void UI::update()
{
	// �θ� UI�� update������ ��ġ�� ���� �ڽĵ��� update�� �����־�� �Ѵ�.
	for (int i = 0; i < m_vecChildUI.size(); ++i)
	{
		m_vecChildUI[i]->update();
	}
}

void UI::render()
{
	if (m_bOpen)
	{
		// �θ� UI
		if (nullptr == m_ParentUI)
		{
			// �ش� UI�� �θ� UI��, Modal�� Open�ȴٸ�
			if (m_bModal)
			{
				// ���� �������� �߾ӿ��� â�� �������� ��.
				// SetNextWindowPos�� ����° ���ڴ� pivot(�ܼ� offset�� �ƴϴ�)�̴�.
				ImVec2 vCenter = ImGui::GetMainViewport()->GetCenter();
				ImGui::SetNextWindowPos(vCenter, ImGuiCond_::ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
				ImGui::SetNextWindowSize(m_vSize, ImGuiCond_::ImGuiCond_Appearing);

				// Modal Popup UI ����
				ImGui::OpenPopup(m_strName.c_str());
				// BeginPopupModal�� ������ ���� �ֱ� ������ �������� ���� ���� EndPopup�� ȣ��Ǿ�� �Ѵ�.
				// �� return���� ���� ���� ���ο� ���� bool���� ��ȯ�Ѵ�.
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

		// �ڽ� UI ���
		else
		{
			// �ڽ� UI�� ��� ImGui���� �������ִ� BeginChild�� ����� ����� �ȴ�.
			// UI�� ����� ����� �־ �ش� �ڽ� UI�� �θ� UI������ ��������� ������ �����ϴ��� �˷��ش�.
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