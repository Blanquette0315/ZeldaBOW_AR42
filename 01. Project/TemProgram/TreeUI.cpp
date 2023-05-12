#include "pch.h"
#include "TreeUI.h"

#include <Engine/CKeyMgr.h>
#include <Engine/CGameObject.h>
#include <Engine/CEventMgr.h>

#include <Engine/CComponent.h>
#include <Engine/GlobalComponent.h>

#include <Script/CScriptMgr.h>

#include "CImGuiMgr.h"
#include "InspectorUI.h"

#include "CSaveLoadMgr.h"
#include "ContentUI.h"

#include "PopupUI.h"
#include "InputTextUI.h"

// ============
// = TreeNode =
// ============

TreeNode::TreeNode()
	: m_ParentNode(nullptr)
	, m_data(0)
	, m_TreeUI(nullptr)
	, m_bFrame(false)
	, m_bSelected(false)
	, m_iIdx(0)
	, m_bIsDataPrefab(false)
{
}

TreeNode::~TreeNode()
{
	Safe_Del_Vec(m_vecChildNode);
}

void TreeNode::render_update()
{
	int iFlag = 0;
	if (m_bFrame)
		iFlag |= ImGuiTreeNodeFlags_Framed;
	if (m_bSelected)
		iFlag |= ImGuiTreeNodeFlags_Selected;
	if (m_vecChildNode.empty())
		iFlag |= ImGuiTreeNodeFlags_Leaf;

	string strName = m_strName;
	if (m_bFrame && m_vecChildNode.empty())
		strName = "\t" + strName;

	char szTag[50] = "";
	sprintf_s(szTag, 50, "##%d", m_iIdx);
	strName += szTag;

	static bool bOpenLevelName = false;

	if (ImGui::TreeNodeEx(strName.c_str(),iFlag))
	{
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		{
			m_TreeUI->SetBeginDragNode(this);

			ImGui::SetDragDropPayload(m_TreeUI->GetName().c_str(), (void*)this, sizeof(TreeNode));
			ImGui::Text(strName.c_str());

			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			m_TreeUI->SetDropTargetNode(this);

			ImGui::EndDragDropTarget();
		}

		if (!m_bFrame && ImGui::IsItemHovered(0) && ImGui::IsMouseReleased(0))
		{
			m_TreeUI->SetSelectedNode(this);
		}
		
		if (!m_bFrame && ImGui::IsItemHovered(0) && KEY_TAP(KEY::RBTN))
		{
			if(m_TreeUI->GetName() == "##OutlinerTree")
				ImGui::OpenPopup("##OutLinerNode");

			if (m_TreeUI->GetName() == "##ContentTree")
			{
				// if m_data is LevelRelativePath
				if (m_bIsDataLevel && IS_LevelRelativePath(m_data))
				{
					ImGui::OpenPopup("##LevelNode");
				}
				else if (m_bIsDataPrefab)
				{
					ImGui::OpenPopup("##OutLinerNode");
				}
				else
				{
					CRes* pTargetRes = (CRes*)m_data;
					if (pTargetRes->GetResType() == RES_TYPE::MATERIAL)
					{
						ImGui::OpenPopup("##MtrlNode");
					}
				}
				
			}

		}

		// OutLinerNode Popup ����
		{
			static PopupUI* OutLiner = new PopupUI("##OutLinerNode");

			tFUNC tFunc = {};
			tFunc.eFuncType = FUNC_TYPE::FUNC_0;
			tFunc.eHeadType = HEAD_TYPE::SELECTABLE;
			tFunc.Func_0 = (FUNC_0)&PopupUI::Func_NewObj;
			OutLiner->AddSelectable("New Object", tFunc);

			tFunc = {};
			tFunc.eFuncType = FUNC_TYPE::FUNC_1;
			tFunc.eHeadType = HEAD_TYPE::SELECTABLE;
			tFunc.Func_1 = (FUNC_1)&PopupUI::Func_DeleteObj;
			tFunc.lFactor = (DWORD_PTR)m_data;
			OutLiner->AddSelectable("Delete Object", tFunc);

			tFunc = {};
			tFunc.eFuncType = FUNC_TYPE::FUNC_1;
			tFunc.eHeadType = HEAD_TYPE::SELECTABLE;
			tFunc.Func_1 = (FUNC_1)&PopupUI::Func_ChangePrefab;
			tFunc.lFactor = (DWORD_PTR)m_data;
			OutLiner->AddSelectable("Change Prefab", tFunc);

			tFunc = {};
			tFunc.eHeadType = HEAD_TYPE::MENU;
			tFunc.eMenuType = MENU_TYPE::ADD_COMPONENT;
			tFunc.lFactor = (DWORD_PTR)m_data;
			OutLiner->AddSelectable("Add Component", tFunc);

			tFunc = {};
			tFunc.eHeadType = HEAD_TYPE::MENU;
			tFunc.eMenuType = MENU_TYPE::ADD_SCRIPT;
			OutLiner->AddSelectable("Add Script", tFunc);

			OutLiner->render_update();
		}

		// Create LevelNode Popup
		{
			static PopupUI* LevelNode = new PopupUI("##LevelNode");

			tFUNC tFunc = {};
			tFunc.eFuncType = FUNC_TYPE::FUNC_1;
			tFunc.eHeadType = HEAD_TYPE::SELECTABLE;
			tFunc.Func_1 = (FUNC_1)&PopupUI::Func_NewLevel;
			tFunc.lFactor = (DWORD_PTR)&bOpenLevelName;
			LevelNode->AddSelectable("New Level", tFunc);

			LevelNode->render_update();
		}

		// MtrlNode Popup
		{
			static PopupUI* MaterialNode = new PopupUI("##MtrlNode");

			tFUNC tFunc = {};
			tFunc.eFuncType = FUNC_TYPE::FUNC_0;
			tFunc.eHeadType = HEAD_TYPE::SELECTABLE;
			tFunc.Func_0 = (FUNC_0)&PopupUI::Func_NewMaterial;
			MaterialNode->AddSelectable("New Material", tFunc);

			MaterialNode->render_update();
		}

		// Create "Create Level?" Modal Popup
		if (bOpenLevelName)
		{
			ImGui::OpenPopup("Create Level?");
			bOpenLevelName = false;
		}
			

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		// Create InputTextPopup
		static InputTextUI* pPopupTextUI = new InputTextUI("##CreateLevel");
		pPopupTextUI->SetFunc_1((FUNC_1)&InputTextUI::CreateLevel);
		pPopupTextUI->CreateTextPopup("Create Level?", "Input Level Name");

		for (size_t i = 0; i < m_vecChildNode.size(); ++i)
		{
			m_vecChildNode[i]->render_update();
		}

		ImGui::TreePop();
	}

}


