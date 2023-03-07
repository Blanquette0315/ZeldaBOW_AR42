
#include "pch.h"
#include "ListUI_EX.h"

UINT ListUI_EX::ID = 0;

ListUI_EX::ListUI_EX()
	: UI("ListUI_EX" + std::to_string(ID))
	, m_iSelectIdx(-1)
	, m_DBCInst(nullptr)
	, m_DBCFunc(nullptr)
	, m_bMultiSelect(false)
	, m_ID(ID)
{
	m_vecMultiSelectIdx.reserve(MAX_LAYER);
	m_vecMultiSelectIdx.assign(MAX_LAYER, false);
	Close();
	SetSize(ImVec2(200.f, 500.f));
	++ID;
}


ListUI_EX::~ListUI_EX()
{
}

void ListUI_EX::InitNotRes(vector<string>& _vecItemName, int _Initial)
{
	m_iSelectIdx = _Initial;
	m_vecItemKey = _vecItemName;
	m_vecItemName = _vecItemName;
}

void ListUI_EX::render_update()
{
	ImVec2 vRegion = ImGui::GetContentRegionAvail();
	vRegion.x += 18.f;
	vRegion.y -= 50.f;

	string str = "##ListBox" + std::to_string(m_ID);
	if (ImGui::BeginListBox(str.c_str(), vRegion))
	{
		if (m_bMultiSelect)
		{
			for (size_t i = 0; i < m_vecItemName.size(); ++i)
			{
				// List Item ������ �޶����� ��
				if(m_vecItemName.size() != m_vecMultiSelectIdx.size())
				{
					m_vecMultiSelectIdx.resize(m_vecItemName.size(), false);
				}

				static char buf[256] = {};
				sprintf_s(buf, m_vecItemName[i].c_str());

				if (ImGui::Selectable(buf, m_vecMultiSelectIdx[i]))
				{
					if (!ImGui::GetIO().KeyCtrl)    // Clear selection when CTRL is not held
						m_vecMultiSelectIdx.assign(m_vecMultiSelectIdx.size(), 0);
					//memset(m_bMultiSelectIdx., 0, sizeof(m_bMultiSelectIdx));
					m_vecMultiSelectIdx[i] = m_vecMultiSelectIdx[i] ^ 1;
				}

				if (m_SelectInst && m_SelectFunc)
				{
					(m_SelectInst->*m_SelectFunc)();
				}
			}
		}
		else
		{
			for (size_t i = 0; i < m_vecItemName.size(); ++i)
			{
				bool Selectable = (m_iSelectIdx == i);

				// i��° �������� ���ȴٸ�
				if (ImGui::Selectable(m_vecItemName[i].c_str(), Selectable))
				{
					// ���� ������ �ε����� �������ش�. -> �ٽ� ���� ���� true
					m_iSelectIdx = i;
				}

				// �ʱ⿡ ���� �ִ� ������ ����
				if (Selectable)
				{
					ImGui::SetItemDefaultFocus();
				}

				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					if (m_DBCInst && m_DBCFunc)
					{
						(m_DBCInst->*m_DBCFunc)((DWORD_PTR)m_vecItemKey[i].c_str());
					}
				}
			}
		}
		ImGui::EndListBox();

		if (m_bMultiSelect)
		{
			static char buf[256] = {};
			string strButtonName = "confirm##" + std::to_string(m_ID);
			sprintf_s(buf, strButtonName.c_str());
			if (ImGui::Button(buf))
			{
				if (m_ConfirmInst && m_ConfirmFunc)
				{
					(m_ConfirmInst->*m_ConfirmFunc)((DWORD_PTR)&m_vecMultiSelectIdx);
				}
			}
		}
	}
}

void ListUI_EX::Close()
{
}


