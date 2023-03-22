#include "pch.h"
#include "InspectorUI.h"

#include <Engine/CLevelMgr.h>
#include <Engine/CGameObject.h>
#include <Engine/CLevel.h>
#include <Engine/CTransform.h>

#include "ListUI.h"
#include "CImGuiMgr.h"
#include "OutlinerUI.h"
#include "LevelUI.h"

#include "TransformUI.h"
#include "MeshRenderUI.h"
#include "Collider2DUI.h"
#include "Light2DUI.h"
#include "Light3DUI.h"
#include "CameraUI.h"
#include "ParticleSystemUI.h"
#include "TileMapUI.h"
#include "Animator2DUI.h"
#include "SkyBoxUI.h"
#include "DecalUI.h"
#include "RigidBodyUI.h"
#include "ScriptUI.h"

#include "MeshUI.h"
#include "RTextureUI.h"
#include "MaterialUI.h"
#include "PreFabUI.h"
#include "GraphicsShaderUI.h"
#include "SoundUI.h"

InspectorUI::InspectorUI()
	: UI("Inspector")
	, m_TargetObj(nullptr)
	, m_arrComUI{}
	, m_arrResUI{}
	, m_TargetPrefObj(nullptr)
	, m_iCurItem(0)
	, m_bMouseCheck(false)
	, m_TargetLevelPath(nullptr)
{
	// ComponentUI
	m_arrComUI[(UINT)COMPONENT_TYPE::TRANSFORM] = new TransformUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::TRANSFORM]->SetSize(ImVec2(0.f, 120.f));
	AddChild(m_arrComUI[(UINT)COMPONENT_TYPE::TRANSFORM]);

	m_arrComUI[(UINT)COMPONENT_TYPE::MESHRENDER] = new MeshRenderUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::MESHRENDER]->SetSize(ImVec2(0.f, 280.f));
	AddChild(m_arrComUI[(UINT)COMPONENT_TYPE::MESHRENDER]);

	m_arrComUI[(UINT)COMPONENT_TYPE::LIGHT2D] = new Light2DUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::LIGHT2D]->SetSize(ImVec2(0.f, 280.f));
	AddChild(m_arrComUI[(UINT)COMPONENT_TYPE::LIGHT2D]);

	m_arrComUI[(UINT)COMPONENT_TYPE::LIGHT3D] = new Light3DUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::LIGHT3D]->SetSize(ImVec2(0.f, 280.f));
	AddChild(m_arrComUI[(UINT)COMPONENT_TYPE::LIGHT3D]);

	m_arrComUI[(UINT)COMPONENT_TYPE::CAMERA] = new CameraUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::CAMERA]->SetSize(ImVec2(0.f, 280.f));
	AddChild(m_arrComUI[(UINT)COMPONENT_TYPE::CAMERA]);

	m_arrComUI[(UINT)COMPONENT_TYPE::PARTICLESYSTEM] = new ParticleSystemUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::PARTICLESYSTEM]->SetSize(ImVec2(0.f, 330.f));
	AddChild(m_arrComUI[(UINT)COMPONENT_TYPE::PARTICLESYSTEM]);

	m_arrComUI[(UINT)COMPONENT_TYPE::TILEMAP] = new TileMapUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::TILEMAP]->SetSize(ImVec2(0.f, 300.f));
	AddChild(m_arrComUI[(UINT)COMPONENT_TYPE::TILEMAP]);

	m_arrComUI[(UINT)COMPONENT_TYPE::ANIMATOR2D] = new Animator2DUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::ANIMATOR2D]->SetSize(ImVec2(0.f, 450.f));
	AddChild(m_arrComUI[(UINT)COMPONENT_TYPE::ANIMATOR2D]);

	m_arrComUI[(UINT)COMPONENT_TYPE::SKYBOX] = new SkyBoxUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::SKYBOX]->SetSize(ImVec2(0.f, 150.f));
	AddChild(m_arrComUI[(UINT)COMPONENT_TYPE::SKYBOX]);

	m_arrComUI[(UINT)COMPONENT_TYPE::DECAL] = new DecalUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::DECAL]->SetSize(ImVec2(0.f, 250.f));
	AddChild(m_arrComUI[(UINT)COMPONENT_TYPE::DECAL]);

	m_arrComUI[(UINT)COMPONENT_TYPE::RIGIDBODY] = new RigidBodyUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::RIGIDBODY]->SetSize(ImVec2(0.f, 450.f));
	AddChild(m_arrComUI[(UINT)COMPONENT_TYPE::RIGIDBODY]);

	// ResourceUI
	m_arrResUI[(UINT)RES_TYPE::MESH] = new MeshUI;
	m_arrResUI[(UINT)RES_TYPE::MESH]->SetSize(ImVec2(0.f, 0.f));
	m_arrResUI[(UINT)RES_TYPE::MESH]->ShowSeperator(false);
	m_arrResUI[(UINT)RES_TYPE::MESH]->Close();
	AddChild(m_arrResUI[(UINT)RES_TYPE::MESH]);

	m_arrResUI[(UINT)RES_TYPE::TEXTURE] = new RTextureUI;
	m_arrResUI[(UINT)RES_TYPE::TEXTURE]->SetSize(ImVec2(0.f, 0.f));
	m_arrResUI[(UINT)RES_TYPE::TEXTURE]->ShowSeperator(false);
	m_arrResUI[(UINT)RES_TYPE::TEXTURE]->Close();
	AddChild(m_arrResUI[(UINT)RES_TYPE::TEXTURE]);

	m_arrResUI[(UINT)RES_TYPE::MATERIAL] = new MaterialUI;
	m_arrResUI[(UINT)RES_TYPE::MATERIAL]->SetSize(ImVec2(0.f, 0.f));
	m_arrResUI[(UINT)RES_TYPE::MATERIAL]->ShowSeperator(false);
	m_arrResUI[(UINT)RES_TYPE::MATERIAL]->Close();
	AddChild(m_arrResUI[(UINT)RES_TYPE::MATERIAL]);

	m_arrResUI[(UINT)RES_TYPE::PREFAB] = new PreFabUI;
	m_arrResUI[(UINT)RES_TYPE::PREFAB]->SetSize(ImVec2(0.f, 0.f));
	m_arrResUI[(UINT)RES_TYPE::PREFAB]->ShowSeperator(false);
	m_arrResUI[(UINT)RES_TYPE::PREFAB]->Close();
	AddChild(m_arrResUI[(UINT)RES_TYPE::PREFAB]);

	m_arrResUI[(UINT)RES_TYPE::GRAPHICS_SHADER] = new GraphicsShaderUI;
	m_arrResUI[(UINT)RES_TYPE::GRAPHICS_SHADER]->SetSize(ImVec2(0.f, 0.f));
	m_arrResUI[(UINT)RES_TYPE::GRAPHICS_SHADER]->ShowSeperator(false);
	m_arrResUI[(UINT)RES_TYPE::GRAPHICS_SHADER]->Close();
	AddChild(m_arrResUI[(UINT)RES_TYPE::GRAPHICS_SHADER]);

	m_arrResUI[(UINT)RES_TYPE::SOUND] = new SoundUI;
	m_arrResUI[(UINT)RES_TYPE::SOUND]->SetSize(ImVec2(0.f, 0.f));
	m_arrResUI[(UINT)RES_TYPE::SOUND]->ShowSeperator(false);
	m_arrResUI[(UINT)RES_TYPE::SOUND]->Close();
	AddChild(m_arrResUI[(UINT)RES_TYPE::SOUND]);

	// LevelUI
	m_pLevelUI = new LevelUI("LevelUI##UI");
	m_pLevelUI->SetSize(ImVec2(0.f, 0.f));
	m_pLevelUI->ShowSeperator(false);
	m_pLevelUI->Close();
	AddChild(m_pLevelUI);

	ScriptUI* pScriptUI = new ScriptUI;
	pScriptUI->SetSize(ImVec2(0.f, 200.f));
	pScriptUI->Close();
	AddChild(pScriptUI);
	m_vecScriptUI.push_back(pScriptUI);
}

