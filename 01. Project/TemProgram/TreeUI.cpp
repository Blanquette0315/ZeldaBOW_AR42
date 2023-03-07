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

	// 뒤에 식별번호를 붙여서 이름(키) 중복을 방지해준다.
	// UINT니까 4억개 정도는 방지가 될 것.
	char szTag[50] = "";
	sprintf_s(szTag, 50, "##%d", m_iIdx);
	strName += szTag;

	static bool bOpenLevelName = false;

	if (ImGui::TreeNodeEx(strName.c_str(),iFlag))
	{
		// 드래그 체크
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		{
			m_TreeUI->SetBeginDragNode(this);

			// 첫 인자인 키값은 나중에 드랍을 받는쪽에서 체크해 같으면 데이터를 전달해준다.
			// 두번째 인자인 데이터는 해당 노드 자체를 보낼 것이기 때문에 데이터 사이즈는 TreeNode만큼이다.
			ImGui::SetDragDropPayload(m_TreeUI->GetName().c_str(), (void*)this, sizeof(TreeNode));
			ImGui::Text(strName.c_str());

			ImGui::EndDragDropSource();
		}

		// 드랍체크
		if (ImGui::BeginDragDropTarget())
		{
			m_TreeUI->SetDropTargetNode(this);

			ImGui::EndDragDropTarget();
		}

		// 클릭 체크 : Clicked로 하면, 드래그와 문제가 생겨서 Released로 변경하였다.
		if (!m_bFrame && ImGui::IsItemHovered(0) && ImGui::IsMouseReleased(0))
		{
			m_TreeUI->SetSelectedNode(this);
		}

		//// 우클릭 체크
		//if (ImGui::IsItemHovered(0) && KEY_TAP(KEY::RBTN))
		//{
		//	// OutLinerUI의 Node일 경우 부모(프레임)도 팝업을 띄워 주어야 한다.
		//	if (m_TreeUI->GetName() == "##OutlinerTree")
		//	{
		//		ImGui::OpenPopup("##OutLinerNode");
		//	}

		//	// ContentUI의 Node일 경우 부모(프레임)은 팝업을 띄우면 안된다.
		//	else if (m_TreeUI->GetName() == "##ContentTree")
		//	{
		//		if (!m_bFrame)
		//		{
		//			// 해당 노드가 Level노드일 경우
		//			if (dynamic_cast<CLevel*>((CEntity*)m_data))
		//			{
		//				ImGui::OpenPopup("##LevelNode");
		//			}
		//			else
		//			{
		//				CRes* pTargetRes = (CRes*)m_data;
		//				// 해당 노드가 Mtrl노드일 경우
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
			// 해당 노드가 OutLiner(오브젝트)일 경우 
			if(m_TreeUI->GetName() == "##OutlinerTree")
				ImGui::OpenPopup("##OutLinerNode");

			// 해당 노드가 ContentTree 소속노드일 경우
			if (m_TreeUI->GetName() == "##ContentTree")
			{
				// 해당 노드가 Level노드일 경우
				if (dynamic_cast<CLevel*>((CEntity*)m_data))
				{
					ImGui::OpenPopup("##LevelNode");
				}
				else
				{
					CRes* pTargetRes = (CRes*)m_data;
					// 해당 노드가 Mtrl노드일 경우
					if (pTargetRes->GetResType() == RES_TYPE::MATERIAL)
					{
						ImGui::OpenPopup("##MtrlNode");
					}
				}
				
			}

		}

		// OutLinerNode Popup 생성
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

		// LevelNode Popup 생성
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

		// MtrlNode Popup 생성
		{
			static PopupUI* MaterialNode = new PopupUI("##MtrlNode");

			tFUNC tFunc = {};
			tFunc.eFuncType = FUNC_TYPE::FUNC_0;
			tFunc.eHeadType = HEAD_TYPE::SELECTABLE;
			tFunc.Func_0 = (FUNC_0)&PopupUI::Func_NewMaterial;
			MaterialNode->AddSelectable("New Material", tFunc);

			MaterialNode->render_update();
		}

		// Create Level? Modal Popup 열어주는 파트
		if (bOpenLevelName)
		{
			ImGui::OpenPopup("Create Level?");
			bOpenLevelName = false;
		}
			

		// 레벨 생성 전 이름 정하기 팝업 띄우기
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		// InputTextPopup 생성
		static InputTextUI* pPopupTextUI = new InputTextUI("##CreateLevel");
		pPopupTextUI->SetFunc_1((FUNC_1)&InputTextUI::CreateLevel);
		pPopupTextUI->CreateTextPopup("Create Level?", "Input Level Name");

		// 자식 노드들까지 재귀적 호출을 해준다.
		for (size_t i = 0; i < m_vecChildNode.size(); ++i)
		{
			m_vecChildNode[i]->render_update();
		}

		ImGui::TreePop();
	}

}


