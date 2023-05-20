#include "pch.h"
#include "CameraUI.h"

#include <Engine/CCamera.h>
#include "ListUI_EX.h"

CameraUI::CameraUI()
	: ComponentUI("Camera", COMPONENT_TYPE::CAMERA)
	, m_eProjType(PROJ_TYPE::PERSPECTIVE)
	, m_fFar(0.f)
	, m_fScale(0.f)
	, m_bInit(false)
	, m_bShowDebugDraw(false)
{
	m_uiList = new ListUI_EX;
}

CameraUI::~CameraUI()
{
	delete m_uiList;
}


void CameraUI::Open()
{
	UI::Open();
	vector<string> vecInitName = {};
	vecInitName.reserve(MAX_LAYER);
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		vecInitName.push_back(std::to_string(i + 1));
	}

	m_uiList->SetMultiSelect(true);
	m_uiList->AddDynamicSelected(this, (FUNC_0)&CameraUI::SetLayerOnOff);
	m_iVisibleLayer = GetTarget()->Camera()->GetLayerMask();
	m_uiList->InitNotRes(vecInitName, m_iVisibleLayer);
	m_bInit = true;
}

void CameraUI::update()
{
	if (nullptr != GetTarget())
	{
		m_fFar = GetTarget()->Camera()->GetFar();
		m_fNear = GetTarget()->Camera()->GetNear();
		m_fFOV = GetTarget()->Camera()->GetFOV();
		m_fScale = GetTarget()->Camera()->GetOrthographicScale();
		m_eProjType = GetTarget()->Camera()->GetProjType();
		m_iVisibleLayer = GetTarget()->Camera()->GetLayerMask();
		m_iCamIdx = GetTarget()->Camera()->GetCamIdx();

		m_bShowDebugDraw = GetTarget()->Camera()->Is_ShowDebugDraw();
	}

	ComponentUI::update();
}

void CameraUI::render_update()
{
	ComponentUI::render_update();

	ImGui::Text("Use Deferred"); ImGui::SameLine(); ImGui::Checkbox("##Use deferred", &GetTarget()->Camera()->GetUseDeferred());
	ImGui::Text("Show Frustum"); ImGui::SameLine(); ImGui::Checkbox("##CameraFrustum", &m_bShowDebugDraw);
	GetTarget()->Camera()->ShowDebugDraw(m_bShowDebugDraw);

	if (ImGui::RadioButton("ORTHOGRAHPICS", m_eProjType == PROJ_TYPE::ORTHOGRAPHICS))
	{
		m_eProjType = PROJ_TYPE::ORTHOGRAPHICS;
		GetTarget()->Camera()->SetProjType(m_eProjType);
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("PERSPECTIVE", m_eProjType == PROJ_TYPE::PERSPECTIVE))
	{
		m_eProjType = PROJ_TYPE::PERSPECTIVE;
		GetTarget()->Camera()->SetProjType(m_eProjType);
	}
	
	switch (m_eProjType)
	{
	case PROJ_TYPE::PERSPECTIVE:
	{
		RenderPerspective();
	}
		break;
	case PROJ_TYPE::ORTHOGRAPHICS:
	{
		RenderOrtho();
	}
		break;
	}
	
	RenderGeneral();
}

void CameraUI::RenderPerspective()
{
	ImGui::Text("Near "); ImGui::SameLine(); ImGui::DragFloat("##Near", &m_fNear, 1.f);
	ImGui::Text("Far  "); ImGui::SameLine(); ImGui::DragFloat("##Far", &m_fFar, 1.f);
	ImGui::Text("FOV  "); ImGui::SameLine(); ImGui::DragFloat("##FOV", &m_fFOV, 0.1f);

	GetTarget()->Camera()->SetNear(m_fNear);
	GetTarget()->Camera()->SetFar(m_fFar);
	GetTarget()->Camera()->SetFOV(m_fFOV);
}

void CameraUI::RenderOrtho()
{
	ImGui::Text("Scale "); ImGui::SameLine(); ImGui::DragFloat("##Scale", &m_fScale, 1.f);

	GetTarget()->Camera()->SetOrthographicScale(m_fScale);
}

void CameraUI::RenderGeneral()
{
	ImGui::Text("CamOrder "); ImGui::SameLine(); ImGui::InputInt("##CamOrder", &m_iCamIdx);

	m_uiList->render_update();
}

void CameraUI::SetLayerOnOff()
{
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		vector<bool>& LayerIdxArr = m_uiList->GetMultiSelectIdx();

		if (LayerIdxArr[i])
			GetTarget()->Camera()->SetLayerVisible(i);
		else
			GetTarget()->Camera()->SetLayerInvisible(i);
	}
}