InspectorUI::~InspectorUI()
{
}

void InspectorUI::update()
{
	if (!IsValid(m_TargetObj))
	{
		SetTargetObject(nullptr);
	}

	UI::update();
}

void InspectorUI::render_update()
{
	if (nullptr != m_TargetObj)
	{
		ImGui::PushID(0);
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.f, 0.6f, 0.8f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.f, 0.7f, 0.9f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.f, 0.8f, 1.f));
		ImGui::Button("Select Object");
		ImGui::PopStyleColor(3);
		ImGui::PopID();
	
		ImGui::Text("Now Object : "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), string(m_TargetObj->GetName().begin(), m_TargetObj->GetName().end()).c_str());

		ImGui::SameLine();

		// ������Ʈ �̸� �����ϱ� â����
		if (ImGui::Button("##ChangeObjNameBtn", ImVec2(18.f, 18.f)))
			ImGui::OpenPopup("Change Object Name?");

		CGameObject* pParentObj = m_TargetObj;
		while (pParentObj->GetParent())
		{
			pParentObj = pParentObj->GetParent();
		}
		if (pParentObj->GetOwnerPrefab().Get())
		{
			SavePrefab(pParentObj); ImGui::SameLine(); InstantiatePrefab(pParentObj);
		}

		

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal("Change Object Name?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			static char TemName[128] = "";
			ImGui::InputText("Input Name##ObjName", TemName, IM_ARRAYSIZE(TemName));

			ImGui::Separator();

			if (ImGui::Button("OK##ObjName", ImVec2(120, 0)))
			{ 
				string NewName = TemName;
				wstring wNewName = wstring(NewName.begin(), NewName.end());
				m_TargetObj->SetName(wNewName);
				ImGui::CloseCurrentPopup();
				
				// OutLinerUI ���ΰ�ħ
				OutlinerUI* pOutlinerUI = dynamic_cast<OutlinerUI*>(CImGuiMgr::GetInst()->FindUI("Outliner"));
				pOutlinerUI->ResetLevel();
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel##ObjName", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}

		// ������Ʈ�� ���� ���̾� �����ֱ�
		ImGui::Text("Now Layer  : "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), std::to_string(m_TargetObj->GetLayerIdx()).c_str());
		
		ImGui::SameLine();
		// Layer ���� â ����
		if (ImGui::Button("##ChangeObjLayerBtn", ImVec2(18.f, 18.f)))
			ImGui::OpenPopup("Change Object Layer?");

		center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal("Change Object Layer?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			static int TemNum = 0;
			ImGui::InputInt("Input Layer Index##ObjLayer", &TemNum);

			ImGui::Separator();

			if (ImGui::Button("OK##ObjLayer", ImVec2(120, 0)))
			{
				int NewNum = TemNum;
				if (0 <= NewNum && NewNum <= 32)
				{
					// ���� ���̾�� �ش� ������Ʈ�� Layer �Ҽ��� �ٲ�޶�� ��û��.
					CLevelMgr::GetInst()->GetCurLevel()->ChangeObjectLayer(m_TargetObj, NewNum);
				}
				else
				{
					MessageBox(nullptr, L"�ùٸ��� �ʴ� ���̾� �ε��� ��", L"���̾� �ε��� Ȯ��", MB_OK);
				}
				ImGui::CloseCurrentPopup();

				// OutLinerUI ���ΰ�ħ
				OutlinerUI* pOutlinerUI = dynamic_cast<OutlinerUI*>(CImGuiMgr::GetInst()->FindUI("Outliner"));
				pOutlinerUI->ResetLevel();
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel##ObjLayer", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}
	}
}

void InspectorUI::SetTargetbyKey(DWORD_PTR _strKey)
{
	string strKey = (char*)_strKey;
	wstring wstrKey = wstring(strKey.begin(), strKey.end());

	SetTargetObject(CLevelMgr::GetInst()->FindObjectByName((wstrKey)));
}

void InspectorUI::SetTargetObject(CGameObject* _Target)
{
	// if Prev Target is Resource
	if (nullptr != m_TargetRes && _Target != nullptr)
	{
		SetTargetResource(nullptr);
	}

	// if Prev Target is Level
	if (nullptr != m_TargetLevelPath && _Target != nullptr)
	{
		SetTargetLevel(nullptr);
	}

	//if (nullptr != m_TargetPrefObj && _Target != nullptr)
	//{
	//	SetTargetPrefObject(nullptr);
	//}

	m_TargetObj = _Target;

	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr != m_arrComUI[i])
		{
			if (m_TargetObj != nullptr && nullptr == m_TargetObj->GetComponent((COMPONENT_TYPE)i))
			{
				m_arrComUI[i]->SetTarget(nullptr);
				m_arrComUI[i]->Close();
			}
			else
			{
				m_arrComUI[i]->SetTarget(_Target);

				if (nullptr != _Target)
				{
					m_arrComUI[i]->Open();
					m_arrComUI[i]->update();
				}
				else
					m_arrComUI[i]->Close();
			}
		}
	}

	// Object�� ��ũ��Ʈ Ȯ��
	if (nullptr == m_TargetObj)
	{
		for (auto& pScript : m_vecScriptUI)
		{
			pScript->Close();
		}
	}

	else
	{
		// ��� ��ũ��Ʈ UI ��Ȱ��ȭ
		for (size_t i = 0; i < m_vecScriptUI.size(); ++i)
		{
			m_vecScriptUI[i]->Close();
		}

		// ������Ʈ�� ���� ��ũ��Ʈ ������
		const vector<CScript*>& vecScript = m_TargetObj->GetScripts();

		// ��ũ��Ʈ ���� ��ũ��ƮUI ���� ������, �߰��� UI�� �� ����
		if (m_vecScriptUI.size() < vecScript.size())
		{
			for (size_t i = 0; i < vecScript.size(); ++i)
			{
				ScriptUI* pScriptUI = new ScriptUI;
				pScriptUI->Close();
				AddChild(pScriptUI);
				m_vecScriptUI.push_back(pScriptUI);
			}
		}

		// ��ũ��Ʈ�� ��ũ��ƮUI�� ����
		for (size_t i = 0; i < vecScript.size(); ++i)
		{
			m_vecScriptUI[i]->Open();
			m_vecScriptUI[i]->SetTarget(m_TargetObj);
			m_vecScriptUI[i]->SetTargetScript(vecScript[i]);
			m_vecScriptUI[i]->update();
		}
	}
}



