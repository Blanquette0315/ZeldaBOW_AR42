#include "pch.h"
#include "RigidBodyUI.h"

#include <Engine/CRigidbody.h>

RigidBodyUI::RigidBodyUI()
	: ComponentUI("RigidBody", COMPONENT_TYPE::RIGIDBODY)
	, m_fMass(0.f)
	, m_fStaticFriction(0.f)
	, m_fDynamicFriction(0.f)
	, m_fRestitution(0.f)
	, m_bGravity(false)
	, m_bKinematic(false)
	, m_bDynamic(false)
	, m_arrLockAxisPos{false, false, false}
	, m_arrLockAxisRot{false, false, false}
	, m_eColliderType(COLLIDER_TYPE::COLLIDER_CUBE)
	, m_iCurItem(0)
	, m_bColScaleSize(false)
	, m_fSphereSize(0.f)
{
}

RigidBodyUI::~RigidBodyUI()
{
}

void RigidBodyUI::update()
{
	if (nullptr != GetTarget())
	{
		m_fMass = GetTarget()->RigidBody()->GetMass();
		m_fStaticFriction = GetTarget()->RigidBody()->GetStaticFricttion();
		m_fDynamicFriction = GetTarget()->RigidBody()->GetDynamicFriction();
		m_fRestitution = GetTarget()->RigidBody()->GetRestitution();

		m_vCenterPoint = GetTarget()->RigidBody()->GetCenterPoint();
		m_bGravity = GetTarget()->RigidBody()->GetGravityOption();
		m_bKinematic = GetTarget()->RigidBody()->GetKinematicOption();
		m_bDynamic = GetTarget()->RigidBody()->GetDinamicOption();

		Vec3 LockAxisPos = GetTarget()->RigidBody()->GetLockAxis_Pos();
		m_arrLockAxisPos[0] = (bool)LockAxisPos.x;
		m_arrLockAxisPos[1] = (bool)LockAxisPos.y;
		m_arrLockAxisPos[2] = (bool)LockAxisPos.z;

		Vec3 LockAxisRot = GetTarget()->RigidBody()->GetLockAxis_Rot();
		m_arrLockAxisRot[0] = (bool)LockAxisRot.x;
		m_arrLockAxisRot[1] = (bool)LockAxisRot.y;
		m_arrLockAxisRot[2] = (bool)LockAxisRot.z;

		m_eColliderType = GetTarget()->RigidBody()->GetRigidColliderType();
		switch (m_eColliderType)
		{
		case COLLIDER_TYPE::COLLIDER_CUBE:
		{
			m_iCurItem = 0;
		}
			break;

		case COLLIDER_TYPE::COLLIDER_SPHERE:
		{
			m_iCurItem = 1;
		}
			break;

		case COLLIDER_TYPE::COLLIDER_CAPSULE:
		{
			m_iCurItem = 2;
		}
			break;

		case COLLIDER_TYPE::COLLIDER_TRI:
		{
			m_iCurItem = 3;
		}
			break;

		case COLLIDER_TYPE::COLLIDER_MESH:
		{
			m_iCurItem = 4;
		}
		break;
		}

		m_bColScaleSize = GetTarget()->RigidBody()->GetColliderScaleSize();
		m_vBoxSize = GetTarget()->RigidBody()->GetBoxSize();
		m_fSphereSize = GetTarget()->RigidBody()->GetSphereSize();
		m_vCapsuleSize = GetTarget()->RigidBody()->GetCapsuleSize();

		m_vColOffSet = GetTarget()->RigidBody()->GetColOffSet();

		m_vVelocity = GetTarget()->RigidBody()->GetVelocity();
		m_vSaveVelocity = GetTarget()->RigidBody()->GetSaveVelocity();
		m_vForce = GetTarget()->RigidBody()->GetForce();
	}

	ComponentUI::update();
}

