#include "pch.h"
#include "PopupUI.h"

#include <Engine/CGameObject.h>
#include <Engine/CEventMgr.h>

#include <Engine/GlobalComponent.h>

#include <Script/CScriptMgr.h>

#include "CImGuiMgr.h"
#include "InspectorUI.h"
#include "ContentUI.h"
#include "CSaveLoadMgr.h"

PopupUI::PopupUI(const string& _strName)
	: UI(_strName)
	, m_strName(_strName)
{
	CImGuiMgr::GetInst()->RegisterUI(this);
	UI::Close();
}

PopupUI::~PopupUI()
{
}

void PopupUI::render_update()
{
	if (ImGui::BeginPopup(m_strName.c_str()))
	{
		for (size_t i = 0; i < m_vecSelectable.size(); ++i)
		{
			if (m_vecSelectable[i].eHeadType == HEAD_TYPE::SELECTABLE)
			{
				// Selectable 생성
				if (ImGui::Selectable(m_vecSelectable[i].strName.c_str()))
				{
					// 만약 해당 Selectable이 선택되면, 사용할 함수
					switch (m_vecSelectable[i].eFuncType)
					{
					case FUNC_TYPE::FUNC_0:
					{
						(this->*m_vecSelectable[i].Func_0)();
					}
					break;

					case FUNC_TYPE::FUNC_1:
					{
						(this->*m_vecSelectable[i].Func_1)(m_vecSelectable[i].lFactor);
					}
					break;

					case FUNC_TYPE::FUNC_2:
					{
						(this->*m_vecSelectable[i].Func_2)(m_vecSelectable[i].lFactor, m_vecSelectable[i].rFactor);
					}
					break;
					}
				}
			}

			// 헤드라인이 Menu 타입일 경우
			else if (m_vecSelectable[i].eHeadType == HEAD_TYPE::MENU)
			{
				switch (m_vecSelectable[i].eMenuType)
				{
				case MENU_TYPE::ADD_COMPONENT:
				{
					if (ImGui::BeginMenu(m_vecSelectable[i].strName.c_str()))
					{
						Create_AddCompoMenu(m_vecSelectable[i].lFactor);

						ImGui::EndMenu();
					}
				}
					break;

				case MENU_TYPE::ADD_SCRIPT: 
				{
					if (ImGui::BeginMenu(m_vecSelectable[i].strName.c_str()))
					{
						Create_AddScriptMenu();

						ImGui::EndMenu();
					}
				}
					break;
				}
			}

			if (i + 1 != m_vecSelectable.size())
			{
				ImGui::Separator();
			}
				
		}

		ImGui::EndPopup();
	}

	Clear();
}

void PopupUI::AddSelectable(const string& _strName, tFUNC _Func)
{
	_Func.strName = _strName;
	m_vecSelectable.push_back(_Func);
}

void PopupUI::Clear()
{
	m_vecSelectable.clear();
}

void PopupUI::Func_NewObj()
{
	CGameObject* pObj = new CGameObject;
	pObj->SetName(L"New Object");

	tEvent CreateObj;
	CreateObj.eType = EVENT_TYPE::CREATE_OBJECT;
	CreateObj.wParam = (DWORD_PTR)pObj;
	CreateObj.lParam = 0;

	CEventMgr::GetInst()->AddEvent(CreateObj);
}

void PopupUI::Func_DeleteObj(DWORD_PTR _lFactor)
{
	CGameObject* pObj = ((CGameObject*)_lFactor);
	pObj->Destroy();
}

void PopupUI::Func_ChangePrefab(DWORD_PTR _lFactor)
{
	tEvent ChangePreFab;
	ChangePreFab.eType = EVENT_TYPE::CHANGE_PREFAB;
	ChangePreFab.wParam = _lFactor;

	CEventMgr::GetInst()->AddEvent(ChangePreFab);
}

void PopupUI::Func_NewLevel(DWORD_PTR _lFactor)
{
	*(bool*)_lFactor = true;
}

void PopupUI::Func_NewMaterial()
{
	Ptr<CMaterial> pMtrl = new CMaterial;
	wstring strKey = CResMgr::GetInst()->GetNewResName<CMaterial>();
	pMtrl->SetName(strKey);
	CResMgr::GetInst()->AddRes<CMaterial>(strKey, pMtrl.Get());

	ContentUI* pContentUI = (ContentUI*)CImGuiMgr::GetInst()->FindUI("Content");
	pContentUI->ResetContent();
}

void PopupUI::Func_NewLevel()
{
}

void PopupUI::Create_AddCompoMenu(DWORD_PTR _pTargetObj)
{
	static bool toggles[(UINT)COMPONENT_TYPE::END] = {};
	CGameObject* pTargetObj = (CGameObject*)_pTargetObj;
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

		// 선택이 되면 false true를 확인해서 Add Component와 Delete Component를 하면 된다.
		if (ImGui::MenuItem(strName.c_str(), "", &toggles[i]))
		{
			// 타입별로 분기처리해 알맞은 컴포넌트를 추가해 준다.
			// 눌러서 true로 바뀌면 AddComponent, false로 바뀌면 ReleaseComponent
			AddReleaseComponent((COMPONENT_TYPE)i, toggles[i], pTargetObj);
		}
	}
}

void PopupUI::AddReleaseComponent(COMPONENT_TYPE _type, bool _Select, CGameObject* _Target)
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

void PopupUI::Create_AddScriptMenu()
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
				pTargetObject->AddComponent((CComponent*)CScriptMgr::GetScript(vecScriptName[i]));
				pInspector->SetTargetObject(pTargetObject);
			}
		}
	}
}