#include "pch.h"
#include "Collider2DUI.h"

#include <Engine/CCollider2D.h>

Collider2DUI::Collider2DUI()
	: ComponentUI("Collider", COMPONENT_TYPE::COLLIDER2D)
	, m_eType(COLLIDER2D_TYPE::COLLIDER2D_RECT)
	, m_bIgnorObjectScale(false)
	, m_iCurItem(0)
{
}

Collider2DUI::~Collider2DUI()
{
}

void Collider2DUI::update()
{
	if (nullptr != GetTarget())
	{
		m_eType = GetTarget()->Collider2D()->GetCollider2DType();
		switch (m_eType)
		{
		case COLLIDER2D_TYPE::COLLIDER2D_RECT:
		{
			m_iCurItem = 0;
			break;
		}
		case COLLIDER2D_TYPE::COLLIDER2D_CIRCLE: 
		{
			m_iCurItem = 1;
			break;
		}
		}

		m_vOffSet = Vec3(GetTarget()->Collider2D()->GetOffsetPos().x, GetTarget()->Collider2D()->GetOffsetPos().y, 0.f);
		m_vScale = Vec3(GetTarget()->Collider2D()->GetScale().x, GetTarget()->Collider2D()->GetScale().y , 0.f);
		m_vRot = GetTarget()->Collider2D()->GetRotation();

		m_bIgnorObjectScale = GetTarget()->Collider2D()->GetIgnorObjectScale();
	}

	ComponentUI::update();
}

void Collider2DUI::render_update()
{
	ComponentUI::render_update();

	// Collider Type : ComboBox·Î ¶ç¿ò.
	static const char* m_arrColType[] = { "COLLIDER2D_RECT", "COLLIDER2D_CIRCLE" };
	ImGui::Text("Collider_Type"); ImGui::SameLine(); ImGui::Combo("##Collider_Type", &m_iCurItem, m_arrColType, IM_ARRAYSIZE(m_arrColType));

	ImGui::Text("OffSet       "); ImGui::SameLine(); ImGui::InputFloat3("##COL_OffSet", m_vOffSet);
	ImGui::Text("Scale        "); ImGui::SameLine(); ImGui::InputFloat3("##COL_Scale", m_vScale);
	ImGui::Text("Rotation     "); ImGui::SameLine(); ImGui::InputFloat3("##COL_Rot", m_vRot);
	ImGui::Text("Ignore Parent Scale"); ImGui::SameLine(); ImGui::Checkbox("##COL_IgnorParentScale", &m_bIgnorObjectScale);

	if (GetTarget())
	{
		switch (m_iCurItem)
		{
		case 0:
		{
			GetTarget()->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::COLLIDER2D_RECT);
			break;
		}
		case 1:
		{
			GetTarget()->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::COLLIDER2D_CIRCLE);
			break;
		}
		}

		GetTarget()->Collider2D()->SetOffsetPos(Vec2(m_vOffSet.x, m_vOffSet.y));
		GetTarget()->Collider2D()->SetScale(Vec2(m_vScale.x, m_vScale.y));
		GetTarget()->Collider2D()->SetRotationX(m_vRot.x);
		GetTarget()->Collider2D()->SetRotationY(m_vRot.y);
		GetTarget()->Collider2D()->SetRotationZ(m_vRot.z);

		GetTarget()->Collider2D()->SetIgnorObjectScale(m_bIgnorObjectScale);
	}
}