void InspectorUI::SetTargetResource(CRes* _Resource)
{
	// if Prev Target is Object
	if (nullptr != m_TargetObj)
	{
		SetTargetObject(nullptr);
	}

	// if Prev Target is Level
	if (nullptr != m_TargetLevelPath)
	{
		SetTargetLevel(nullptr);
	}

	if (nullptr != _Resource)
	{
		// ������ ����Ű�� ���ҽ��� ������, �ش� UI�� ����
		if (nullptr != m_TargetRes && nullptr != m_arrResUI[(UINT)m_TargetRes->GetResType()])
		{
			m_arrResUI[(UINT)m_TargetRes->GetResType()]->Close();
		}

		// ���� ������ ���ҽ��� ����ϴ� UI�� Ȱ��ȭ
		m_TargetRes = _Resource;
		RES_TYPE eType = m_TargetRes->GetResType();

		if (nullptr != m_arrResUI[(UINT)eType])
		{
			m_arrResUI[(UINT)eType]->SetTarget(m_TargetRes);
			m_arrResUI[(UINT)eType]->Open();
		}
	}
	else
	{
		for (UINT i = 0; i < (UINT)RES_TYPE::END; ++i)
		{
			if (nullptr != m_arrResUI[i])
			{
				m_arrResUI[i]->SetTarget(nullptr);
				m_arrResUI[i]->Close();
			}
		}
	}
}

