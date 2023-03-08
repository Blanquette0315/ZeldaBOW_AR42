#include "pch.h"
#include "Light3DUI.h"

#include <Engine/CLight3D.h>
#include "CommonUI.h"

Light3DUI::Light3DUI()
	: ComponentUI("Light3D", COMPONENT_TYPE::LIGHT3D)
	, m_eLightType(LIGHT_TYPE::DIRECTIONAL)
	, m_iCurItem(0)
	, m_fInAngle(0.f)
	, m_fOutAngle(0.f)
	, m_fRadius(0.f)
	, m_bShowDebugDraw(false)
{
}

Light3DUI::~Light3DUI()
{
}

void Light3DUI::update()
{
	if (nullptr != GetTarget())
	{
		m_eLightType = GetTarget()->Light3D()->GetLightType();
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
		m_vDiff = GetTarget()->Light3D()->GetLightColor();
		m_vSpec = GetTarget()->Light3D()->GetLightSpec();
		m_Emb = GetTarget()->Light3D()->GetLightEmbient();
		m_fRadius = GetTarget()->Light3D()->GetRadius();
		m_fInAngle = GetTarget()->Light3D()->GetInAngle();
		m_fOutAngle = GetTarget()->Light3D()->GetOutAngle();

		m_bShowDebugDraw = GetTarget()->Light3D()->Is_ShowDebugDraw();
	}

	ComponentUI::update();
}

void Light3DUI::render_update()
{
	ComponentUI::render_update();
	ImGui::Text("ShowDebugDraw"); ImGui::SameLine(); ImGui::Checkbox("##Light3D_ShowDebugDraw", &m_bShowDebugDraw);
	GetTarget()->Light3D()->ShowDebugDraw(m_bShowDebugDraw);

	static const char* m_arrLightType[] = { "DIRECTIONAL", "POINT", "SPOT" };
	ImGui::Text("Light_Type"); ImGui::SameLine(); ImGui::Combo("##Light3D_Type", &m_iCurItem, m_arrLightType, IM_ARRAYSIZE(m_arrLightType));

	ImGui::Text("Diff Color"); ImGui::SameLine(); ImGui::InputFloat3("##L3D_DiffColor", m_vDiff);
	ImGui::Text("          "); ImGui::SameLine(); CommonUI::CreateColorPicker(m_vDiff);

	ImGui::Text("Spec Color"); ImGui::SameLine(); ImGui::InputFloat3("##L3D_SpecColor", m_vSpec);
	ImGui::Text("          "); ImGui::SameLine(); CommonUI::CreateColorPicker(m_vSpec);

	ImGui::Text("Emb  Color"); ImGui::SameLine(); ImGui::InputFloat3("##L3D_EnbColor", m_Emb);
	ImGui::Text("          "); ImGui::SameLine(); CommonUI::CreateColorPicker(m_Emb);

	if (m_eLightType == LIGHT_TYPE::POINT || m_eLightType == LIGHT_TYPE::SPOT)
	{
		ImGui::Text("Radius    "); ImGui::SameLine(); ImGui::InputFloat("##L3D_Radius", &m_fRadius);
		ImGui::Text("          "); ImGui::SameLine(); ImGui::DragFloat("##L3D_Slid_Radius", &m_fRadius, 1.f);

		if (m_eLightType == LIGHT_TYPE::SPOT)
		{
			ImGui::Text("InAngle   "); ImGui::SameLine(); ImGui::InputFloat("##L3D_InAngle", &m_fInAngle);
			m_fInAngle = (m_fInAngle / 180.f) * XM_PI;
			ImGui::Text("          "); ImGui::SameLine(); ImGui::SliderAngle("##L3D_Slid_InAngle", &m_fInAngle, 0.f, 360.f);

			ImGui::Text("OutAngle  "); ImGui::SameLine(); ImGui::InputFloat("##L3D_OutAngle", &m_fOutAngle);
			m_fOutAngle = (m_fOutAngle / 180.f) * XM_PI;
			ImGui::Text("          "); ImGui::SameLine(); ImGui::SliderAngle("##L3D_Slid_OutAngle", &m_fOutAngle, 0.f, 360.f);
		}
	}

	if (GetTarget())
	{
		switch (m_iCurItem)
		{
		case (UINT)LIGHT_TYPE::DIRECTIONAL:
		{
			GetTarget()->Light3D()->SetLightType(LIGHT_TYPE::DIRECTIONAL);
			break;
		}
		case (UINT)LIGHT_TYPE::POINT:
		{
			GetTarget()->Light3D()->SetLightType(LIGHT_TYPE::POINT);
			break;
		}
		case (UINT)LIGHT_TYPE::SPOT:
		{
			GetTarget()->Light3D()->SetLightType(LIGHT_TYPE::SPOT);
			break;
		}
		}

		GetTarget()->Light3D()->SetLightColor(m_vDiff);
		GetTarget()->Light3D()->SetLightSpecular(m_vSpec);
		GetTarget()->Light3D()->SetLightAmbient(m_Emb);

		if (m_eLightType == LIGHT_TYPE::POINT || m_eLightType == LIGHT_TYPE::SPOT)
		{
			GetTarget()->Light3D()->SetRadius(m_fRadius);

			if (m_eLightType == LIGHT_TYPE::SPOT)
			{
				m_fInAngle = (m_fInAngle / XM_PI) * 180.f;
				GetTarget()->Light3D()->SetInAngle(m_fInAngle);
				m_fOutAngle = (m_fOutAngle / XM_PI) * 180.f;
				GetTarget()->Light3D()->SetOutAngle(m_fOutAngle);
			}
		}
	}
}