#include "pch.h"
#include "PreFabUI.h"

#include <Engine/CPrefab.h>
#include <Engine/CEventMgr.h>


PreFabUI::PreFabUI()
	: ResUI("PreFab##UI", RES_TYPE::PREFAB)
{
}

PreFabUI::~PreFabUI()
{
}

void PreFabUI::update()
{
	ResUI::update();
}

void PreFabUI::render_update()
{
	ResUI::render_update();

	if (ImGui::Button("Instantiate", ImVec2(100.f, 30.f)))
	{
		CPrefab* pTarget = (CPrefab*)GetTarget().Get();
		CGameObject* pNewObj = pTarget->Instantiate();
		Instantiate(pNewObj, Vec3(0.f, 0.f, 990.f), 0);
	}

	// ������ ���� ���
	if (ImGui::Button("Save##PrefabSaveBtn", ImVec2(100.f, 30.f)))
	{
		CPrefab* pTarget = (CPrefab*)GetTarget().Get();

		if (L"" == pTarget->GetRelativePath())
		{
			wstring strRelativePath;

			strRelativePath = L"prefab\\";
			strRelativePath += pTarget->GetKey();
			strRelativePath += L".pref";
			pTarget->Save(strRelativePath);
		}
		// ���� ��� �״�θ� �־� ������.
		else
		{
			pTarget->Save(pTarget->GetRelativePath());
		}
	}
}