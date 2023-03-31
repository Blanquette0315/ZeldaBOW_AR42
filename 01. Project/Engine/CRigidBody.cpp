#include "pch.h"
#include "CRigidBody.h"

#include "CLevelMgr.h"
#include "CPhysMgr.h"
#include "CTransform.h"
#include "CMeshRender.h"
#include "CLandScape.h"

CRigidBody::CRigidBody()
	: CComponent(COMPONENT_TYPE::RIGIDBODY)
	, m_pPhysData(new PhysData)
	, m_pToGroundRay(new PhysRayCast)
	, m_eRigidColliderType(COLLIDER_TYPE::COLLIDER_CUBE)
	, m_bKeyRelease(false)
	, m_vCapsuleSize(Vec2(1.f, 2.f))
	, m_fShpereSize(1.f)
	, m_bGround(false)
	, m_bColScaleSize(false)
{
}

CRigidBody::CRigidBody(const CRigidBody& _origin)
	: CComponent(COMPONENT_TYPE::RIGIDBODY)
	, m_pPhysData(new PhysData)
	, m_pToGroundRay(new PhysRayCast)
	, m_eRigidColliderType(_origin.m_eRigidColliderType)
	, m_bKeyRelease(false)
	, m_vCapsuleSize(_origin.m_vCapsuleSize)
	, m_fShpereSize(_origin.m_fShpereSize)
	, m_bGround(false)
	, m_bColScaleSize(false)
{
}

CRigidBody::~CRigidBody()
{
	if (nullptr != m_pPhysData)
	{
		PhysX_Delete_Actor(m_pPhysData);
	}
	if (nullptr != m_pToGroundRay)
	{
		delete m_pToGroundRay;
	}
}

void CRigidBody::begin()
{
	m_pPhysData->BOWObj = GetOwner();
	if (nullptr != LandScape())
	{
		LandScape()->CreateActor();
	}
	UpdateTransformData(m_eRigidColliderType, m_pPhysData->isKinematic, m_pPhysData->isDinamic);
	CreateActor();
}

void CRigidBody::tick()
{
	if (CLevelMgr::GetInst()->GetLevelState() == LEVEL_STATE::PLAY)
	{
		Vec3 vPrePos = GetWorldPosition();


		PhysX_Update_Actor(m_pPhysData);
		// Reflect simulation results to Transform
		Vec3 vPos = GetWorldPosition();
		Transform()->SetRelativePos(vPos);

		Vec4 vQRot = GetWorldRoation();
		Vec3 vRot = {};

		QuaternionToEuler(vQRot, vRot);
		if (m_eRigidColliderType != COLLIDER_TYPE::COLLIDER_CAPSULE)
			Transform()->SetRelativeRotation(vRot);
		else
			Transform()->SetRelativeRotation(vRot + Vec3(0.f, 0.f, -XM_PI * 0.5f));

		// ToGroundRay Refresh
		vPos = Transform()->GetRelativePos();
		vPos.y -= Transform()->GetRelativeScale().y * 0.5f;
		vPos /= 100.f;
		m_pToGroundRay->SetStartOrigin(vPos.x, vPos.y, vPos.z);

		PhysX_RayCast(m_pToGroundRay);
		//m_bGround = PhysX_RayCast(m_pToGroundRay);
	}
}

void CRigidBody::finaltick()
{
	if (CLevelMgr::GetInst()->GetLevelState() == LEVEL_STATE::PLAY)
	{
		// Velocity gathering update : Engine -> PhysX

		/*if (m_vVelocity != Vec3(0.f, 0.f, 0.f))
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
		}*/

		

		if (m_bGround)
		{
			if (m_vVelocity != Vec3(0.f, 0.f, 0.f)) 
			{
				m_pPhysData->SetVelocity(m_vVelocity.x, m_vVelocity.y, m_vVelocity.z);
				m_vSaveVelocity = m_vVelocity;
				m_vVelocity = Vec3(0.f, 0.f, 0.f);
			}
			else
			{
				if (m_bKeyRelease)
				{
					m_pPhysData->SetVelocity(0.f, 0.f, 0.f);
					m_vVelocity = Vec3(0.f, 0.f, 0.f);
					m_vSaveVelocity = m_vVelocity;
					m_bKeyRelease = false;
				}
			}
		}
		else
		{
			if (m_pPhysData->m_vPxLinearVelocity.y != 0.f)
			{
				if (m_vSaveVelocity != Vec3(0.f, 0.f, 0.f))
				{
					m_pPhysData->SetVelocity(m_vSaveVelocity.x, m_pPhysData->m_vPxLinearVelocity.y, m_vSaveVelocity.z);
					m_vSaveVelocity = Vec3(0.f, 0.f, 0.f);
				}
			}
		}

		// ...
		// will be added Force
		m_pPhysData->AddForce(m_vForce.x, m_vForce.y, m_vForce.z);
		m_vForce = Vec3(0.f, 0.f, 0.f);
		// ...
	}
}

