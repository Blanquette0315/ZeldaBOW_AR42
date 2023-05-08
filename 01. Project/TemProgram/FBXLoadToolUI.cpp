#include "pch.h"
#include "FBXLoadToolUI.h"

#include <Engine/CGameObject.h>

FBXLoadToolUI::FBXLoadToolUI()
	: UI("FBXLoadTool")
	, m_iOption(0)
{
	Close();
}

FBXLoadToolUI::~FBXLoadToolUI()
{
}

void FBXLoadToolUI::update()
{
}

void FBXLoadToolUI::render_update()
{
	ImGui::Text("FBX Name"); ImGui::SameLine(); ImGui::InputText("##FBXName", m_FBXName, 256);
	ImGui::Text("Option  "); ImGui::InputInt("##Option", &m_iOption);
	if (ImGui::Button("Make Prefab With FBX"))
	{
		
		wstring wstrFBXName = ConvertStrToWstr(m_FBXName);
		wstring strRelPath = L"fbx\\" + wstrFBXName;

		wstring sub = wstrFBXName.substr(0, wstrFBXName.find(L"."));
		

		Ptr<CMeshData> pMeshData = nullptr;
		CGameObject* pObj = nullptr;
		pMeshData = CResMgr::GetInst()->LoadFBX(strRelPath, m_iOption);
		pMeshData->Save(pMeshData->GetRelativePath());

		pObj = pMeshData->Instantiate();
		pObj->SetName(sub);
		pObj->ChangePrefab();
		delete pObj;
	}
}


