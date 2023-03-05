#include "pch.h"
#include "CameraUI.h"

#include <Engine/CCamera.h>

CameraUI::CameraUI()
	: ComponentUI("Camera", COMPONENT_TYPE::CAMERA)
	, m_eProjType(PROJ_TYPE::PERSPECTIVE)
	, m_fFar(0.f)
	, m_fScale(0.f)
	, m_iCurItem(0)
{
}

CameraUI::~CameraUI()
{
}


void CameraUI::update()
{
	if (nullptr != GetTarget())
	{
		m_eProjType = GetTarget()->Camera()->GetProjType();
		switch (m_eProjType)
		{
		case PROJ_TYPE::PERSPECTIVE:
		{
			m_iCurItem = 0;
			break;
		}
		case PROJ_TYPE::ORTHOGRAPHICS:
		{
			m_iCurItem = 1;
			break;
		}
		}

		m_fFar = GetTarget()->Camera()->GetFar();
		m_fScale = GetTarget()->Camera()->GetOrthographicScale();
	}

	ComponentUI::update();
}

void CameraUI::render_update()
{
	ComponentUI::render_update();

	static const char* m_arrProjType[] = { "PERSPECTIVE", "ORTHOGRAPHICS" };
	ImGui::Text("Proj_Type"); ImGui::SameLine(); ImGui::Combo("##Proj_Type", &m_iCurItem, m_arrProjType, IM_ARRAYSIZE(m_arrProjType));
	ImGui::Text("Far      "); ImGui::SameLine(); ImGui::InputFloat("##CAM_Far", &m_fFar, 100.f, 1000.f);
	// ORTHO일때는 해당 부분 차단해야함.
	ImGui::Text("Scale    "); ImGui::SameLine(); ImGui::InputFloat("##CAM_Scale", &m_fScale, 0.01f, 1.0f);

	if (GetTarget())
	{
		switch (m_iCurItem)
		{
		case 0:
		{
			GetTarget()->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
			break;
		}
		case 1:
		{
			GetTarget()->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHICS);
			break;
		}
		}
		
		GetTarget()->Camera()->SetFar(m_fFar);
		GetTarget()->Camera()->SetOrthographicScale(m_fScale);
	}
}