void RigidBodyUI::render_update()
{
	ComponentUI::render_update();
	// ...
	// will be add DebugDraw
	// ...

	//Mtrl Setting
	ImGui::Text("== PhysData Setting ==");
	ImGui::Text("Mass           "); ImGui::SameLine(); ImGui::InputFloat("##RB_Mass", &m_fMass);

	ImGui::Text("StaticFriction "); ImGui::SameLine(); ImGui::InputFloat("##RB_StaticFriction", &m_fStaticFriction);

	ImGui::Text("DynamicFriction"); ImGui::SameLine(); ImGui::InputFloat("##RB_DynamicFriction", &m_fDynamicFriction);

	ImGui::Text("Restitution    "); ImGui::SameLine(); ImGui::InputFloat("##RB_Restitution", &m_fRestitution);

	ImGui::Text("Center Point   "); ImGui::SameLine(); ImGui::InputFloat3("##RB_CenterPoint", m_vCenterPoint);

	ImGui::Text("Option");
	ImGui::Text("Gravity"); ImGui::SameLine(); ImGui::Checkbox("##RB_GravitOP", &m_bGravity); ImGui::SameLine();
	ImGui::Text("Kinematic"); ImGui::SameLine(); ImGui::Checkbox("##RB_KinematicOP", &m_bKinematic); ImGui::SameLine();
	ImGui::Text("Dynamic"); ImGui::SameLine(); ImGui::Checkbox("##RB_DynamicOP", &m_bDynamic);

	ImGui::Text("Lock Axis Pos  "); ImGui::SameLine(); ImGui::Checkbox("_X##RB_LAP", &m_arrLockAxisPos[0]);
	ImGui::SameLine(); ImGui::Checkbox("_Y##RB_LAP", &m_arrLockAxisPos[1]);
	ImGui::SameLine(); ImGui::Checkbox("_Z##RB_LAP", &m_arrLockAxisPos[2]);

	ImGui::Text("Lock Axis Rot  "); ImGui::SameLine(); ImGui::Checkbox("_X##RB_LAR", &m_arrLockAxisRot[0]);
	ImGui::SameLine(); ImGui::Checkbox("_Y##RB_LAR", &m_arrLockAxisRot[1]);
	ImGui::SameLine(); ImGui::Checkbox("_Z##RB_LAR", &m_arrLockAxisRot[2]);
	ImGui::Text("");

	// Collider Setting
	ImGui::Text("== Collider Setting ==");
	static const char* m_arrRigidColType[] = { "CUBE", "SPHERE", "CAPSULE", "TRIANGLE", "MESH"};
	ImGui::Text("Collider Type  "); ImGui::SameLine(); ImGui::Combo("##Light3D_Type", &m_iCurItem, m_arrRigidColType, IM_ARRAYSIZE(m_arrRigidColType));

	ImGui::Text("Size = Scale   "); ImGui::SameLine(); ImGui::Checkbox("##RB_ColScaleSize", &m_bColScaleSize);
	ImGui::Text("Collider OffSet"); ImGui::SameLine(); ImGui::InputFloat3("##RB_ColOffSet", m_vColOffSet);

	switch (m_eColliderType)
	{
	case COLLIDER_TYPE::COLLIDER_CUBE:
	{
		ImGui::Text("Col Box Size   "); ImGui::SameLine(); ImGui::InputFloat3("##RB_ColBoxSize", m_vBoxSize);
	}
		break;

	case COLLIDER_TYPE::COLLIDER_SPHERE:
	{
		ImGui::Text("Col Sphere Size"); ImGui::SameLine(); ImGui::InputFloat("##RB_ColSphereSize", &m_fSphereSize);
	}
		break;

	case COLLIDER_TYPE::COLLIDER_CAPSULE:
	{
		ImGui::Text("Col Capsule Size"); ImGui::SameLine(); ImGui::InputFloat2("##RB_CapsuleSize", m_vCapsuleSize);
	}
		break;

	case COLLIDER_TYPE::COLLIDER_TRI:
	{

	}
		break;

	case COLLIDER_TYPE::COLLIDER_MESH:
	{

	}
	break;
	}
	ImGui::Text("");

	ImGui::Text("== Only Debug ==");
	ImGui::Text("Show Velocity "); ImGui::SameLine(); ImGui::InputFloat3("##RB_Velocity", m_vVelocity, "%.3f", ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);
	ImGui::Text("Show SVelocity"); ImGui::SameLine(); ImGui::InputFloat3("##RB_SVelocity", m_vSaveVelocity, "%.3f", ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);
	ImGui::Text("Show Force    "); ImGui::SameLine(); ImGui::InputFloat3("##RB_Force", m_vForce, "%.3f", ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);



	if (GetTarget())
	{
		GetTarget()->RigidBody()->SetMass(m_fMass);
		GetTarget()->RigidBody()->SetStaticFriction(m_fStaticFriction);
		GetTarget()->RigidBody()->SetDynamicFriction(m_fDynamicFriction);
		GetTarget()->RigidBody()->SetRestitution(m_fRestitution);

		GetTarget()->RigidBody()->SetCenterPoint(m_vCenterPoint);
		GetTarget()->RigidBody()->SetGravityOption(m_bGravity);
		GetTarget()->RigidBody()->SetKinematicOption(m_bKinematic);
		GetTarget()->RigidBody()->SetDinamicOption(m_bDynamic);

		GetTarget()->RigidBody()->SetLockAxis_Pos(m_arrLockAxisPos[0], m_arrLockAxisPos[1], m_arrLockAxisPos[2]);
		GetTarget()->RigidBody()->SetLockAxis_Rot(m_arrLockAxisRot[0], m_arrLockAxisRot[1], m_arrLockAxisRot[2]);

		switch (m_iCurItem)
		{
		case (UINT)COLLIDER_TYPE::COLLIDER_CUBE:
		{
			GetTarget()->RigidBody()->SetColliderType(COLLIDER_TYPE::COLLIDER_CUBE);
			GetTarget()->RigidBody()->SetBoxSize(m_vBoxSize);
		}
			break;

		case (UINT)COLLIDER_TYPE::COLLIDER_SPHERE:
		{
			GetTarget()->RigidBody()->SetColliderType(COLLIDER_TYPE::COLLIDER_SPHERE);
			GetTarget()->RigidBody()->SetSphereSize(m_fSphereSize);
		}
			break;

		case (UINT)COLLIDER_TYPE::COLLIDER_CAPSULE:
		{
			GetTarget()->RigidBody()->SetColliderType(COLLIDER_TYPE::COLLIDER_CAPSULE);
			GetTarget()->RigidBody()->SetCapsuleSize(m_vCapsuleSize);
		}
			break;

		case (UINT)COLLIDER_TYPE::COLLIDER_TRI:
		{
			
		}
			break;

		case (UINT)COLLIDER_TYPE::COLLIDER_MESH:
		{
			GetTarget()->RigidBody()->SetColliderType(COLLIDER_TYPE::COLLIDER_MESH);
		}
		break;
		}

		GetTarget()->RigidBody()->SetColldierScaleSize(m_bColScaleSize);
		GetTarget()->RigidBody()->SetColOffSet(m_vColOffSet);
	}
}