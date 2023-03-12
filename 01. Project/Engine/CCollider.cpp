#include "pch.h"
#include "CCollider.h"
#include <PhysEngine/Export/PhysX_API.h>

#include "CTransform.h"
#include "CScript.h"

CCollider::CCollider()
	: CComponent(COMPONENT_TYPE::COLLIDER)
	, m_pPhysData(nullptr)
	, m_eType(COLLIDER_TYPE::COLLIDER_CUBE)
	, m_bBeCreateActor(false)
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

void CCollider::finaltick()
{
	//FinalPos
	Vec3 vObjectPos = Transform()->GetWorldPos();
	Vec3 vColliderWorldOffset = XMVector3TransformNormal(m_vOffsetPos, Transform()->GetWorldRotMat());
	m_vFinalPos = vColliderWorldOffset + vObjectPos;

	Matrix matRot = XMMatrixRotationX(m_vRot.x);
	matRot *= XMMatrixRotationY(m_vRot.y);
	matRot *= XMMatrixRotationZ(m_vRot.z);
	matRot *= Transform()->GetWorldRotMat();

	// Physx 업데이트
	if (m_bBeCreateActor)
	{
		PhysX_Update_Actor(m_pPhysData);
		m_pPhysData->SetWorldPosition(m_vFinalPos.x / 100.f, m_vFinalPos.y / 100.f, m_vFinalPos.z / 100.f);
		Quaternion Q_Rot = SimpleMath::Quaternion::CreateFromRotationMatrix(matRot);
		m_pPhysData->Rotation = Vec4(Q_Rot.x, Q_Rot.y, Q_Rot.z, Q_Rot.w);
	}
	else
	{
		m_pPhysData = PhysX_Create_Data();
		m_pPhysData->EaterObj = GetOwner();
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
		m_bBeCreateActor = true;
	}

	// DebugDraw 요청
#ifdef _DEBUG
	// 오버랩이 1개 이상이면 빨간색으로 그린다.
	Vec4 vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	if (0 < m_iOverlapCount)
		vColor = Vec4(1.f, 0.f, 0.f, 1.f);

	if (COLLIDER_TYPE::COLLIDER_CUBE == m_eType)
	{
		// 지속 시간을 0으로 넣어주는 이유는 Collider2D에서 매 Finaltick 마다 호출해주기 때문에 0이 들어가지 않으면 중첩이 발생한다.
		Vec3 vRot = m_vRot;
		CGameObject* pParent = GetOwner();
		while (true)
		{
			if (nullptr == pParent)
				break;
			
			vRot += pParent->Transform()->GetRelativeRotation();
			pParent = pParent->GetParent();
		}
		DebugDrawCube(vColor, m_vFinalPos, m_vScale * 2.f, vRot);
	}
	if (COLLIDER_TYPE::COLLIDER_SPHERE == m_eType)
	{
		DebugDrawSphere(vColor, m_vFinalPos, m_vScale.x);
	}
#endif
}

void CCollider::BeginOverlap(CCollider* _pOther)
{
	++m_iOverlapCount;

	const vector<CScript*>& vecScripts = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScripts.size(); ++i)
	{
		vecScripts[i]->BeginOverlap(_pOther);
	}
}

void CCollider::Overlap(CCollider* _pOther)
{
	const vector<CScript*>& vecScripts = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScripts.size(); ++i)
	{
		vecScripts[i]->Overlap(_pOther);
	}
}

void CCollider::EndOverlap(CCollider* _pOther)
{
	--m_iOverlapCount;

	const vector<CScript*>& vecScripts = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScripts.size(); ++i)
	{
		vecScripts[i]->EndOverlap(_pOther);
	}
}
