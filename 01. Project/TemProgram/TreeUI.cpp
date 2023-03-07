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
				// �ش� ��尡 Level����� ���
				if (dynamic_cast<CLevel*>((CEntity*)m_data))
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

		// �ƿ� ���̳� ����� �� ��Ŭ�� (������Ʈ)
		if (ImGui::BeginPopup("##OutLinerNode"))
		{
			// �� ������Ʈ ����
			if (ImGui::Selectable("New Object"))
			{
				CGameObject* pObj = new CGameObject;
				pObj->SetName(L"New Object");

				tEvent CreateObj;
				CreateObj.eType = EVENT_TYPE::CREATE_OBJECT;
				CreateObj.wParam = (DWORD_PTR)pObj;
				CreateObj.lParam = 0;

				CEventMgr::GetInst()->AddEvent(CreateObj);
			}

			ImGui::Separator();

			// ������Ʈ ����
			if (ImGui::Selectable("Delete Object"))
			{
				CGameObject* pObj = ((CGameObject*)m_data);
				pObj->Destroy();

				/*tEvent DelObject;
				DelObject.eType = EVENT_TYPE::DELETE_OBJECT;
				DelObject.wParam = (DWORD_PTR)pObj;

				CEventMgr::GetInst()->AddEvent(DelObject);*/
			}

			ImGui::Separator();

			// ���� ������Ʈ ������ ��ȯ
			if (ImGui::Selectable("Change Prefab"))
			{
				CGameObject* pObj = ((CGameObject*)m_data);

				tEvent ChangePreFab;
				ChangePreFab.eType = EVENT_TYPE::CHANGE_PREFAB;
				ChangePreFab.wParam = m_data;

				CEventMgr::GetInst()->AddEvent(ChangePreFab);
			}

			ImGui::Separator();

			// ������Ʈ�� ������Ʈ �߰�
			if (ImGui::BeginMenu("Add Component"))
			{
				static bool toggles[(UINT)COMPONENT_TYPE::END] = {};
				CGameObject* pTargetObj = (CGameObject*)m_data;
				for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
				{
					string strName = ToString((COMPONENT_TYPE)i);

					CComponent* pComponent = pTargetObj->GetComponent((COMPONENT_TYPE)i);

					if (nullptr != pComponent)
					{
						toggles[i] = true;
					}
					else
					{
						toggles[i] = false;
					}

					// ������ �Ǹ� false true�� Ȯ���ؼ� Add Component�� Delete Component�� �ϸ� �ȴ�.
					if (ImGui::MenuItem(strName.c_str(), "", &toggles[i]))
					{
						// Ÿ�Ժ��� �б�ó���� �˸��� ������Ʈ�� �߰��� �ش�.
						// ������ true�� �ٲ�� AddComponent, false�� �ٲ�� ReleaseComponent
						AddReleaseComponent((COMPONENT_TYPE)i, toggles[i], pTargetObj);
					}
				}
				
				ImGui::EndMenu();
			}

			// ������Ʈ�� Script �߰�
			if (ImGui::BeginMenu("Add Script"))
			{
				vector<wstring> vecScriptName;
				CScriptMgr::GetScriptInfo(vecScriptName);

				for (size_t i = 0; i < vecScriptName.size(); ++i)
				{
					string strScriptName = WStringToString(vecScriptName[i]);
					if (ImGui::MenuItem(strScriptName.c_str()))
					{
						InspectorUI* pInspector = (InspectorUI*)CImGuiMgr::GetInst()->FindUI("Inspector");
						CGameObject* pTargetObject = pInspector->GetTargetObject();
						if (nullptr != pTargetObject)
						{
							pTargetObject->AddComponent(CScriptMgr::GetScript(vecScriptName[i]));
							pInspector->SetTargetObject(pTargetObject);
						}
					}
				}

				ImGui::EndMenu();
			}

			ImGui::EndPopup();
		}

		// ContentUI Tree�̰�, Level ����� �� ��Ŭ��
		if (ImGui::BeginPopup("##LevelNode"))
		{
			// �� ���� ����
			if (ImGui::Selectable("New Level"))
			{
				// ���� �̸��� ��ġ�� ���� ����� ������ �߻��ϱ� ������ �̸��� ������ �� �ִ� â ����
				bOpenLevelName = true;
			}

			ImGui::EndPopup();
		}

		if (bOpenLevelName)
			ImGui::OpenPopup("Create Level?");

		// ���� ���� �� �̸� ���ϱ� �˾� ����
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal("Create Level?", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			bOpenLevelName = false;

			static char TemName[128] = "";
			ImGui::InputText("Input Level Name", TemName, IM_ARRAYSIZE(TemName));

			ImGui::Separator();

			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				string NewName = TemName;
				wstring wNewName = wstring(NewName.begin(), NewName.end());

				CLevel* pLevel = new CLevel;
				pLevel->SetName(wNewName);

				CSaveLoadMgr::GetInst()->SaveLevel(pLevel);
				delete pLevel;

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

		// ContentUI Tree�̰�, Mtrl ����� �� ��Ŭ�� (���ҽ���)
		if (ImGui::BeginPopup("##MtrlNode"))
		{
			// ���ο� Mtrl ����
			if (ImGui::Selectable("New Material"))
			{
				Ptr<CMaterial> pMtrl = new CMaterial;
				wstring strKey = CResMgr::GetInst()->GetNewResName<CMaterial>();
				pMtrl->SetName(strKey);
				CResMgr::GetInst()->AddRes<CMaterial>(strKey, pMtrl.Get());

				ContentUI* pContentUI = (ContentUI*)CImGuiMgr::GetInst()->FindUI("Content");
				pContentUI->ResetContent();
			}

			ImGui::EndPopup();
		}

		// �ڽ� ������� ����� ȣ���� ���ش�.
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
	case COMPONENT_TYPE::LANDSCAPE:
		break;
	}


	// ���� �������� �÷����� ����ΰ� ���� �̺κ���.
	UI* pInspector = CImGuiMgr::GetInst()->FindUI("Inspector");
	dynamic_cast<InspectorUI*>(pInspector)->SetTargetObject(_Target);
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