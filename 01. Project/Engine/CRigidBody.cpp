#include "pch.h"
#include "CRigidBody.h"

#include "CLevelMgr.h"
#include "CPhysMgr.h"
#include "CTransform.h"
#include "CMeshRender.h"

CRigidBody::CRigidBody()
	: CComponent(COMPONENT_TYPE::RIGIDBODY)
	, m_pPhysData(new PhysData)
{
}

CRigidBody::~CRigidBody()
{
	if (nullptr != m_pPhysData)
	{
		// �ش� �����ͷ� ���� Actor�� ���� ����� ����ó���� ���ο� ����
		PhysX_Delete_Actor(m_pPhysData);
	}
}

void CRigidBody::tick()
{
	if (CLevelMgr::GetInst()->GetLevelState() == LEVEL_STATE::PLAY)
	{
		PhysX_Update_Actor(m_pPhysData);
		// Transform�� ���� �ùķ��̼� ��� �ݿ�
		Vec3 vPos = GetWorldPosition();
		vPos -= m_pPhysData->mCollider->GetSize();
		Transform()->SetRelativePos(vPos);

		Vec4 vQRot = GetWorldRoation();
		Vec3 vRot = {};

		QuaternionToEuler(vQRot, vRot);
		Transform()->SetRelativeRotation(vRot);
	}
}

void CRigidBody::finaltick()
{
	if (CLevelMgr::GetInst()->GetLevelState() == LEVEL_STATE::PLAY)
	{
		// Transform�� ���� ���� -> Phsx ������Ʈ
		//Vec3 vRelativePos = Transform()->GetRelativePos();
		//SetWorldPosition(Transform()->GetRelativePos());

		if (m_vVelocity != Vec3(0.f, 0.f, 0.f))
		{
			m_pPhysData->SetVelocity(m_vVelocity.x, m_vVelocity.y, m_vVelocity.z);
			m_vVelocity = Vec3(0.f, 0.f, 0.f);
		}
		else
		{
			if (m_bKeyRelease)
			{
				m_pPhysData->SetVelocity(0.f, 0.f, 0.f);
				m_bKeyRelease = false;
			}
		}
		//m_pPhysData->AddForce(m_vForce.x, m_vForce.y, m_vForce.z);
	}
}

void CRigidBody::render()
{
}


void CRigidBody::SaveToYAML(YAML::Emitter& _emitter)
{
}

void CRigidBody::LoadFromYAML(YAML::Node& _node)
{
}


void CRigidBody::UpdateTransformData(RIGIDCOLLIDER_TYPE _eColliderType, bool _bKinematick, bool _bDinamic)
{
	// ������ ����
	RigidBody()->SetWorldPosition(Transform()->GetRelativePos());

	// �ݶ��̴� Ÿ�� ����
	switch (_eColliderType)
	{
	case RIGIDCOLLIDER_TYPE::CUBE:
	{
		SetBoxCollider(Transform()->GetRelativeScale() * 0.5f);
	}
		break;

	case RIGIDCOLLIDER_TYPE::SPHERE:
	{
		SetSphereCollider(Transform()->GetRelativeScale().x * 0.5f);
	}
	break;

	case RIGIDCOLLIDER_TYPE::CAPSULE:
	{

	}
	break;
	
	case RIGIDCOLLIDER_TYPE::TRIANGLE:
	{

	}
	break;
	}

	// ȸ�� ����
	SetWorldRotation(Transform()->GetRelativeRotation());

	// Ű�׸�ƽ ����
	if (_bKinematick)
	{
		SetKinematicOption(true);
	}

	if (_bDinamic)
	{
		SetDinamicOption(true);
	}
}

void CRigidBody::SetWorldRotation(Vec3 _vWorldRot)
{
	DirectX::XMMATRIX _P = DirectX::XMMatrixRotationX(_vWorldRot.x);
	DirectX::XMMATRIX _Y = DirectX::XMMatrixRotationY(_vWorldRot.y);
	DirectX::XMMATRIX _R = DirectX::XMMatrixRotationZ(_vWorldRot.z);


	Quaternion Q_Rot = SimpleMath::Quaternion::CreateFromRotationMatrix(_R * _Y * _P);
	
	m_pPhysData->Rotation = (Vec4)Q_Rot;
}

void CRigidBody::SetTriangleCollider(int _iIdxSize, int _iVertexSize, UINT* IdxArray, Vector3* _vecVertexList)
{

}