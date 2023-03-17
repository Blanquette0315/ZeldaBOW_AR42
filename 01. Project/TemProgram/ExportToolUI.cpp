#include "pch.h"
#include "ExportToolUI.h"

#include "ListUI_EX.h"
#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>
#include <Engine/CGameObject.h>
#include <Engine/CExportMgr.h>

#include <Engine/DetourNavMesh.h>
#include <Engine/DetourNavMeshQuery.h>


ExportToolUI::ExportToolUI()
	: UI("ExportTool")
{
	m_List = new ListUI_EX;
	m_List->AddDynamicConfirm(this, (FUNC_1)&ExportToolUI::ExportSelectedObject);

	UI::Close();
}

ExportToolUI::~ExportToolUI()
{
	delete m_List;
}

void ExportToolUI::Open()
{
	UI::Open();
	vector<CGameObject*> vecGameObj = CLevelMgr::GetInst()->GetCurLevel()->GetGameObjects();
	vector<string> vecName;
	vecName.reserve(vecGameObj.size());
	m_List->ClearDataVec();
	for (size_t i = 0; i < vecGameObj.size(); ++i)
	{
		vecName.push_back(ConvertWstrToStr(vecGameObj[i]->GetName()));
		m_List->PushData(vecGameObj[i]);
	}
	m_List->SetMultiSelect(true);
	m_List->InitNotRes(vecName ,0);
}

void ExportToolUI::update()
{

}

void ExportToolUI::render_update()
{
	m_List->render_update();
	/*if(ImGui::Button("Export All"))
	{
		vector<CGameObject*> vecGameObj = CLevelMgr::GetInst()->GetCurLevel()->GetGameObjects();

		CExportMgr::GetInst()->AssignVector(vecGameObj);
		CExportMgr::GetInst()->SaveAsObj();
	}*/
}

void ExportToolUI::ExportSelectedObject(DWORD_PTR _vecSelectedIdx)
{
	vector<bool>* vecSelectedIdx = (vector<bool>*)_vecSelectedIdx;
	const vector<void*>& vecGameObject = m_List->GetData();
	for (size_t i = 0; i < vecSelectedIdx->size(); ++i)
	{
		if (vecSelectedIdx->at(i))
		{
			CExportMgr::GetInst()->RegisterObj((CGameObject*)vecGameObject[i]);
		}
	}
	CExportMgr::GetInst()->SaveAsObj();
}


