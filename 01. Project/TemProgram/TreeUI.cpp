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

	// �ڿ� �ĺ���ȣ�� �ٿ��� �̸�(Ű) �ߺ��� �������ش�.
	// UINT�ϱ� 4�ﰳ ������ ������ �� ��.
	char szTag[50] = "";
	sprintf_s(szTag, 50, "##%d", m_iIdx);
	strName += szTag;

	static bool bOpenLevelName = false;

	if (ImGui::TreeNodeEx(strName.c_str(),iFlag))
	{
		// �巡�� üũ
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		{
			m_TreeUI->SetBeginDragNode(this);

			// ù ������ Ű���� ���߿� ����� �޴��ʿ��� üũ�� ������ �����͸� �������ش�.
			// �ι�° ������ �����ʹ� �ش� ��� ��ü�� ���� ���̱� ������ ������ ������� TreeNode��ŭ�̴�.
			ImGui::SetDragDropPayload(m_TreeUI->GetName().c_str(), (void*)this, sizeof(TreeNode));
			ImGui::Text(strName.c_str());

			ImGui::EndDragDropSource();
		}

		// ���üũ
		if (ImGui::BeginDragDropTarget())
		{
			m_TreeUI->SetDropTargetNode(this);

			ImGui::EndDragDropTarget();
		}

		// Ŭ�� üũ : Clicked�� �ϸ�, �巡�׿� ������ ���ܼ� Released�� �����Ͽ���.
		if (!m_bFrame && ImGui::IsItemHovered(0) && ImGui::IsMouseReleased(0))
		{
			m_TreeUI->SetSelectedNode(this);
		}

		//// ��Ŭ�� üũ
		//if (ImGui::IsItemHovered(0) && KEY_TAP(KEY::RBTN))
		//{
		//	// OutLinerUI�� Node�� ��� �θ�(������)�� �˾��� ��� �־�� �Ѵ�.
		//	if (m_TreeUI->GetName() == "##OutlinerTree")
		//	{
		//		ImGui::OpenPopup("##OutLinerNode");
		//	}

		//	// ContentUI�� Node�� ��� �θ�(������)�� �˾��� ���� �ȵȴ�.
		//	else if (m_TreeUI->GetName() == "##ContentTree")
		//	{
		//		if (!m_bFrame)
		//		{
		//			// �ش� ��尡 Level����� ���
		//			if (dynamic_cast<CLevel*>((CEntity*)m_data))
		//			{
		//				ImGui::OpenPopup("##LevelNode");
		//			}
		//			else
		//			{
		//				CRes* pTargetRes = (CRes*)m_data;
		//				// �ش� ��尡 Mtrl����� ���
		//				if (pTargetRes->GetResType() == RES_TYPE::MATERIAL)
		//				{
		//					ImGui::OpenPopup("##MtrlNode");
		//				}
		//			}
		//		}
		//	}
		//}
		
		if (!m_bFrame && ImGui::IsItemHovered(0) && KEY_TAP(KEY::RBTN))
		{
			// �ش� ��尡 OutLiner(������Ʈ)�� ��� 
			if(m_TreeUI->GetName() == "##OutlinerTree")
				ImGui::OpenPopup("##OutLinerNode");

			// �ش� ��尡 ContentTree �Ҽӳ���� ���
			if (m_TreeUI->GetName() == "##ContentTree")
			{
				// if m_data is LevelRelativePath
				if (IS_LevelRelativePath(m_data))
				{
					ImGui::OpenPopup("##LevelNode");
				}
				else
				{
					CRes* pTargetRes = (CRes*)m_data;
					// �ش� ��尡 Mtrl����� ���
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

		// MtrlNode Popup ����
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
			

		// ���� ���� �� �̸� ���ϱ� �˾� ����
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		// Create InputTextPopup
		static InputTextUI* pPopupTextUI = new InputTextUI("##CreateLevel");
		pPopupTextUI->SetFunc_1((FUNC_1)&InputTextUI::CreateLevel);
		pPopupTextUI->CreateTextPopup("Create Level?", "Input Level Name");

		// �ڽ� ������� ����� ȣ���� ���ش�.
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
// TreeUI�� �̸��� �����ڿ��� �־��־ �̸� �ߺ��� �����ϵ��� �����ߴ�. (ImGui���� �̸� �ߺ��� �߻��ϸ� �ȵ�)

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

	// ���콺 ���� ������ üũ
	if (ImGui::IsMouseReleased(0))
	{
		m_BeginDragNode = nullptr;
		m_DropTargetNode = nullptr;
	}
}

TreeNode* TreeUI::AddItem(TreeNode* _parent, const string& _strName, DWORD_PTR _data, bool _IsFrame)
{
	TreeNode* pNode = new TreeNode;
	pNode->SetNodeName(_strName);
	pNode->SetData(_data);
	pNode->SetFrame(_IsFrame);
	pNode->m_TreeUI = this;
	pNode->m_iIdx = m_iNextNodeIdx++;

	// RootNode�� �����Ǵ� ���
	if (nullptr == _parent)
	{
		// �ش� ��带 Root���� �����Ϸ��ϴµ�, �̹� rootnode�� �����Ѵٸ� �ߴ�.
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
	// ������ ���õ� ��尡 �־��ٸ�, �ش� ����� ������ Ǯ���ش�.
	if (nullptr != m_SelectedNode)
	{
		m_SelectedNode->m_bSelected = false;
	}

	// ���� ��带 ���� ���� �����ϰ�, �ش� ��忡�� ���õ��� �˷��ش�.
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

	// �ܺ� ��� ���� PayLoad ó���ϱ�
}