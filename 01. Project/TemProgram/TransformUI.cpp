#include "pch.h"
#include "TransformUI.h"


#include <Engine\CTransform.h>

TransformUI::TransformUI()
	: ComponentUI("Transform", COMPONENT_TYPE::TRANSFORM)
	, m_bIgnorScale(false)
{
}

TransformUI::~TransformUI()
{
}


void TransformUI::update()
{
	if (nullptr != GetTarget())
	{
		CGameObject* Temp = GetTarget();
		m_vPos = GetTarget()->Transform()->GetRelativePos();
		m_vScale = GetTarget()->Transform()->GetRelativeScale();
		m_vRot = GetTarget()->Transform()->GetRelativeRotation();
		m_bIgnorScale = GetTarget()->Transform()->GetIgnoreParentScale();
	}
}

void TransformUI::render_update()
{
	// 타이틀 버튼을 만들어주기 위해 호출해주어야 한다.
	ComponentUI::render_update();

	// 인자로 Vec3를 못받는데, Vec3내부에 float 3개로 바뀌는 컨버젼 기능을 만들어 두어서 사용이 가능하다.
	ImGui::Text("Position");	ImGui::SameLine();	ImGui::DragFloat3("##Position", m_vPos);
	ImGui::Text("Scale   ");	ImGui::SameLine();	ImGui::DragFloat3("##Scale", m_vScale);

	m_vRot.ToDegree(); // 라디안 기준이 우리의 Rotation정보를 호도법으로 변환해 보여준다.
	ImGui::Text("Rotation");	ImGui::SameLine();	ImGui::DragFloat3("##Rotation", m_vRot);

	ImGui::Text("Ignore Parent Scale");	ImGui::SameLine();	ImGui::Checkbox("##IgnorParentScale", &m_bIgnorScale);

	if (GetTarget())
	{
		// 우리가 지닌 멤버에는 우리가 UI에 값을 입력해 변환한 값도 들어가므로 해당 값을 넣어주는 것으로 영향을 미치도록 만들었다.
		GetTarget()->Transform()->SetRelativePos(m_vPos);
		GetTarget()->Transform()->SetRelativeScale(m_vScale);

		m_vRot.ToRadian(); // 호도법으로 입력된 정보를 라디안으로 바꾸어 입력해준다.
		GetTarget()->Transform()->SetRelativeRotation(m_vRot);

		GetTarget()->Transform()->SetIgnoreParentScale(m_bIgnorScale);
	}
}