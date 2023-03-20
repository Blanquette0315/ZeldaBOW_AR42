#include "pch.h"
#include "ColliderUI.h"

#include <Engine/CCollider.h>

ColliderUI::ColliderUI()
	: ComponentUI("Collider", COMPONENT_TYPE::COLLIDER)
	, m_eType(COLLIDER_TYPE::COLLIDER_CUBE)
	, m_iCurItem(0)
	, m_fRadius(0.f)
{
}

ColliderUI::~ColliderUI()
{
}

void ColliderUI::update()
{
	if (nullptr != GetTarget())
	{
		m_eType = GetTarget()->Collider()->GetColliderType();
		m_iCurItem = (UINT)m_eType;

		m_vOffSet = GetTarget()->Collider()->GetOffsetPos();
		m_vScale = GetTarget()->Collider()->GetScale();
		if (m_eType == COLLIDER_TYPE::COLLIDER_SPHERE)
		{
			m_fRadius = m_vScale.x;
		}
		else if (m_eType == COLLIDER_TYPE::COLLIDER_CAPSULE)
		{
			m_vCapsuleSize = Vec2(m_vScale.x, m_vScale.y);
		}
		m_vRot = GetTarget()->Collider()->GetRotation();
	}

	ComponentUI::update();
}

void ColliderUI::render_update()
{
	ComponentUI::render_update();

	// Collider Type : ComboBox

	static const char* m_arrColType[] = { "COLLIDER_CUBE", "COLLIDER_SPHERE", "COLLIDER_CAPSULE", "COLLIDER_TRI" };
	ImGui::Text("Collider_Type"); ImGui::SameLine(); ImGui::Combo("##COL_Collider_Type", &m_iCurItem, m_arrColType, IM_ARRAYSIZE(m_arrColType));

	if (m_iCurItem != 3)
	{
		ImGui::Text("OffSetPos    "); ImGui::SameLine(); ImGui::DragFloat3("##COL_OffSet", m_vOffSet);
		if (m_iCurItem == 0)
		{
			ImGui::Text("Scale        "); ImGui::SameLine(); ImGui::DragFloat3("##COL_Scale", m_vScale);
			if (m_vScale.x < 1.f)
				m_vScale.x = 1.f;
			if (m_vScale.y < 1.f)
				m_vScale.y = 1.f;
			if (m_vScale.z < 1.f)
				m_vScale.z = 1.f;
		}
		else if (m_iCurItem == 1)
		{
			ImGui::Text("Radius       "); ImGui::SameLine(); ImGui::DragFloat("##COL_Radius", &m_fRadius);
			if (m_fRadius < 1.f)
				m_fRadius = 1.f;
			m_vScale = Vec3(m_fRadius, m_fRadius, m_fRadius);
		}
		else if (m_iCurItem == 2)
		{
			ImGui::Text("Capsule_Size ");
			ImGui::Text("	Radius   "); ImGui::SameLine(); ImGui::DragFloat("##COL_Capsule_Radius", &m_vCapsuleSize.x);
			ImGui::Text("	Height   "); ImGui::SameLine(); ImGui::DragFloat("##COL_Capsule_Height", &m_vCapsuleSize.y);
			if (m_vCapsuleSize.x < 1.f)
				m_vCapsuleSize.x = 1.f;
			if (m_vCapsuleSize.y < 1.f)
				m_vCapsuleSize.y = 1.f;
			m_vScale = Vec3(m_vCapsuleSize.x, m_vCapsuleSize.y, m_vCapsuleSize.x);
		}
		m_vRot.ToDegree();
		ImGui::Text("Rotation     "); ImGui::SameLine(); ImGui::DragFloat3("##COL_Rot", m_vRot);
	}
	ImGui::NewLine();

	if (ImGui::Button("CreateActor##COL", ImVec2(100.f, 24.f)))
	{
		GetTarget()->Collider()->CreateColliderActor();
	}
	ImGui::SameLine();
	if (ImGui::Button("DeleteActor##COL", ImVec2(100.f, 24.f)))
	{
		GetTarget()->Collider()->DeleteColliderActor();
	}

	if (GetTarget())
	{
		GetTarget()->Collider()->SetColliderType((COLLIDER_TYPE)m_iCurItem);

		GetTarget()->Collider()->SetOffsetPos(m_vOffSet);
		GetTarget()->Collider()->SetScale(m_vScale);
		m_vRot.ToRadian();
		GetTarget()->Collider()->SetRotationX(m_vRot.x);
		GetTarget()->Collider()->SetRotationY(m_vRot.y);
		GetTarget()->Collider()->SetRotationZ(m_vRot.z);
	}
}