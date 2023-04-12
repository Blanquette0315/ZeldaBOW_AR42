#include "pch.h"
#include "LandScapeUI.h"

#include <Engine/CLandScape.h>
#include "ParamUI.h"

LandScapeUI::LandScapeUI()
	: ComponentUI("LandScape", COMPONENT_TYPE::LANDSCAPE)
	, m_iXFaceCount(0)
	, m_iZFaceCount(0)
	, m_iMod((int)LANDSCAPE_MOD::NONE)
	, m_bCurDataCooking(false)
{
}

LandScapeUI::~LandScapeUI()
{
}


void LandScapeUI::update()
{
	if (nullptr != GetTarget())
	{
		m_iXFaceCount = (int)GetTarget()->LandScape()->GetFaceCountX();
		m_iZFaceCount = (int)GetTarget()->LandScape()->GetFaceCountZ();

		m_iMod = (int)GetTarget()->LandScape()->GetLandMod();

		m_vBrushScale = GetTarget()->LandScape()->GetBrushScale();
		m_pBrushTex = GetTarget()->LandScape()->GetBrushTex();

		m_bCurDataCooking = GetTarget()->LandScape()->IsCurDataCooking();
	}

	ComponentUI::update();
}

void LandScapeUI::render_update()
{
	ComponentUI::render_update();

	ImGui::Text("== LandScape Editor ==");

	ImGui::Text("FaceCountX"); ImGui::SameLine(); ImGui::InputInt("##LS_FaceCountX", &m_iXFaceCount);
	ImGui::Text("FaceCountZ"); ImGui::SameLine(); ImGui::InputInt("##LS_FaceCountZ", &m_iZFaceCount);

	static const char* arrLandModType[] = { "HEIGHT_MAP", "SPLAT", "NONE" };
	ImGui::Text("Land Mod  "); ImGui::SameLine(); ImGui::Combo("##LS_LandMod", &m_iMod, arrLandModType, IM_ARRAYSIZE(arrLandModType));

	ImGui::Text("BrushScale"); ImGui::SameLine(); ImGui::DragFloat2("##LS_BrushScale", m_vBrushScale, 0.001f);

	ParamUI::Param_Tex("Brush Tex", m_pBrushTex, this, (FUNC_1)&LandScapeUI::SetTexture);

	ImGui::Text(" ");
	ImGui::Text("== Data Cooking ==");

	ImGui::Text("Latest data Cooking?"); ImGui::SameLine(); ImGui::Checkbox("##LS_Cooking", &m_bCurDataCooking); ImGui::SameLine();

	if (ImGui::Button("TessData Cooking##MaxTessData", ImVec2(130.f, 36.f)))
	{
		GetTarget()->LandScape()->Cooking();
	}

	ImGui::Text(" ");
	ImGui::Text("== Save Map Data ==");

	if (ImGui::Button("Save HightTex##HightMap", ImVec2(130.f, 36.f)))
	{
		GetTarget()->LandScape()->SaveHightImage();
	}
	
	ImGui::SameLine();

	if (ImGui::Button("Save WeightData##WeightMap", ImVec2(130.f, 36.f)))
	{
		GetTarget()->LandScape()->SaveWightData();
	}

	// Setting
	GetTarget()->LandScape()->SetFaceCount(m_iXFaceCount, m_iZFaceCount);
	GetTarget()->LandScape()->SetBrushScale(m_vBrushScale);

	switch (m_iMod)
	{
	case (int)LANDSCAPE_MOD::HEIGHT_MAP:
	{
		GetTarget()->LandScape()->SetLandMod(LANDSCAPE_MOD::HEIGHT_MAP);
	}
		break;

	case (int)LANDSCAPE_MOD::SPLAT:
	{
		GetTarget()->LandScape()->SetLandMod(LANDSCAPE_MOD::SPLAT);
	}
		break;

	case (int)LANDSCAPE_MOD::NONE:
	{
		GetTarget()->LandScape()->SetLandMod(LANDSCAPE_MOD::NONE);
	}
		break;
	}
}

void LandScapeUI::SetTexture(DWORD_PTR _strTexKey)
{
	string strKey = (char*)_strTexKey;
	wstring wstrKey = ConvertStrToWstr(strKey);

	Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(wstrKey);
	assert(pTex.Get());

	m_pBrushTex = pTex;
	GetTarget()->LandScape()->SetBrushTexture(m_pBrushTex);
}