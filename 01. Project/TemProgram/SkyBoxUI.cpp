#include "pch.h"
#include "SkyBoxUI.h"

#include <Engine/CSkyBox.h>
#include "ParamUI.h"

SkyBoxUI::SkyBoxUI()
	: ComponentUI("SkyBoxUI", COMPONENT_TYPE::SKYBOX)
{
}

SkyBoxUI::~SkyBoxUI()
{
}

void SkyBoxUI::update()
{
	ComponentUI::update();
	if (GetTarget())
	{
		m_SkyBoxTex = GetTarget()->SkyBox()->GetSkyBoxTex();
		m_eType = GetTarget()->SkyBox()->GetType();
	}
}

void SkyBoxUI::render_update()
{
	ComponentUI::render_update();


	ParamUI::Param_Tex("SkyBoxTex", m_SkyBoxTex, this, (FUNC_1)&SkyBoxUI::SetTexture);

	if (ImGui::RadioButton("Sphere", !(int)m_eType))
	{
		m_eType = SKYBOX_TYPE::SPHERE;
		GetTarget()->SkyBox()->SetType(SKYBOX_TYPE::SPHERE);
		GetTarget()->SkyBox()->SetSkyMesh();
		GetTarget()->SkyBox()->SetSkyBoxTex(m_SkyBoxTex);
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Cube", (int)m_eType))
	{
		m_eType = SKYBOX_TYPE::CUBE;
		GetTarget()->SkyBox()->SetType(SKYBOX_TYPE::CUBE);
		GetTarget()->SkyBox()->SetSkyMesh();
		GetTarget()->SkyBox()->SetSkyBoxTex(m_SkyBoxTex);
	}
}

void SkyBoxUI::SetTexture(DWORD_PTR _strTexKey)
{
	string strKey = (char*)_strTexKey;
	wstring wstrKey = ConvertStrToWstr(strKey);

	Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(wstrKey);
	assert(pTex.Get());

	m_SkyBoxTex = pTex;
	GetTarget()->SkyBox()->SetSkyMesh();
	GetTarget()->SkyBox()->SetTexture(m_SkyBoxTex);
}