void CRigidBody::render()
{
}


void CRigidBody::SaveToYAML(YAML::Emitter& _emitter)
{
	_emitter << YAML::Key << "RIGIDBODY";
	_emitter << YAML::Value << YAML::BeginMap;

	_emitter << YAML::Key << "PhysDataMass";
	_emitter << YAML::Value << m_pPhysData->MT_Mass;
	_emitter << YAML::Key << "PhysDataStaticFriction";
	_emitter << YAML::Value << m_pPhysData->mMeterial->MT_StaticFriction;
	_emitter << YAML::Key << "PhysDataDynamicFriction";
	_emitter << YAML::Value << m_pPhysData->mMeterial->MT_DynamicFriction;
	_emitter << YAML::Key << "PhysDataRestitution";
	_emitter << YAML::Value << m_pPhysData->mMeterial->MT_Restitution;
	_emitter << YAML::Key << "PhysDataGravity";
	_emitter << YAML::Value << m_pPhysData->isGravity;
	_emitter << YAML::Key << "PhysDataKinematic";
	_emitter << YAML::Value << m_pPhysData->isKinematic;
	_emitter << YAML::Key << "PhysDataDynamic";
	_emitter << YAML::Value << m_pPhysData->isDinamic;
	_emitter << YAML::Key << "PhysDataLockAxis_Position";
	_emitter << YAML::Value << m_pPhysData->GetLockAxis_Position();
	_emitter << YAML::Key << "PhysDataLockAxis_Rotation";
	_emitter << YAML::Value << m_pPhysData->GetLockAxis_Rotation();
	_emitter << YAML::Key << "PhysDataColliderType";
	_emitter << YAML::Value << (UINT)m_eRigidColliderType;
	_emitter << YAML::Key << "PhysDataColliderFilter";
	_emitter << YAML::Value << m_pPhysData->GetFilterData0();
	_emitter << YAML::Key << "CapsuleSize";
	_emitter << YAML::Value << m_vCapsuleSize;
	_emitter << YAML::Key << "BoxSize";
	_emitter << YAML::Value << m_vBoxSize;
	_emitter << YAML::Key << "SphereSize";
	_emitter << YAML::Value << m_fShpereSize;
	_emitter << YAML::Key << "ColScaleSize";
	_emitter << YAML::Value << m_bColScaleSize;

	_emitter << YAML::EndMap;
}

void CRigidBody::LoadFromYAML(YAML::Node& _node)
{
	m_pPhysData->MT_Mass = _node["RIGIDBODY"]["PhysDataMass"].as<float>();
	m_pPhysData->mMeterial->MT_StaticFriction = _node["RIGIDBODY"]["PhysDataStaticFriction"].as<float>();
	m_pPhysData->mMeterial->MT_DynamicFriction = _node["RIGIDBODY"]["PhysDataDynamicFriction"].as<float>();
	m_pPhysData->mMeterial->MT_Restitution = _node["RIGIDBODY"]["PhysDataRestitution"].as<float>();
	m_pPhysData->isGravity = _node["RIGIDBODY"]["PhysDataGravity"].as<bool>();
	m_pPhysData->isKinematic = _node["RIGIDBODY"]["PhysDataKinematic"].as<bool>();
	m_pPhysData->isDinamic = _node["RIGIDBODY"]["PhysDataDynamic"].as<bool>();
	Vec3 LockAxisPos = _node["RIGIDBODY"]["PhysDataLockAxis_Position"].as<Vec3>();
	m_pPhysData->SetLockAxis_Position((bool)LockAxisPos.x, (bool)LockAxisPos.y, (bool)LockAxisPos.z);
	Vec3 LocakAxisRot = _node["RIGIDBODY"]["PhysDataLockAxis_Rotation"].as<Vec3>();
	m_pPhysData->SetLockAxis_Rotation((bool)LocakAxisRot.x, (bool)LocakAxisRot.y, (bool)LocakAxisRot.z);
	m_eRigidColliderType = (COLLIDER_TYPE)(_node["RIGIDBODY"]["PhysDataColliderType"].as<UINT>());
	uint32_t Filter = _node["RIGIDBODY"]["PhysDataColliderFilter"].as<uint32_t>();
	m_vCapsuleSize = _node["RIGIDBODY"]["CapsuleSize"].as<Vec2>();
	m_vBoxSize = _node["RIGIDBODY"]["BoxSize"].as<Vec3>();
	m_fShpereSize = _node["RIGIDBODY"]["SphereSize"].as<float>();
	m_bColScaleSize = _node["RIGIDBODY"]["ColScaleSize"].as<bool>();

	SetColliderFilter(Filter);
}


