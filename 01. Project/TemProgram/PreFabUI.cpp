#include "pch.h"
#include "PreFabUI.h"

#include <Engine/CPrefab.h>
#include <Engine/CEventMgr.h>
#include <Engine/CLevelMgr.h>
#include <Engine/CGameObject.h>


#include "CImGuiMgr.h"
#include "TreeUI.h"
#include "InspectorUI.h"
#include "OutlinerUI.h"

PreFabUI::PreFabUI()
	: ResUI("PreFab##UI", RES_TYPE::PREFAB)
	, m_pTargetObj(nullptr)
{
}

PreFabUI::~PreFabUI()
{
}

void PreFabUI::update()
{
	if (GetTargetObj())
	{
		m_pTargetObj = GetTargetObj();
	}

	ResUI::update();
}

void PreFabUI::render_update()
{
	if (m_pTargetObj == nullptr)
		return;

	InstantiatePrefab(); ImGui::SameLine(); SavePrefab();

	if (nullptr != m_pTargetObj)
	{
		ImGui::PushID(0);
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.f, 0.6f, 0.8f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.f, 0.7f, 0.9f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.f, 0.8f, 1.f));
		ImGui::Button("Select Object");
		ImGui::PopStyleColor(3);
		ImGui::PopID();

		ImGui::Text("Now Object : "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), string(m_pTargetObj->GetName().begin(), m_pTargetObj->GetName().end()).c_str());

		ImGui::SameLine();

		// ������Ʈ �̸� �����ϱ� â����
		if (ImGui::Button("##ChangeObjNameBtn", ImVec2(18.f, 18.f)))
			ImGui::OpenPopup("Change Object Name?");

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal("Change Object Name?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			static char TemName[128] = "";
			ImGui::InputText("Input Name##ObjName", TemName, IM_ARRAYSIZE(TemName));

			ImGui::Separator();

			if (ImGui::Button("OK##ObjName", ImVec2(120, 0)))
			{
				string NewName = TemName;
				wstring wNewName = wstring(NewName.begin(), NewName.end());
				m_pTargetObj->SetName(wNewName);
				ImGui::CloseCurrentPopup();

				// OutLinerUI ���ΰ�ħ
				OutlinerUI* pOutlinerUI = dynamic_cast<OutlinerUI*>(CImGuiMgr::GetInst()->FindUI("Outliner"));
				pOutlinerUI->ResetLevel();
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel##ObjName", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}

		// ������Ʈ�� ���� ���̾� �����ֱ�
		ImGui::Text("Now Layer  : "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), std::to_string(m_pTargetObj->GetLayerIdx()).c_str());

		ImGui::SameLine();
		// Layer ���� â ����
		if (ImGui::Button("##ChangeObjLayerBtn", ImVec2(18.f, 18.f)))
			ImGui::OpenPopup("Change Object Layer?");

		center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal("Change Object Layer?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			static int TemNum = 0;
			ImGui::InputInt("Input Layer Index##ObjLayer", &TemNum);

			ImGui::Separator();

			if (ImGui::Button("OK##ObjLayer", ImVec2(120, 0)))
			{
				int NewNum = TemNum;
				if (0 <= NewNum && NewNum <= 32)
				{
					// ���� ���̾�� �ش� ������Ʈ�� Layer �Ҽ��� �ٲ�޶�� ��û��.
					CLevelMgr::GetInst()->GetCurLevel()->ChangeObjectLayer(m_pTargetObj, NewNum);
				}
				else
				{
					MessageBox(nullptr, L"�ùٸ��� �ʴ� ���̾� �ε��� ��", L"���̾� �ε��� Ȯ��", MB_OK);
				}
				ImGui::CloseCurrentPopup();

				// OutLinerUI ���ΰ�ħ
				OutlinerUI* pOutlinerUI = dynamic_cast<OutlinerUI*>(CImGuiMgr::GetInst()->FindUI("Outliner"));
				pOutlinerUI->ResetLevel();
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel##ObjLayer", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}
	}
}

void PreFabUI::SavePrefab()
{
	// ������ ���� ���
	if (ImGui::Button("Save##PrefabSaveBtn", ImVec2(100.f, 30.f)))
	{
		CGameObject* pParentObj = m_pTargetObj;
		while (pParentObj->GetParent())
		{
			pParentObj = m_pTargetObj->GetParent();
		}
		Ptr<CPrefab> pOwnerPref = pParentObj->GetOwnerPrefab();

		if (L"" == pOwnerPref->GetRelativePath())
		{
			wstring strRelativePath;

			strRelativePath = L"prefab\\";
			strRelativePath += pOwnerPref->GetKey();
			strRelativePath += L".pref";
			pOwnerPref->Save(strRelativePath);
		}
		// ���� ��� �״�θ� �־� ������.
		else
		{
			pOwnerPref->Save(pOwnerPref->GetRelativePath());
		}
	}
}

void PreFabUI::InstantiatePrefab()
{
	if (ImGui::Button("Instantiate", ImVec2(100.f, 30.f)))
	{
		CGameObject* pParentObj = m_pTargetObj;
		while (pParentObj->GetParent())
		{
			pParentObj = m_pTargetObj->GetParent();
		}
		Ptr<CPrefab> pOwnerPref = pParentObj->GetOwnerPrefab();

		CGameObject* pNewObj = pOwnerPref->Instantiate();
		Instantiate(pNewObj, Vec3(0.f, 0.f, 990.f), 0);
	}
}


