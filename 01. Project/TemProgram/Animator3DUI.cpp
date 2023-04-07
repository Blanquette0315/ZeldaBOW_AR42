#include "pch.h"
#include "Animator3DUI.h"

#include <Engine\CAnimator3D.h>
#include <Engine\CAnimation3D.h>

#include "ListUI.h"
#include "CImGuiMgr.h"

Animator3DUI::Animator3DUI()
	: ComponentUI("Animator3D", COMPONENT_TYPE::ANIMATOR3D)
	, m_Animation(nullptr)
	, m_iStartFrame(0)
	, m_iEndFrame(0)
	, m_iFrameCount(30)
	, m_bRepeat(true)
{
}

Animator3DUI::~Animator3DUI()
{
}

void Animator3DUI::update()
{
	if (nullptr != GetTarget())
	{
		if (nullptr == m_Animation)
		{
			if (GetTarget()->Animator3D()->GetCurAnimation())
			{
				m_Animation = GetTarget()->Animator3D()->GetCurAnimation();
			}
			else if (!GetTarget()->Animator3D()->GetMapAnimation().empty())
			{
				m_Animation = GetTarget()->Animator3D()->GetMapAnimation().begin()->second;
			}
		}

		if (nullptr != m_Animation)
		{
			m_iStartFrame = m_Animation->GetStartFrame();
			m_iEndFrame = m_Animation->GetEndFrame();
			m_iFrameCount = m_Animation->GetFrameCount();
		}
	}

	ComponentUI::update();
}