void InspectorUI::SetTargetLevel(const wstring* _LevelPath)
{
	// Resource�� Target�̾��ٸ�
	if (nullptr != m_TargetRes && nullptr != _LevelPath)
	{
		SetTargetResource(nullptr);
	}
	// Object�� Target�̾��ٸ�
	if (nullptr != m_TargetObj && nullptr != _LevelPath)
	{
		SetTargetObject(nullptr);
	}

	m_TargetLevelPath = _LevelPath;

	// ���� Target Level�� nullptr�� �ƴ϶�� UI���� Target�� �����ϰ� ���� �־�� �Ѵ�.
	if (nullptr != _LevelPath)
	{

		if (nullptr != m_pLevelUI)
		{
			m_pLevelUI->SetTarget(m_TargetLevelPath);
			m_pLevelUI->Open();
		}
	}

	// Target Level�� nullptr�� ���Դٸ� UI�� �ݰ� Target�� �ٽ� ������ �־�� �Ѵ�.
	else
	{
		m_pLevelUI->SetTarget(nullptr);
		m_pLevelUI->Close();
	}
}



UI* InspectorUI::FindResUI(RES_TYPE _eType)
{
	return m_arrResUI[(UINT)_eType];
}

UI* InspectorUI::FindComponentUI(COMPONENT_TYPE _eType)
{
	return m_arrComUI[(UINT)_eType];
}

