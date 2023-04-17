#include "pch.h"
#include "CCollider.h"
#include <PhysEngine/Export/PhysX_API.h>

#include "CTransform.h"
#include "CScript.h"

CCollider::CCollider()
	: CComponent(COMPONENT_TYPE::COLLIDER)
	, m_pPhysData(nullptr)
	, m_eType(COLLIDER_TYPE::COLLIDER_CUBE)
	, m_iOverlapCount(0)
{
}

CCollider::CCollider(const CCollider& _origin)
	: CComponent(COMPONENT_TYPE::COLLIDER)
	, m_pPhysData(nullptr)
	, m_eType(_origin.m_eType)
	, m_vOffsetPos(_origin.m_vOffsetPos)
	, m_vScale(_origin.m_vScale)
	, m_vRot(_origin.m_vRot)
	, m_iOverlapCount(0)
{
}

CCollider::~CCollider()
{
	if (m_pPhysData != nullptr)
	{
		PhysX_Delete_Actor(m_pPhysData);
		m_pPhysData = nullptr;
	}
}

void CCollider::begin()
{
	CreateColliderActor();
}

void CCollider::finaltick()
{
	//FinalPos
	Vec3 vObjectPos = Transform()->GetWorldPos();
	Vec3 vColliderWorldOffset = XMVector3TransformNormal(m_vOffsetPos, Transform()->GetRotMat());
	m_vFinalPos = vColliderWorldOffset + vObjectPos;

	Vec3 vRot = m_vRot;
	Matrix matRot = XMMatrixRotationX(m_vRot.x);
	matRot *= XMMatrixRotationY(m_vRot.y);
	if (m_eType != COLLIDER_TYPE::COLLIDER_CAPSULE)
		matRot *= XMMatrixRotationZ(m_vRot.z);
	else
		matRot *= XMMatrixRotationZ(m_vRot.z + XM_PI * 0.5f);
	CGameObject* pParent = GetOwner();
	while (true)
	{
		if (nullptr == pParent)
			break;
		Vec3 vParentRot = pParent->Transform()->GetRelativeRotation();
		matRot *= XMMatrixRotationX(vParentRot.x);
		matRot *= XMMatrixRotationY(vParentRot.y);
		matRot *= XMMatrixRotationZ(vParentRot.z);
		
		pParent = pParent->GetParent();
	}

	Quaternion Q_Rot = SimpleMath::Quaternion::CreateFromRotationMatrix(matRot);

	// DebugDraw
#ifdef _DEBUG
	Vec4 vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	if (0 < m_iOverlapCount)
		vColor = Vec4(1.f, 0.f, 0.f, 1.f);

	QuaternionToEuler(Q_Rot, vRot);

	if (COLLIDER_TYPE::COLLIDER_CUBE == m_eType)
	{
		DebugDrawCube(vColor, m_vFinalPos, m_vScale * 2.f, vRot);
	}
	else if (COLLIDER_TYPE::COLLIDER_SPHERE == m_eType)
	{
		DebugDrawSphere(vColor, m_vFinalPos, m_vScale.x);
	}
	else if (COLLIDER_TYPE::COLLIDER_CAPSULE == m_eType)
	{
		DebugDrawCylinder(vColor, m_vFinalPos, Vec3(m_vScale.x * 2.f, m_vScale.x * 2.f + m_vScale.y, m_vScale.x * 2.f), vRot - Vec3(0.f, 0.f, XM_PI * 0.5f));
	}
#endif

	if (m_pPhysData == nullptr)
		return;

	// Physx update
	PhysX_Update_Actor(m_pPhysData);
	m_pPhysData->SetWorldPosition(m_vFinalPos.x / 100.f, m_vFinalPos.y / 100.f, m_vFinalPos.z / 100.f);
	m_pPhysData->Rotation = Vec4(Q_Rot.x, Q_Rot.y, Q_Rot.z, Q_Rot.w);
}

