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
	//m_List = new ListUI_EX;
	//m_List->AddDynamicSelected(this, )

	UI::Close();
}

ExportToolUI::~ExportToolUI()
{
}

void ExportToolUI::Open()
{
	UI::Open();
	//vector<CGameObject*> vecGameObj = CLevelMgr::GetInst()->GetCurLevel()->GetGameObjects();
	//vector<string> vecName;
	//vecName.reserve(vecGameObj.size());
	//for (size_t i = 0; i < vecGameObj.size(); ++i)
	//{
	//	vecName.push_back(ConvertWstrToStr(vecGameObj[i]->GetName()));
	//}
	//m_List->SetMultiSelect(true);
	//m_List->InitNotRes(vecName ,0);
}

void ExportToolUI::update()
{

}

void ExportToolUI::render_update()
{
	if(ImGui::Button("Export All"))
	{
		vector<CGameObject*> vecGameObj = CLevelMgr::GetInst()->GetCurLevel()->GetGameObjects();

		CExportMgr::GetInst()->AssignVector(vecGameObj);
		CExportMgr::GetInst()->SaveAsObj();
	}

	if (ImGui::Button("Test"))
	{

		vector<CGameObject*> vecGameObj = CLevelMgr::GetInst()->GetCurLevel()->GetGameObjects();

		CExportMgr::GetInst()->AssignVector(vecGameObj);
		CExportMgr::GetInst()->SaveAsObj();
		wstring path = CPathMgr::GetInst()->GetContentPath();
		path += L"mesh\\all_tiles_navmesh.bin";
		dtNavMesh* testMesh = CExportMgr::GetInst()->LoadNavMesh(path);

		const float startPos[3] = { 0.0f, -500.0f, 400.0f };
		const float endPos[3] = { 30.0f, -500.0f, 375.0f  };

		dtNavMeshQuery* navQuery = dtAllocNavMeshQuery();
		navQuery->init(testMesh, 2048);
		dtPolyRef p1 = {};
		dtPolyRef p2 = {};
		int pathCount;
		dtPolyRef arrPath[200] = {};
		const int MaxPath = 1000;
		dtQueryFilter queryFilter = {};
		float extents[3] = { 10.0f, 0.0f, 10.0f };
		float nearest1[3], nearest2[3];
		dtStatus status = navQuery->findNearestPoly(startPos, extents, &queryFilter, &p1, nearest1);
		if (status != DT_SUCCESS) {
			// no path... we also assume that if we didn't find a path that's less
			// than 200, we error
			return;
		}
		status = navQuery->findNearestPoly(endPos, extents, &queryFilter, &p1, nearest1);
		if (status != DT_SUCCESS) {
			// no path... we also assume that if we didn't find a path that's less
			// than 200, we error
			return;
		}
		status = navQuery->findPath(p1, p2, startPos, endPos, &queryFilter, arrPath, &pathCount, MaxPath);


	}
}