void CRigidBody::UpdateTransformData(COLLIDER_TYPE _eColliderType, bool _bKinematick, bool _bDinamic)
{
	// PhysData Position Setting
	RigidBody()->SetWorldPosition(Transform()->GetRelativePos());

	// PhysData Collider Setting
	m_eRigidColliderType = _eColliderType;
	switch (_eColliderType)
	{
	case COLLIDER_TYPE::COLLIDER_CUBE:
	{
		if (m_bColScaleSize)
			SetBoxCollider(Transform()->GetRelativeScale() * 0.5f);
		else
			SetBoxCollider(m_vBoxSize);
	}
		break;

	case COLLIDER_TYPE::COLLIDER_SPHERE:
	{
		if (m_bColScaleSize)
			SetSphereCollider(Transform()->GetRelativeScale().x * 0.5f);
		else
			SetSphereCollider(m_fShpereSize);
	}
	break;

	case COLLIDER_TYPE::COLLIDER_CAPSULE:
	{
		SetCapsuleCollider(m_vCapsuleSize.x, m_vCapsuleSize.y * 0.5f);
	}
	break;
	
	case COLLIDER_TYPE::COLLIDER_TRI:
	{
	}
	break;
	}

	// PhysData Rotation Setting
	SetWorldRotation(Transform()->GetRelativeRotation());

	// PhysData Kinematic Option Setting
	if (_bKinematick)
	{
		SetKinematicOption(true);
	}

	// PhysData Dynamic Option Setting
	if (_bDinamic)
	{
		SetDinamicOption(true);
	}

	// ToGroundRay Setting
	Vec3 vPos = Transform()->GetRelativePos();
	vPos.y -= Transform()->GetRelativeScale().y * 0.5f;
	vPos /= 100.f;
	m_pToGroundRay->SetStartOrigin(vPos.x, vPos.y, vPos.z);
	Vec3 vDir = Vec3(0.f, -1.f, 0.f).Normalize();
	m_pToGroundRay->SetDirection(vDir.x, vDir.y, vDir.z);
	m_pToGroundRay->SetMaxDistance(25.f / 100.f);
	m_pToGroundRay->SetQueryFilterData0(FILTER_GROUP::eGround);
}

bool CRigidBody::IsGround()
{
	return m_bGround;
}

Vec3 CRigidBody::GetHitNormal()
{
	return m_pToGroundRay->Hit.HitNormal;
}

void CRigidBody::SetWorldRotation(Vec3 _vWorldRot)
{
	DirectX::XMMATRIX _P;
	DirectX::XMMATRIX _Y;
	DirectX::XMMATRIX _R;
	if (m_eRigidColliderType == COLLIDER_TYPE::COLLIDER_CAPSULE)
	{
		_P = DirectX::XMMatrixRotationX(_vWorldRot.x);
		_Y = DirectX::XMMatrixRotationY(_vWorldRot.y);
		_R = DirectX::XMMatrixRotationZ(_vWorldRot.z + XM_PI * 0.5f);
	}
	else
	{
		_P = DirectX::XMMatrixRotationX(_vWorldRot.x);
		_Y = DirectX::XMMatrixRotationY(_vWorldRot.y);
		_R = DirectX::XMMatrixRotationZ(_vWorldRot.z);
	}
	
	Quaternion Q_Rot = SimpleMath::Quaternion::CreateFromRotationMatrix(_R * _Y * _P);
	
	m_pPhysData->Rotation = (Vec4)Q_Rot;
}

void CRigidBody::SetTriangleCollider(int _iIdxSize, int _iVertexSize, UINT* IdxArray, Vector3* _vecVertexList)
{
	m_pPhysData->mCollider->CreateTriangle(_iIdxSize, _iVertexSize, IdxArray, _vecVertexList);
}