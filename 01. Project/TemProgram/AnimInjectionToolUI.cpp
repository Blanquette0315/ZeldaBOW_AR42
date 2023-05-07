#include "pch.h"
#include "AnimInjectionToolUI.h"

#include <Engine/CResMgr.h>

#include "ComboBoxUI.h"

AnimInjectionToolUI::AnimInjectionToolUI()
	: UI("AnimInjectionTool")
{
	m_ComoboSrc = new ComboBoxUI;
	m_ComoboTarget = new ComboBoxUI;
	Close();
}

AnimInjectionToolUI::~AnimInjectionToolUI()
{
	delete m_ComoboSrc;
	delete m_ComoboTarget;
}

void AnimInjectionToolUI::Open()
{
	UI::Open();
	m_ComoboSrc->init_res<CMesh>(0);
	m_ComoboTarget->init_res<CMesh>(0);
}

void AnimInjectionToolUI::update()
{
}

void AnimInjectionToolUI::render_update()
{
	ImGui::Text("Source"); ImGui::SameLine(); m_ComoboSrc->render_update();
	ImGui::Text("Target"); ImGui::SameLine(); m_ComoboTarget->render_update();

	Ptr<CMesh> pMeshSrc = CResMgr::GetInst()->FindRes<CMesh>(ConvertStrToWstr(m_ComoboSrc->GetItemKey(m_ComoboSrc->GetCurIdx())));
	Ptr<CMesh> pMeshTar = CResMgr::GetInst()->FindRes<CMesh>(ConvertStrToWstr(m_ComoboTarget->GetItemKey(m_ComoboTarget->GetCurIdx())));

	if (ImGui::Button("Injection"))
	{
		pMeshTar->InjectAnimation(pMeshSrc);
	}
}