// ============
// =   Tree   =
// ============

UINT TreeUI::m_iNextNodeIdx = 0;

TreeUI::TreeUI(const string& _strName)
	: UI(_strName)
	, m_RootNode(nullptr)
	, m_bDummyRootUse(false)
	, m_SelectedNode(nullptr)
	, m_BeginDragNode(nullptr)
	, m_DropTargetNode(nullptr)
	, m_SelectInst(nullptr)
	, m_SelectFunc(nullptr)
	, m_DragDropInst(nullptr)
	, m_DragDropFunc(nullptr)
{
}

TreeUI::~TreeUI()
{
	Clear();
}

void TreeUI::update()
{
}

void TreeUI::render_update()
{
	if (nullptr == m_RootNode)
		return;

	if (!m_bDummyRootUse)
	{
		m_RootNode->render_update();
	}
	else
	{
		const vector<TreeNode*>& vecChildNode = m_RootNode->GetChild();
		for (size_t i = 0; i < vecChildNode.size(); ++i)
		{
			vecChildNode[i]->render_update();
		}
	}

	if (ImGui::IsMouseReleased(0))
	{
		m_BeginDragNode = nullptr;
		m_DropTargetNode = nullptr;
	}
}

TreeNode* TreeUI::AddItem(TreeNode* _parent, const string& _strName, DWORD_PTR _data, bool _IsFrame, bool _LevelData)
{
	TreeNode* pNode = new TreeNode;
	pNode->SetNodeName(_strName);
	pNode->SetData(_data);
	pNode->SetLevelData(_LevelData);
	pNode->SetFrame(_IsFrame);
	pNode->m_TreeUI = this;
	pNode->m_iIdx = m_iNextNodeIdx++;

	if (nullptr == _parent)
	{
		assert(!m_RootNode);
		m_RootNode = pNode;
	}
	else
	{
		_parent->AddChild(pNode);
	}

	return pNode;
}

void TreeUI::Clear()
{
	if (nullptr != m_RootNode)
	{
		delete m_RootNode;
		m_RootNode = nullptr;
	}
}

void TreeUI::SetSelectedNode(TreeNode* _SelectedNode)
{
	if (nullptr != m_SelectedNode)
	{
		m_SelectedNode->m_bSelected = false;
	}

	m_SelectedNode = _SelectedNode;
	m_SelectedNode->m_bSelected = true;

	if (m_SelectInst && m_SelectedNode)
	{
		(m_SelectInst->*m_SelectFunc)((DWORD_PTR)m_SelectedNode);
	}
}

void TreeUI::SetDropTargetNode(TreeNode* _DropTargetNode)
{
	m_DropTargetNode = _DropTargetNode;

	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GetName().c_str()))
	{
		if (m_DragDropInst && m_DragDropFunc)
		{
			(m_DragDropInst->*m_DragDropFunc)((DWORD_PTR)m_BeginDragNode, (DWORD_PTR)m_DropTargetNode);
		}
	}

}