#include "pch.h"
#include "InputTextUI.h"

#include <Engine/CLevel.h>
#include "CImGuiMgr.h"
#include "CSaveLoadMgr.h"
#include "ContentUI.h"

InputTextUI::InputTextUI(const string& _strName)
	: UI(_strName.c_str())
	, m_eFuncType(FUNC_TYPE::FUNC_0)
	, m_pFunc_0(nullptr)
	, m_pFunc_1(nullptr)
	, m_pFunc_2(nullptr)
	, m_pFuncOwner(nullptr)
	, m_Factor1{}
{
  CImGuiMgr::GetInst()->RegisterUI(this);
  UI::Close();
}

InputTextUI::~InputTextUI()
{
}

void InputTextUI::render_update()
{
}

void InputTextUI::CreateTextPopup(const string& _strPopupName, const string& _strInputTextExplan)
{
	if (ImGui::BeginPopupModal(_strPopupName.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static char TemName[128] = "";
		ImGui::InputText(_strInputTextExplan.c_str(), TemName, IM_ARRAYSIZE(TemName));

		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			switch (m_eFuncType)
			{
			case FUNC_TYPE::FUNC_0:
			{
				if (m_pFuncOwner == nullptr)
					(this->*m_pFunc_0)();
				else
					(m_pFuncOwner->*m_pFunc_0)();
			}
				break;

			case FUNC_TYPE::FUNC_1:
			{
				if(m_pFuncOwner == nullptr)
					(this->*m_pFunc_1)((DWORD_PTR)TemName);
				else
					(m_pFuncOwner->*m_pFunc_1)((DWORD_PTR)TemName);
			}
				break;

			case FUNC_TYPE::FUNC_2: 
			{
				if (m_pFuncOwner == nullptr)
					(this->*m_pFunc_2)((DWORD_PTR)TemName, (DWORD_PTR)m_Factor1);
				else
					(m_pFuncOwner->*m_pFunc_2)((DWORD_PTR)TemName, (DWORD_PTR)m_Factor1);
			}
				break;
			}

			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void InputTextUI::CreateLevel(DWORD_PTR _strLvName)
{
	string NewName = (char*)_strLvName;

	// SameFile Check in LevelFolder
	wstring RelativePath = L"level\\";
	RelativePath += wstring(NewName.begin(), NewName.end());
	RelativePath += L".lv";

	ContentUI* pContentUI = (ContentUI*)CImGuiMgr::GetInst()->FindUI("Content");
	if (!pContentUI->Is_ValidLvPath(RelativePath))
	{
		MessageBox(nullptr, L"동일한 이름의 레벨 파일이 이미 존재한다.", L"레벨 생성 실패!!", MB_OK);
		return;
	}

	wstring wNewName = wstring(NewName.begin(), NewName.end());

	CLevel* pLevel = new CLevel;
	pLevel->SetName(wNewName);

	CSaveLoadMgr::GetInst()->SaveLevel(pLevel);
	delete pLevel;
}