void CCollider::CreateColliderActor()
{
	if (m_pPhysData != nullptr)
		return;

	Vec3 vObjectPos = Transform()->GetWorldPos();
	Vec3 vColliderWorldOffset = XMVector3TransformNormal(m_vOffsetPos, Transform()->GetRotMat());
	m_vFinalPos = vColliderWorldOffset + vObjectPos;

	Vec3 vRot = m_vRot;
	Matrix matRot = XMMatrixRotationX(m_vRot.x);
	matRot *= XMMatrixRotationY(m_vRot.y);
	if (m_eType != COLLIDER_TYPE::COLLIDER_CAPSULE)
		matRot *= XMMatrixRotationZ(m_vRot.z);
	else
		matRot *= XMMatrixRotationZ(m_vRot.z + XM_PI * 0.5f);
	CGameObject* pParent = GetOwner();
	while (true)
	{
		if (nullptr == pParent)
			break;
		Vec3 vParentRot = pParent->Transform()->GetRelativeRotation();
		matRot *= XMMatrixRotationX(vParentRot.x);
		matRot *= XMMatrixRotationY(vParentRot.y);
		matRot *= XMMatrixRotationZ(vParentRot.z);

		pParent = pParent->GetParent();
	}

	m_pPhysData = PhysX_Create_Data();
	m_pPhysData->BOWObj = GetOwner();
	m_pPhysData->isKinematic = true;
	m_pPhysData->SetTrigger(true);
	if (m_eType == COLLIDER_TYPE::COLLIDER_CUBE)
	{
		m_pPhysData->mCollider->SetBoxCollider(m_vScale.x / 100.f, m_vScale.y / 100.f, m_vScale.z / 100.f);
	}
	else if (m_eType == COLLIDER_TYPE::COLLIDER_SPHERE)
	{
		m_pPhysData->mCollider->SetSphereCollider(m_vScale.x / 100.f);
	}
	else if (m_eType == COLLIDER_TYPE::COLLIDER_CAPSULE)
	{
		m_pPhysData->mCollider->SetCapsuleCollider(m_vScale.x / 100.f, m_vScale.y / 200.f);
	}
	m_pPhysData->SetWorldPosition(m_vFinalPos.x / 100.f, m_vFinalPos.y / 100.f, m_vFinalPos.z / 100.f);
	Quaternion Q_Rot = SimpleMath::Quaternion::CreateFromRotationMatrix(matRot);
	m_pPhysData->Rotation = Vec4(Q_Rot.x, Q_Rot.y, Q_Rot.z, Q_Rot.w);

	for (int i = 0; i < 10; ++i)
	{
		m_pPhysData->TriggerEnter_List[i] = nullptr;
		m_pPhysData->TriggerStay_List[i] = nullptr;
		m_pPhysData->TriggerExit_List[i] = nullptr;
	}

	PhysX_Create_Actor(m_pPhysData);
}

void CCollider::DeleteColliderActor()
{
	if (m_pPhysData == nullptr)
		return;

	PhysX_Delete_Actor(m_pPhysData);
	m_pPhysData = nullptr;
}

void CCollider::BeginOverlap(CGameObject* _pOther)
{
	++m_iOverlapCount;

	const vector<CScript*>& vecScripts = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScripts.size(); ++i)
	{
		vecScripts[i]->BeginOverlap(_pOther);
	}
}

void CCollider::Overlap(CGameObject* _pOther)
{
	const vector<CScript*>& vecScripts = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScripts.size(); ++i)
	{
		vecScripts[i]->Overlap(_pOther);
	}
}

void CCollider::EndOverlap(CGameObject* _pOther)
{
	--m_iOverlapCount;

	const vector<CScript*>& vecScripts = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScripts.size(); ++i)
	{
		vecScripts[i]->EndOverlap(_pOther);
	}
}

void CCollider::SaveToYAML(YAML::Emitter& _emitter)
{
	_emitter << YAML::Key << "COLLIDER";
	_emitter << YAML::Value << YAML::BeginMap;

	_emitter << YAML::Key << "ColliderType";
	_emitter << YAML::Value << (UINT)m_eType;
	_emitter << YAML::Key << "OffsetPos";
	_emitter << YAML::Value << m_vOffsetPos;
	_emitter << YAML::Key << "Scale";
	_emitter << YAML::Value << m_vScale;
	_emitter << YAML::Key << "Rotation";
	_emitter << YAML::Value << m_vRot;

	_emitter << YAML::EndMap;
}

void CCollider::LoadFromYAML(YAML::Node& _node)
{
	m_eType = (COLLIDER_TYPE)(_node["COLLIDER"]["ColliderType"].as<UINT>());
	m_vOffsetPos = _node["COLLIDER"]["OffsetPos"].as<Vec3>();
	m_vScale = _node["COLLIDER"]["Scale"].as<Vec3>();
	m_vRot = _node["COLLIDER"]["Rotation"].as<Vec3>();
}
