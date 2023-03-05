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
	// Ÿ��Ʋ ��ư�� ������ֱ� ���� ȣ�����־�� �Ѵ�.
	ComponentUI::render_update();

	// ���ڷ� Vec3�� ���޴µ�, Vec3���ο� float 3���� �ٲ�� ������ ����� ����� �ξ ����� �����ϴ�.
	ImGui::Text("Position");	ImGui::SameLine();	ImGui::DragFloat3("##Position", m_vPos);
	ImGui::Text("Scale   ");	ImGui::SameLine();	ImGui::DragFloat3("##Scale", m_vScale);

	m_vRot.ToDegree(); // ���� ������ �츮�� Rotation������ ȣ�������� ��ȯ�� �����ش�.
	ImGui::Text("Rotation");	ImGui::SameLine();	ImGui::DragFloat3("##Rotation", m_vRot);

	ImGui::Text("Ignore Parent Scale");	ImGui::SameLine();	ImGui::Checkbox("##IgnorParentScale", &m_bIgnorScale);

	if (GetTarget())
	{
		// �츮�� ���� ������� �츮�� UI�� ���� �Է��� ��ȯ�� ���� ���Ƿ� �ش� ���� �־��ִ� ������ ������ ��ġ���� �������.
		GetTarget()->Transform()->SetRelativePos(m_vPos);
		GetTarget()->Transform()->SetRelativeScale(m_vScale);

		m_vRot.ToRadian(); // ȣ�������� �Էµ� ������ �������� �ٲپ� �Է����ش�.
		GetTarget()->Transform()->SetRelativeRotation(m_vRot);

		GetTarget()->Transform()->SetIgnoreParentScale(m_bIgnorScale);
	}
}