void InspectorUI::SavePrefab(CGameObject* _pParentObj)
{
	// ������ ���� ���
	if (ImGui::Button("Save##PrefabSaveBtn", ImVec2(100.f, 30.f)))
	{
		CGameObject* pParentObj = _pParentObj;
		while (pParentObj->GetParent())
		{
			pParentObj = _pParentObj->GetParent();
		}
		Ptr<CPrefab> pOwnerPref = pParentObj->GetOwnerPrefab();

		if (L"" == pOwnerPref->GetRelativePath())
		{
			wstring strRelativePath;

			strRelativePath = L"prefab\\";
			strRelativePath += pOwnerPref->GetKey();
			strRelativePath += L".pref";
			pOwnerPref->Save(strRelativePath);
		}
		// ���� ��� �״�θ� �־� ������.
		else
		{
			pOwnerPref->Save(pOwnerPref->GetRelativePath());
		}
	}
}

void InspectorUI::InstantiatePrefab(CGameObject* _pParentObj)
{
	if (ImGui::Button("Instantiate", ImVec2(100.f, 30.f)))
	{
		CGameObject* pParentObj = _pParentObj;
		while (pParentObj->GetParent())
		{
			pParentObj = _pParentObj->GetParent();
		}
		Ptr<CPrefab> pOwnerPref = pParentObj->GetOwnerPrefab();

		CGameObject* pNewObj = pOwnerPref->Instantiate();
		Instantiate(pNewObj, Vec3(0.f, 0.f, 990.f), 0);
	}
}

// void InspectorUI::SetTargetPrefObject(CGameObject* _Target)
//{
//	// Object�� Ÿ���� ���¿��ٸ�
//	if (nullptr != m_TargetObj)
//	{
//		SetTargetObject(nullptr);
//	}
//
//	// Level�� Ÿ���̾��ٸ�
//	if (nullptr != m_TargetLevel)
//	{
//		SetTargetLevel(nullptr);
//	}
//
//	if (nullptr != _Target)
//	{
//		// ������ ����Ű�� ���ҽ��� ������, �ش� UI�� ����
//		if (nullptr != m_TargetRes && nullptr != m_arrResUI[(UINT)m_TargetRes->GetResType()])
//		{
//			m_arrResUI[(UINT)m_TargetRes->GetResType()]->Close();
//		}
//
//		// ���� ������ ���ҽ��� ����ϴ� UI�� Ȱ��ȭ
//		m_TargetPrefObj = _Target;
//		RES_TYPE eType = RES_TYPE::PREFAB;
//
//		if (nullptr != m_arrResUI[(UINT)eType])
//		{
//			m_arrResUI[(UINT)eType]->SetTargetObj(m_TargetPrefObj);
//			m_arrResUI[(UINT)eType]->Open();
//		}
//	}
//	else
//	{
//		for (UINT i = 0; i < (UINT)RES_TYPE::END; ++i)
//		{
//			if (nullptr != m_arrResUI[i])
//			{
//				m_arrResUI[i]->SetTarget(nullptr);
//				m_arrResUI[i]->Close();
//			}
//		}
//	}
//}