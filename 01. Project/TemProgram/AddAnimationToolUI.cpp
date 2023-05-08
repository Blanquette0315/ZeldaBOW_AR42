#include "pch.h"
#include "AddAnimationToolUI.h"

#include <Engine/CResMgr.h>

#include "ComboBoxUI.h"


AddAnimationToolUI::AddAnimationToolUI()
	: UI("AddAnimationTool")
{
	m_ComoboBox = new ComboBoxUI;
	Close();
}

AddAnimationToolUI::~AddAnimationToolUI()
{
	delete m_ComoboBox;
}

void AddAnimationToolUI::Open()
{
	UI::Open();
	m_ComoboBox->init_res<CMesh>(0);
}

void AddAnimationToolUI::update()
{
}

void AddAnimationToolUI::render_update()
{
	m_ComoboBox->render_update();
	ImGui::Text("FBX Name"); ImGui::SameLine(); ImGui::InputText("##FBXName", m_FBXName, 256);

	if(ImGui::Button("Add Animation"))
	{
		Ptr<CMesh> pMesh = CResMgr::GetInst()->FindRes<CMesh>(ConvertStrToWstr(m_ComoboBox->GetItemKey(m_ComoboBox->GetCurIdx())));
		pMesh->AddAnimationFromContainer(ConvertStrToWstr(m_FBXName));
	}
}