void Animator3DUI::render_update()
{
	ComponentUI::render_update();

	string AnimName;
	if (nullptr != m_Animation)
	{
		AnimName = string(m_Animation->GetName().begin(), m_Animation->GetName().end());
	}

	ImGui::Text("Animation Name");
	ImGui::SameLine();
	ImGui::InputText("##AnimName", (char*)AnimName.data(), AnimName.length(), ImGuiInputTextFlags_ReadOnly);
	ImGui::SameLine();
	if (ImGui::Button("##AnimBtn", Vec2(15.f, 15.f)))
	{
		ListUI* pListUI = dynamic_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("ListUI"));
		assert(pListUI);

		// 메쉬 목록을 받아와서, ListUI 에 전달
		const map<wstring, CAnimation3D*>& mapAni = GetTarget()->Animator3D()->GetMapAnimation();
		static vector<wstring> vecRes;
		vecRes.clear();

		map<wstring, CAnimation3D*>::const_iterator iter = mapAni.begin();
		for (; iter != mapAni.end(); ++iter)
		{
			vecRes.push_back(iter->first);
		}
		pListUI->SetItemList(vecRes);
		pListUI->AddDynamicDBClicked(this, (FUNC_1)&Animator3DUI::SetAnimaion);

		pListUI->Open();
	}

	if (ImGui::Button("Create##Animbtn", Vec2(50.f, 18.f)))
	{
		GetTarget()->Animator3D()->CreateDefaultAnimation();
	}

	ImGui::SameLine();

	if (ImGui::Button("Delete##Animbtn", Vec2(50.f, 18.f)))
	{
		if (m_Animation != nullptr)
		{
			GetTarget()->Animator3D()->EraseAnimation(m_Animation->GetName());
			m_Animation = nullptr;
		}
	}

	if (m_Animation != nullptr)
	{
		ImGui::Text("Now Animation : "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), string(m_Animation->GetName().begin(), m_Animation->GetName().end()).c_str());

		ImGui::SameLine();

		//이름 변경하기 창띄우기
		if (ImGui::Button("##ChangeObjNameBtn", ImVec2(18.f, 18.f)))
			ImGui::OpenPopup("Change Animation Name?");

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal("Change Animation Name?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			static char AnimName[128] = "";
			ImGui::InputText("Input Name##AnimName", AnimName, IM_ARRAYSIZE(AnimName));

			ImGui::Separator();

			if (ImGui::Button("OK##ObjName", ImVec2(120, 0)))
			{
				string NewName = AnimName;
				wstring wNewName = wstring(NewName.begin(), NewName.end());
				GetTarget()->Animator3D()->ChangeAnimName(m_Animation->GetName(), wNewName);
				m_Animation = GetTarget()->Animator3D()->FindAnimation(wNewName);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel##ObjName", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}

		ImGui::Text("StartFrame"); ImGui::SameLine(); ImGui::InputInt("##Anim3D_StartFrame", &m_iStartFrame);
		if (m_iStartFrame < 0)
		{
			m_iStartFrame = 0;
		}
		ImGui::Text("EndFrame  "); ImGui::SameLine(); ImGui::InputInt("##Anim3D_EndFrame", &m_iEndFrame);
		if (m_iEndFrame < m_iStartFrame)
		{
			m_iEndFrame = m_iStartFrame;
		}
		ImGui::Text("FPS       "); ImGui::SameLine(); ImGui::InputInt("##Anim3D_FPS", &m_iFrameCount);
		if (m_iFrameCount <= 0)
		{
			m_iFrameCount = 1;
		}

		if (GetTarget())
		{
			m_Animation->SetStartFrame(m_iStartFrame);
			m_Animation->SetEndFrame(m_iEndFrame);
			m_Animation->SetFrameCount(m_iFrameCount);
		}

		ImGui::Text("Repeat"); ImGui::SameLine(); ImGui::Checkbox("##Anim3DRepeat", &m_bRepeat);
		if (ImGui::Button("Play##Anim3DPlaybtn", Vec2(45.f, 18.f)))
		{
			GetTarget()->Animator3D()->Play(m_Animation->GetName(), m_bRepeat);
		}
		ImGui::SameLine();
		if (ImGui::Button("Stop##Anim3DStopbtn", Vec2(45.f, 18.f)))
		{
			GetTarget()->Animator3D()->Stop();
		}
	}
	if (ImGui::Button("Save##Animsavebtn", Vec2(45.f, 18.f)))
	{
		SaveAnimation();
	}
	ImGui::SameLine();
	if (ImGui::Button("Load##Animloadbtn", Vec2(45.f, 18.f)))
		ImGui::OpenPopup("Load Animation File");

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Load Animation File", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static char AnimName[128] = "";
		ImGui::InputText("Input Name##AnimName", AnimName, IM_ARRAYSIZE(AnimName));

		ImGui::Separator();

		if (ImGui::Button("OK##ObjName", ImVec2(120, 0)))
		{
			string Name = AnimName;
			wstring wName = wstring(Name.begin(), Name.end());
			LoadAnimation(wName);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel##ObjName", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}
}

void Animator3DUI::SetAnimaion(DWORD_PTR _strAnimKey)
{
	string strKey = (char*)_strAnimKey;
	wstring wstrKey = wstring(strKey.begin(), strKey.end());

	m_Animation = GetTarget()->Animator3D()->FindAnimation(wstrKey);
	m_iStartFrame = m_Animation->GetStartFrame();
	m_iEndFrame = m_Animation->GetEndFrame();
	m_iFrameCount = m_Animation->GetFrameCount();
}

void Animator3DUI::SaveAnimation()
{
	// 경로
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += L"animation\\" + GetTarget()->GetName() + L"_" + m_Animation->GetName() + L".anim";

	YAML::Emitter emitter;
	emitter << YAML::BeginMap;

	m_Animation->SaveToYAML(emitter);

	emitter << YAML::EndMap;

	std::ofstream fout(strFilePath);
	fout << emitter.c_str();
}

void Animator3DUI::LoadAnimation(const wstring& _FileName)
{
	// 경로
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += L"animation\\" + GetTarget()->GetName() + L"_" + _FileName + L".anim";

	// 파일 읽기
	YAML::Node rootNode = YAML::LoadFile(WStringToString(strFilePath));

	CAnimation3D* anim = new CAnimation3D;
	anim->LoadFromYAML(rootNode);
	GetTarget()->Animator3D()->CreateAnimation(anim);
}