void TreeNode::AddReleaseComponent(COMPONENT_TYPE _type, bool _Select, CGameObject* _Target)
{
	switch (_type)
	{
	case COMPONENT_TYPE::TRANSFORM:
	{
		if (_Select)
		{
			_Target->AddComponent(new CTransform);
		}
		else
		{
			_Target->ReleaseComponent(_Target->GetComponent(COMPONENT_TYPE::TRANSFORM));
		}
	}
	break;

	case COMPONENT_TYPE::CAMERA:
	{
		if (_Select)
		{
			_Target->AddComponent(new CCamera);
		}
		else
		{
			_Target->ReleaseComponent(_Target->GetComponent(COMPONENT_TYPE::CAMERA));
		}
	}
	break;

	case COMPONENT_TYPE::COLLIDER2D:
	{
		if (_Select)
		{
			_Target->AddComponent(new CCollider2D);
		}
		else
		{
			_Target->ReleaseComponent(_Target->GetComponent(COMPONENT_TYPE::COLLIDER2D));
		}
	}
	break;

	case COMPONENT_TYPE::COLLIDER3D:
		break;

	case COMPONENT_TYPE::ANIMATOR2D:
	{
		if (_Select)
		{
			_Target->AddComponent(new CAnimator2D);
		}
		else
		{
			_Target->ReleaseComponent(_Target->GetComponent(COMPONENT_TYPE::ANIMATOR2D));
		}
	}
	break;

	case COMPONENT_TYPE::ANIMATOR3D:
		break;

	case COMPONENT_TYPE::LIGHT2D:
	{
		if (_Select)
		{
			_Target->AddComponent(new CLight2D);
		}
		else
		{
			_Target->ReleaseComponent(_Target->GetComponent(COMPONENT_TYPE::LIGHT2D));
		}
	}
	break;

	case COMPONENT_TYPE::LIGHT3D:
		break;

	case COMPONENT_TYPE::MESHRENDER:
	{
		if (_Select)
		{
			_Target->AddComponent(new CMeshRender);
		}
		else
		{
			_Target->ReleaseComponent(_Target->GetComponent(COMPONENT_TYPE::MESHRENDER));
		}
	}
	break;

	case COMPONENT_TYPE::TILEMAP:
	{
		if (_Select)
		{
			_Target->AddComponent(new CTileMap);
		}
		else
		{
			_Target->ReleaseComponent(_Target->GetComponent(COMPONENT_TYPE::TILEMAP));
		}
	}
	break;

	case COMPONENT_TYPE::PARTICLESYSTEM:
	{
		if (_Select)
		{
			_Target->AddComponent(new CParticleSystem);
		}
		else
		{
			_Target->ReleaseComponent(_Target->GetComponent(COMPONENT_TYPE::PARTICLESYSTEM));
		}
	}
	break;

	case COMPONENT_TYPE::SKYBOX:
		break;
	case COMPONENT_TYPE::DECAL:
		break;
	case COMPONENT_TYPE::LANDSPACE:
		break;
	}


	// 봐서 위쪽으로 올려야할 기능인것 같다 이부분은.
	UI* pInspector = CImGuiMgr::GetInst()->FindUI("Inspector");
	dynamic_cast<InspectorUI*>(pInspector)->SetTargetObject(_Target);
}


// ============
// =   Tree   =
// ============
// TreeUI의 이름은 생성자에서 넣어주어서 이름 중복을 방지하도록 구현했다. (ImGui에서 이름 중복이 발생하면 안됨)

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

	// 마우스 왼쪽 릴리즈 체크
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

	// RootNode로 지정되는 노드
	if (nullptr == _parent)
	{
		// 해당 노드를 Root노드로 지정하려하는데, 이미 rootnode가 존재한다면 중단.
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
	// 이전에 선택된 노드가 있었다면, 해당 노드의 선택을 풀어준다.
	if (nullptr != m_SelectedNode)
	{
		m_SelectedNode->m_bSelected = false;
	}

	// 들어온 노드를 선택 노드로 지정하고, 해당 노드에게 선택됨을 알려준다.
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

	// 외부 드랍 관련 PayLoad 처리하기
}