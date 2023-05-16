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
	, m_bUsePhysRot(true)
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

		m_bUsePhysRot = GetTarget()->RigidBody()->IsUsePhysRot();

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

		m_iQueryColFilter = GetTarget()->RigidBody()->GetQueryColliderFilter();
		m_arrQueryColFilter[0] = m_iQueryColFilter & (uint32_t)FILTER_GROUP::ePlayer;
		m_arrQueryColFilter[1] = m_iQueryColFilter & (uint32_t)FILTER_GROUP::eGround;
		m_arrQueryColFilter[2] = m_iQueryColFilter & (uint32_t)FILTER_GROUP::eMonster;
		m_arrQueryColFilter[3] = m_iQueryColFilter & (uint32_t)FILTER_GROUP::eWall;
		m_arrQueryColFilter[4] = m_iQueryColFilter & (uint32_t)FILTER_GROUP::eCollider;


		m_iQueryMskColFilter = GetTarget()->RigidBody()->GetQueryMskColFilter();
		m_arrQueryMaskColFilter[0] = m_iQueryMskColFilter & (uint32_t)FILTER_GROUP::ePlayer;
		m_arrQueryMaskColFilter[1] = m_iQueryMskColFilter & (uint32_t)FILTER_GROUP::eGround;
		m_arrQueryMaskColFilter[2] = m_iQueryMskColFilter & (uint32_t)FILTER_GROUP::eMonster;
		m_arrQueryMaskColFilter[3] = m_iQueryMskColFilter & (uint32_t)FILTER_GROUP::eWall;
		m_arrQueryMaskColFilter[4] = m_iQueryMskColFilter & (uint32_t)FILTER_GROUP::eCollider;

		m_iSimulationColFilter = GetTarget()->RigidBody()->GetSimulationColFilter();
		m_arrSimulationColFilter[0] = m_iSimulationColFilter & (uint32_t)FILTER_GROUP::ePlayer;
		m_arrSimulationColFilter[1] = m_iSimulationColFilter & (uint32_t)FILTER_GROUP::eGround;
		m_arrSimulationColFilter[2] = m_iSimulationColFilter & (uint32_t)FILTER_GROUP::eMonster;
		m_arrSimulationColFilter[3] = m_iSimulationColFilter & (uint32_t)FILTER_GROUP::eWall;
		m_arrSimulationColFilter[4] = m_iSimulationColFilter & (uint32_t)FILTER_GROUP::eCollider;

		m_iSimulationMskColFilter = GetTarget()->RigidBody()->GetSimulationMskColFilter();
		m_arrSimulationMaskColFilter[0] = m_iSimulationMskColFilter & (uint32_t)FILTER_GROUP::ePlayer;
		m_arrSimulationMaskColFilter[1] = m_iSimulationMskColFilter & (uint32_t)FILTER_GROUP::eGround;
		m_arrSimulationMaskColFilter[2] = m_iSimulationMskColFilter & (uint32_t)FILTER_GROUP::eMonster;
		m_arrSimulationMaskColFilter[3] = m_iSimulationMskColFilter & (uint32_t)FILTER_GROUP::eWall;
		m_arrSimulationMaskColFilter[4] = m_iSimulationMskColFilter & (uint32_t)FILTER_GROUP::eCollider;

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

	ImGui::Text("DebugDraw"); ImGui::SameLine(); ImGui::Checkbox("##DebugDrawCheckBox", &(GetTarget()->RigidBody()->GetDebugDraw()));
	ImGui::Text("Following Rigid"); ImGui::SameLine(); ImGui::Checkbox("##FollowingRigid", &(GetTarget()->RigidBody()->GetFollowingRigid()));

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

	ImGui::Text("Use Phys Rot   "); ImGui::SameLine(); ImGui::Checkbox("##RB_UsePhysRot", &m_bUsePhysRot);
	ImGui::Text("");

	// Collider Setting
	ImGui::Text("== Collider Setting ==");

	static int FilterItem = 0;
	static const char* m_arrFilterType[] = { "QueryFilter", "QueryMaskFilter", "SimulationFilter", "SimulationMaskFilter"};
	ImGui::Text("Filter Setting:"); ImGui::SameLine(); ImGui::Combo("##Filter_Type", &FilterItem, m_arrFilterType, IM_ARRAYSIZE(m_arrFilterType));

	switch (FilterItem)
	{
	case 0:
	{
		if (ImGui::Selectable("Player", &m_arrQueryColFilter[0], 0, ImVec2(50.f, 15.f)))
		{
			if (m_arrQueryColFilter[0])
				m_iQueryColFilter |= (uint32_t)FILTER_GROUP::ePlayer;
			else
				m_iQueryColFilter ^= (uint32_t)FILTER_GROUP::ePlayer;

			GetTarget()->RigidBody()->SetQueryColliderFilter(m_iQueryColFilter);
		}
		if (ImGui::Selectable("Ground##Q", &m_arrQueryColFilter[1], 0, ImVec2(50.f, 15.f)))
		{
			if (m_arrQueryColFilter[1])
				m_iQueryColFilter |= (uint32_t)FILTER_GROUP::eGround;
			else
				m_iQueryColFilter ^= (uint32_t)FILTER_GROUP::eGround;

			GetTarget()->RigidBody()->SetQueryColliderFilter(m_iQueryColFilter);
		}
		if (ImGui::Selectable("Monster##Q", &m_arrQueryColFilter[2], 0, ImVec2(50.f, 15.f)))
		{
			if (m_arrQueryColFilter[2])
				m_iQueryColFilter |= (uint32_t)FILTER_GROUP::eMonster;
			else
				m_iQueryColFilter ^= (uint32_t)FILTER_GROUP::eMonster;

			GetTarget()->RigidBody()->SetQueryColliderFilter(m_iQueryColFilter);
		}
		if (ImGui::Selectable("Wall##Q", &m_arrQueryColFilter[3], 0, ImVec2(50.f, 15.f)))
		{
			if (m_arrQueryColFilter[3])
				m_iQueryColFilter |= (uint32_t)FILTER_GROUP::eWall;
			else
				m_iQueryColFilter ^= (uint32_t)FILTER_GROUP::eWall;

			GetTarget()->RigidBody()->SetQueryColliderFilter(m_iQueryColFilter);
		}
		if (ImGui::Selectable("Collider##Q", &m_arrQueryColFilter[4], 0, ImVec2(50.f, 15.f)))
		{
			if (m_arrQueryColFilter[4])
				m_iQueryColFilter |= (uint32_t)FILTER_GROUP::eCollider;
			else
				m_iQueryColFilter ^= (uint32_t)FILTER_GROUP::eCollider;

			GetTarget()->RigidBody()->SetQueryColliderFilter(m_iQueryColFilter);
		}
		break;
	}
	case 1:
	{
		if (ImGui::Selectable("Player##QM", &m_arrQueryMaskColFilter[0], 0, ImVec2(50.f, 15.f)))
		{
			if (m_arrQueryMaskColFilter[0])
				m_iQueryMskColFilter |= (uint32_t)FILTER_GROUP::ePlayer;
			else
				m_iQueryMskColFilter ^= (uint32_t)FILTER_GROUP::ePlayer;

			GetTarget()->RigidBody()->SetQueryMskColFilter(m_iQueryMskColFilter);
		}
		if (ImGui::Selectable("Ground##QM", &m_arrQueryMaskColFilter[1], 0, ImVec2(50.f, 15.f)))
		{
			if (m_arrQueryMaskColFilter[1])
				m_iQueryMskColFilter |= (uint32_t)FILTER_GROUP::eGround;
			else
				m_iQueryMskColFilter ^= (uint32_t)FILTER_GROUP::eGround;

			GetTarget()->RigidBody()->SetQueryMskColFilter(m_iQueryMskColFilter);
		}
		if (ImGui::Selectable("Monster##QM", &m_arrQueryMaskColFilter[2], 0, ImVec2(50.f, 15.f)))
		{
			if (m_arrQueryMaskColFilter[2])
				m_iQueryMskColFilter |= (uint32_t)FILTER_GROUP::eMonster;
			else
				m_iQueryMskColFilter ^= (uint32_t)FILTER_GROUP::eMonster;

			GetTarget()->RigidBody()->SetQueryMskColFilter(m_iQueryMskColFilter);
		}
		if (ImGui::Selectable("Wall##QM", &m_arrQueryMaskColFilter[3], 0, ImVec2(50.f, 15.f)))
		{
			if (m_arrQueryMaskColFilter[3])
				m_iQueryMskColFilter |= (uint32_t)FILTER_GROUP::eWall;
			else
				m_iQueryMskColFilter ^= (uint32_t)FILTER_GROUP::eWall;

			GetTarget()->RigidBody()->SetQueryMskColFilter(m_iQueryMskColFilter);
		}
		if (ImGui::Selectable("Collider##QM", &m_arrQueryMaskColFilter[4], 0, ImVec2(50.f, 15.f)))
		{
			if (m_arrQueryMaskColFilter[4])
				m_iQueryMskColFilter |= (uint32_t)FILTER_GROUP::eCollider;
			else
				m_iQueryMskColFilter ^= (uint32_t)FILTER_GROUP::eCollider;

			GetTarget()->RigidBody()->SetQueryMskColFilter(m_iQueryMskColFilter);
		}
		break;
	}
	case 2:
	{
		if (ImGui::Selectable("Player##S", &m_arrSimulationColFilter[0], 0, ImVec2(50.f, 15.f)))
		{
			if (m_arrSimulationColFilter[0])
				m_iSimulationColFilter |= (uint32_t)FILTER_GROUP::ePlayer;
			else
				m_iSimulationColFilter ^= (uint32_t)FILTER_GROUP::ePlayer;

			GetTarget()->RigidBody()->SetSimulationColFilter(m_iSimulationColFilter);
		}
		if (ImGui::Selectable("Ground##S", &m_arrSimulationColFilter[1], 0, ImVec2(50.f, 15.f)))
		{
			if (m_arrSimulationColFilter[1])
				m_iSimulationColFilter |= (uint32_t)FILTER_GROUP::eGround;
			else
				m_iSimulationColFilter ^= (uint32_t)FILTER_GROUP::eGround;

			GetTarget()->RigidBody()->SetSimulationColFilter(m_iSimulationColFilter);
		}
		if (ImGui::Selectable("Monster##S", &m_arrSimulationColFilter[2], 0, ImVec2(50.f, 15.f)))
		{
			if (m_arrSimulationColFilter[2])
				m_iSimulationColFilter |= (uint32_t)FILTER_GROUP::eMonster;
			else
				m_iSimulationColFilter ^= (uint32_t)FILTER_GROUP::eMonster;

			GetTarget()->RigidBody()->SetSimulationColFilter(m_iSimulationColFilter);
		}
		if (ImGui::Selectable("Wall##S", &m_arrSimulationColFilter[3], 0, ImVec2(50.f, 15.f)))
		{
			if (m_arrSimulationColFilter[3])
				m_iSimulationColFilter |= (uint32_t)FILTER_GROUP::eWall;
			else
				m_iSimulationColFilter ^= (uint32_t)FILTER_GROUP::eWall;

			GetTarget()->RigidBody()->SetSimulationColFilter(m_iSimulationColFilter);
		}
		if (ImGui::Selectable("Collider##S", &m_arrSimulationColFilter[4], 0, ImVec2(50.f, 15.f)))
		{
			if (m_arrSimulationColFilter[4])
				m_iSimulationColFilter |= (uint32_t)FILTER_GROUP::eCollider;
			else
				m_iSimulationColFilter ^= (uint32_t)FILTER_GROUP::eCollider;

			GetTarget()->RigidBody()->SetSimulationColFilter(m_iSimulationColFilter);
		}
		break;
	}
	case 3:
	{
		if (ImGui::Selectable("Player##SM", &m_arrSimulationMaskColFilter[0], 0, ImVec2(50.f, 15.f)))
		{
			if (m_arrSimulationMaskColFilter[0])
				m_iSimulationMskColFilter |= (uint32_t)FILTER_GROUP::ePlayer;
			else
				m_iSimulationMskColFilter ^= (uint32_t)FILTER_GROUP::ePlayer;

			GetTarget()->RigidBody()->SetSimulationMskColFilter(m_iSimulationMskColFilter);
		}
		if (ImGui::Selectable("Ground##SM", &m_arrSimulationMaskColFilter[1], 0, ImVec2(50.f, 15.f)))
		{
			if (m_arrSimulationMaskColFilter[1])
				m_iSimulationMskColFilter |= (uint32_t)FILTER_GROUP::eGround;
			else
				m_iSimulationMskColFilter ^= (uint32_t)FILTER_GROUP::eGround;

			GetTarget()->RigidBody()->SetSimulationMskColFilter(m_iSimulationMskColFilter);
		}
		if (ImGui::Selectable("Monster##SM", &m_arrSimulationMaskColFilter[2], 0, ImVec2(50.f, 15.f)))
		{
			if (m_arrSimulationMaskColFilter[2])
				m_iSimulationMskColFilter |= (uint32_t)FILTER_GROUP::eMonster;
			else
				m_iSimulationMskColFilter ^= (uint32_t)FILTER_GROUP::eMonster;

			GetTarget()->RigidBody()->SetSimulationMskColFilter(m_iSimulationMskColFilter);
		}
		if (ImGui::Selectable("Wall##SM", &m_arrSimulationMaskColFilter[3], 0, ImVec2(50.f, 15.f)))
		{
			if (m_arrSimulationMaskColFilter[3])
				m_iSimulationMskColFilter |= (uint32_t)FILTER_GROUP::eWall;
			else
				m_iSimulationMskColFilter ^= (uint32_t)FILTER_GROUP::eWall;

			GetTarget()->RigidBody()->SetSimulationMskColFilter(m_iSimulationMskColFilter);
		}

		if (ImGui::Selectable("Collider##SM", &m_arrSimulationMaskColFilter[4], 0, ImVec2(50.f, 15.f)))
		{
			if (m_arrSimulationMaskColFilter[4])
				m_iSimulationMskColFilter |= (uint32_t)FILTER_GROUP::eCollider;
			else
				m_iSimulationMskColFilter ^= (uint32_t)FILTER_GROUP::eCollider;

			GetTarget()->RigidBody()->SetSimulationMskColFilter(m_iSimulationMskColFilter);
		}
		break;
	}
	}

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
		GetTarget()->RigidBody()->SetUsePhysRot(m_bUsePhysRot);

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