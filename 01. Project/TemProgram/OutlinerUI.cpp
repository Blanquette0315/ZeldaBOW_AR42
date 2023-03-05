#include "pch.h"
#include "OutlinerUI.h"

#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>
#include <Engine/CGameObject.h>
#include <Engine/CEventMgr.h>

#include "CImGuiMgr.h"
#include "TreeUI.h"
#include "InspectorUI.h"

OutlinerUI::OutlinerUI()
	: UI("Outliner")
	, m_Tree(nullptr)
{
	m_Tree = new TreeUI("##OutlinerTree");
	AddChild(m_Tree);

	// 트리 설정
	m_Tree->SetDummyRoot(true);

	// Tree에 Delegate 등록
	m_Tree->AddDynamic_Selected(this, (FUNC_1)&OutlinerUI::SetObjectToInspector);
	m_Tree->AddDynamic_DragDrop(this, (FUNC_2)&OutlinerUI::AddChildObject);

	// 레벨 오브젝트 갱신
	ResetLevel();
}

OutlinerUI::~OutlinerUI()
{
}

void OutlinerUI::update()
{
	if (CEventMgr::GetInst()->IsLevelChanged())
	{
		ResetLevel();
	}
}

void OutlinerUI::render_update()
{
}

void OutlinerUI::ResetLevel()
{
	m_Tree->Clear();

	// 현재 레벨을 가져온다.
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();
	string strLevelName = string(pCurLevel->GetName().begin(), pCurLevel->GetName().end());

	// 레벨 이름으로 RootNode 추가
	TreeNode* pRootNode = m_Tree->AddItem(nullptr, strLevelName.c_str(), 0, true);

	// 모든 레이어를 확인하면서, 부모 오브젝트들 기준으로 Tree에 추가
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		const vector<CGameObject*>& vecObject = pCurLevel->GetLayer(i)->GetParentObject();

		for (size_t j = 0; j < vecObject.size(); ++j)
		{
			AddGameObjectToTree(pRootNode, vecObject[j]);
		}
	}
}

void OutlinerUI::SetObjectToInspector(DWORD_PTR _Obj)
{
	// _Obj : 클릭한 노드
	TreeNode* pSelectedNode = (TreeNode*)_Obj;
	CGameObject* pObject = (CGameObject*)pSelectedNode->GetData();

	// InspectorUI에 클릭된 Resource를 알려준다.
	InspectorUI* Inspector = (InspectorUI*)CImGuiMgr::GetInst()->FindUI("Inspector");

	Inspector->SetTargetObject(pObject);
}

void OutlinerUI::AddGameObjectToTree(TreeNode* _ParentNode, CGameObject* _Object)
{
	string strObjectName = string(_Object->GetName().begin(), _Object->GetName().end());
	TreeNode* pCurNode = m_Tree->AddItem(_ParentNode, strObjectName.c_str(), (DWORD_PTR)_Object);

	// 부모 오브젝트만 넣어도 해당 오브젝트의 자식까지 재귀함수를 통해 노드를 추가해준다.
	const vector<CGameObject*>& vecChild = _Object->GetChildObject();
	for (size_t i = 0; i < vecChild.size(); ++i)
	{
		AddGameObjectToTree(pCurNode, vecChild[i]);
	}
}

void OutlinerUI::AddChildObject(DWORD_PTR _ChildObject, DWORD_PTR _ParentObject)
{
	TreeNode* pChildNode = (TreeNode*)_ChildObject;
	TreeNode* pParentNode = (TreeNode*)_ParentObject;

	tEvent evt = {};
	evt.eType = EVENT_TYPE::ADD_CHILD;
	evt.wParam = pChildNode->GetData();
	evt.lParam = pParentNode->GetData();
	CEventMgr::GetInst()->AddEvent(evt);
}