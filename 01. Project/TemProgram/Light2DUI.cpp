#include "pch.h"
#include "Light2DUI.h"

#include <Engine/CLight2D.h>

Light2DUI::Light2DUI()
	: ComponentUI("Light2D", COMPONENT_TYPE::LIGHT2D)
	, m_eLightType(LIGHT_TYPE::DIRECTIONAL)
	, m_iCurItem(0)
	, m_fAngle(0.f)
	, m_fRadius(0.f)
{
}

Light2DUI::~Light2DUI()
{
}

void Light2DUI::update()
{
	if (nullptr != GetTarget())
	{
		m_eLightType = GetTarget()->Light2D()->GetLightType();
		switch (m_eLightType)
		{
		case LIGHT_TYPE::DIRECTIONAL:
		{
			m_iCurItem = 0;
			break;
		}
		case LIGHT_TYPE::POINT:
		{
			m_iCurItem = 1;
			break;
		}
		case LIGHT_TYPE::SPOT:
		{
			m_iCurItem = 2;
			break;
		}
		
		}
		m_vDiff = GetTarget()->Light2D()->GetLightColor();
		m_vSpec = GetTarget()->Light2D()->GetLightSpec();
		m_Emb = GetTarget()->Light2D()->GetLightEmbient();
		m_fRadius = GetTarget()->Light2D()->GetRadius();
		m_fAngle = GetTarget()->Light2D()->GetAngle();
	}

	ComponentUI::update();
}

void Light2DUI::render_update()
{
	ComponentUI::render_update();

	static const char* m_arrLightType[] = { "DIRECTIONAL", "POINT", "SPOT"};
	ImGui::Text("Light_Type"); ImGui::SameLine(); ImGui::Combo("##Light2D_Type", &m_iCurItem, m_arrLightType, IM_ARRAYSIZE(m_arrLightType));

	ImGui::Text("Diff Color"); ImGui::SameLine(); ImGui::InputFloat3("##L2D_DiffColor", m_vDiff);
	ImGui::Text("Editor    "); ImGui::SameLine(); ImGui::ColorEdit3("##L2D_DiffColor2", m_vDiff);

	ImGui::Text("Spec Color"); ImGui::SameLine(); ImGui::InputFloat3("##L2D_SpecColor", m_vSpec);
	ImGui::Text("Editor    "); ImGui::SameLine(); ImGui::ColorEdit3("##L2D_SpecfColor2", m_vSpec);

	ImGui::Text("Emb  Color"); ImGui::SameLine(); ImGui::InputFloat3("##L2D_EnbColor", m_Emb);
	ImGui::Text("Editor    "); ImGui::SameLine(); ImGui::ColorEdit3("##L2D_EmbColor2", m_Emb);

	ImGui::Text("Radius    "); ImGui::SameLine(); ImGui::InputFloat("##L2D_Radius", &m_fRadius);
	ImGui::Text("          "); ImGui::SameLine(); ImGui::DragFloat("##L2D_Slid_Radius", &m_fRadius,1.f);

	ImGui::Text("Angle     "); ImGui::SameLine(); ImGui::InputFloat("##L2D_Angle", &m_fAngle);
	m_fAngle = (m_fAngle / 180.f) * XM_PI;
	ImGui::Text("          "); ImGui::SameLine(); ImGui::SliderAngle("##L2D_Slid_Angle", &m_fAngle, 0.f, 360.f);

	if (GetTarget())
	{
		switch (m_iCurItem)
		{
		case (UINT)LIGHT_TYPE::DIRECTIONAL:
		{
			GetTarget()->Light2D()->SetLightType(LIGHT_TYPE::DIRECTIONAL);
			break;
		}
		case (UINT)LIGHT_TYPE::POINT:
		{
			GetTarget()->Light2D()->SetLightType(LIGHT_TYPE::POINT);
			break;
		}
		case (UINT)LIGHT_TYPE::SPOT:
		{
			GetTarget()->Light2D()->SetLightType(LIGHT_TYPE::SPOT);
			break;
		}
		}
		
		GetTarget()->Light2D()->SetLightColor(m_vDiff);
		GetTarget()->Light2D()->SetLightSpec(m_vSpec);
		GetTarget()->Light2D()->SetLightEmbient(m_Emb);
		GetTarget()->Light2D()->SetRadius(m_fRadius);
		m_fAngle = (m_fAngle / XM_PI) * 180.f;
		GetTarget()->Light2D()->SetAngle(m_fAngle);
	}
}