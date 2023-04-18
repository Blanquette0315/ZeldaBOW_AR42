#include "pch.h"
#include "CRigidBody.h"

#include "CLevelMgr.h"
#include "CPhysMgr.h"
#include "CTransform.h"
#include "CMeshRender.h"
#include "CLandScape.h"

CRigidBody::CRigidBody()
	: CComponent(COMPONENT_TYPE::RIGIDBODY)
	, m_pToGroundRay(new PhysRayCast)
	, m_eRigidColliderType(COLLIDER_TYPE::COLLIDER_CUBE)
	, m_bKeyRelease(false)
	, m_vCapsuleSize(Vec2(1.f, 2.f))
	, m_fShpereSize(1.f)
	, m_bGround(false)
	, m_bColScaleSize(false)
	, m_bMeshCollider(false)
	, m_bCreateActor(false)
{
	// base is m_vecPhysData[0]
	m_vecPhysData.push_back(new PhysData);
}

CRigidBody::CRigidBody(const CRigidBody& _origin)
	: CComponent(COMPONENT_TYPE::RIGIDBODY)
	, m_pToGroundRay(new PhysRayCast)
	, m_eRigidColliderType(_origin.m_eRigidColliderType)
	, m_bKeyRelease(false)
	, m_vCapsuleSize(_origin.m_vCapsuleSize)
	, m_fShpereSize(_origin.m_fShpereSize)
	, m_vColOffSet(_origin.m_vColOffSet)
	, m_bGround(false)
	, m_bColScaleSize(false)
	, m_bMeshCollider(_origin.m_bMeshCollider)
	, m_bCreateActor(false)
{
	// base is m_vecPhysData[0]
	m_vecPhysData.push_back(new PhysData);
	m_vecPhysData[0]->CenterPoint = _origin.m_vecPhysData[0]->CenterPoint;

	m_vecPhysData[0]->isDinamic = _origin.m_vecPhysData[0]->isDinamic;
	m_vecPhysData[0]->isGravity = _origin.m_vecPhysData[0]->isGravity;
	m_vecPhysData[0]->isKinematic = _origin.m_vecPhysData[0]->isKinematic;
	m_vecPhysData[0]->mMeterial->MT_DynamicFriction = _origin.m_vecPhysData[0]->mMeterial->MT_DynamicFriction;
	m_vecPhysData[0]->mMeterial->MT_Restitution = _origin.m_vecPhysData[0]->mMeterial->MT_Restitution;
	m_vecPhysData[0]->mMeterial->MT_StaticFriction = _origin.m_vecPhysData[0]->mMeterial->MT_StaticFriction;
	m_vecPhysData[0]->MT_Mass = _origin.m_vecPhysData[0]->MT_Mass;
	m_vecPhysData[0]->m_vPxLinearVelocity = _origin.m_vecPhysData[0]->m_vPxLinearVelocity;

	m_vecPhysData[0]->Rotation = _origin.m_vecPhysData[0]->Rotation;
	m_vecPhysData[0]->WorldPosition = _origin.m_vecPhysData[0]->WorldPosition;
}

CRigidBody::~CRigidBody()
{
	if (m_bCreateActor)
	{
		for (size_t i = 0; i < m_vecPhysData.size(); ++i)
		{
			if (nullptr != m_vecPhysData[i])
			{
				PhysX_Delete_Actor(m_vecPhysData[i]);
			}
		}
		//Safe_Del_Vec<PhysData>(m_vecPhysData);
	}
	else
	{
		Safe_Del_Vec<PhysData>(m_vecPhysData);
	}
	
	if (nullptr != m_pToGroundRay)
	{
		delete m_pToGroundRay;
	}
}

void CRigidBody::begin()
{
	m_vecPhysData[0]->BOWObj = GetOwner();
	if (nullptr != LandScape())
	{
		//LandScape()->LandStreamOut();
		LandScape()->CreateActor();
	}
	
	UpdateTransformData(m_eRigidColliderType, m_vecPhysData[0]->isKinematic, m_vecPhysData[0]->isDinamic);

	// Update PhysData vector
	UpdatePhysDataVec();

	CreateActor();
}

void CRigidBody::tick()
{
	if (CLevelMgr::GetInst()->GetLevelState() == LEVEL_STATE::PLAY)
	{
		UpdateActor();

		// Reflect simulation results to Transform
		Vec3 vPos = GetWorldPosition();
		vPos -= m_vColOffSet;
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

		if (m_bGround)
		{
			if (m_vVelocity != Vec3(0.f, 0.f, 0.f)) 
			{
				m_vecPhysData[0]->SetVelocity(m_vVelocity.x, m_vVelocity.y, m_vVelocity.z);
				m_vSaveVelocity = m_vVelocity;
				m_vVelocity = Vec3(0.f, 0.f, 0.f);
			}
			else
			{
				if (m_bKeyRelease)
				{
					m_vecPhysData[0]->SetVelocity(0.f, 0.f, 0.f);
					m_vVelocity = Vec3(0.f, 0.f, 0.f);
					m_vSaveVelocity = m_vVelocity;
					m_bKeyRelease = false;
				}
			}
		}
		else
		{
			if (m_vecPhysData[0]->m_vPxLinearVelocity.y != 0.f)
			{
				if (m_vSaveVelocity != Vec3(0.f, 0.f, 0.f))
				{
					m_vecPhysData[0]->SetVelocity(m_vSaveVelocity.x, m_vecPhysData[0]->m_vPxLinearVelocity.y, m_vSaveVelocity.z);
					m_vSaveVelocity = Vec3(0.f, 0.f, 0.f);
				}
			}
		}

		// ...
		// will be added Force
		m_vecPhysData[0]->AddForce(m_vForce.x, m_vForce.y, m_vForce.z);
		m_vForce = Vec3(0.f, 0.f, 0.f);
		// ...
	}

	// DebugDraw
#ifdef _DEBUG
	CallDebugDraw();
#endif
}

void CRigidBody::render()
{
}


void CRigidBody::SaveToYAML(YAML::Emitter& _emitter)
{
	_emitter << YAML::Key << "RIGIDBODY";
	_emitter << YAML::Value << YAML::BeginMap;

	_emitter << YAML::Key << "PhysDataMass";
	_emitter << YAML::Value << m_vecPhysData[0]->MT_Mass;
	_emitter << YAML::Key << "PhysDataStaticFriction";
	_emitter << YAML::Value << m_vecPhysData[0]->mMeterial->MT_StaticFriction;
	_emitter << YAML::Key << "PhysDataDynamicFriction";
	_emitter << YAML::Value << m_vecPhysData[0]->mMeterial->MT_DynamicFriction;
	_emitter << YAML::Key << "PhysDataRestitution";
	_emitter << YAML::Value << m_vecPhysData[0]->mMeterial->MT_Restitution;
	_emitter << YAML::Key << "PhysDataGravity";
	_emitter << YAML::Value << m_vecPhysData[0]->isGravity;
	_emitter << YAML::Key << "PhysDataKinematic";
	_emitter << YAML::Value << m_vecPhysData[0]->isKinematic;
	_emitter << YAML::Key << "PhysDataDynamic";
	_emitter << YAML::Value << m_vecPhysData[0]->isDinamic;
	_emitter << YAML::Key << "PhysDataLockAxis_Position";
	_emitter << YAML::Value << m_vecPhysData[0]->GetLockAxis_Position();
	_emitter << YAML::Key << "PhysDataLockAxis_Rotation";
	_emitter << YAML::Value << m_vecPhysData[0]->GetLockAxis_Rotation();
	_emitter << YAML::Key << "PhysDataColliderFilter";
	_emitter << YAML::Value << m_vecPhysData[0]->GetFilterData0();

	_emitter << YAML::Key << "PhysDataColliderType";
	_emitter << YAML::Value << (UINT)m_eRigidColliderType;
	_emitter << YAML::Key << "CapsuleSize";
	_emitter << YAML::Value << m_vCapsuleSize;
	_emitter << YAML::Key << "BoxSize";
	_emitter << YAML::Value << m_vBoxSize;
	_emitter << YAML::Key << "SphereSize";
	_emitter << YAML::Value << m_fShpereSize;
	_emitter << YAML::Key << "ColScaleSize";
	_emitter << YAML::Value << m_bColScaleSize;
	_emitter << YAML::Key << "MeshColllider";
	_emitter << YAML::Value << m_bMeshCollider;
	_emitter << YAML::Key << "ColOffSet";
	_emitter << YAML::Value << m_vColOffSet;

	_emitter << YAML::EndMap;
}

void CRigidBody::LoadFromYAML(YAML::Node& _node)
{
	m_vecPhysData[0]->MT_Mass = _node["RIGIDBODY"]["PhysDataMass"].as<float>();
	m_vecPhysData[0]->mMeterial->MT_StaticFriction = _node["RIGIDBODY"]["PhysDataStaticFriction"].as<float>();
	m_vecPhysData[0]->mMeterial->MT_DynamicFriction = _node["RIGIDBODY"]["PhysDataDynamicFriction"].as<float>();
	m_vecPhysData[0]->mMeterial->MT_Restitution = _node["RIGIDBODY"]["PhysDataRestitution"].as<float>();
	m_vecPhysData[0]->isGravity = _node["RIGIDBODY"]["PhysDataGravity"].as<bool>();
	m_vecPhysData[0]->isKinematic = _node["RIGIDBODY"]["PhysDataKinematic"].as<bool>();
	m_vecPhysData[0]->isDinamic = _node["RIGIDBODY"]["PhysDataDynamic"].as<bool>();
	Vec3 LockAxisPos = _node["RIGIDBODY"]["PhysDataLockAxis_Position"].as<Vec3>();
	m_vecPhysData[0]->SetLockAxis_Position((bool)LockAxisPos.x, (bool)LockAxisPos.y, (bool)LockAxisPos.z);
	Vec3 LocakAxisRot = _node["RIGIDBODY"]["PhysDataLockAxis_Rotation"].as<Vec3>();
	m_vecPhysData[0]->SetLockAxis_Rotation((bool)LocakAxisRot.x, (bool)LocakAxisRot.y, (bool)LocakAxisRot.z);
	
	m_eRigidColliderType = (COLLIDER_TYPE)(_node["RIGIDBODY"]["PhysDataColliderType"].as<UINT>());
	uint32_t Filter = _node["RIGIDBODY"]["PhysDataColliderFilter"].as<uint32_t>();
	m_vCapsuleSize = _node["RIGIDBODY"]["CapsuleSize"].as<Vec2>();
	m_vBoxSize = _node["RIGIDBODY"]["BoxSize"].as<Vec3>();
	m_fShpereSize = _node["RIGIDBODY"]["SphereSize"].as<float>();
	m_bColScaleSize = _node["RIGIDBODY"]["ColScaleSize"].as<bool>();
	m_bMeshCollider = _node["RIGIDBODY"]["MeshColllider"].as<bool>();
	m_vColOffSet = _node["RIGIDBODY"]["ColOffSet"].as<Vec3>();

	SetColliderFilter(Filter);
}


void CRigidBody::UpdateTransformData(COLLIDER_TYPE _eColliderType, bool _bKinematick, bool _bDinamic)
{
	// PhysData Position Setting
	Vec3 vFinalPos = Transform()->GetRelativePos();
	vFinalPos += m_vColOffSet;
		
	RigidBody()->SetWorldPosition(vFinalPos);

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

	case COLLIDER_TYPE::COLLIDER_MESH:
	{
		SetMeshCollider();
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

void CRigidBody::CreateActor()
{
	for (size_t i = 0; i < m_vecPhysData.size(); ++i)
	{
		if (nullptr != m_vecPhysData[i])
		{
			PhysX_Create_Actor(m_vecPhysData[i]);
		}
	}

	m_bCreateActor = true;
}

void CRigidBody::UpdateActor()
{
	for (size_t i = 0; i < m_vecPhysData.size(); ++i)
	{
		if (nullptr != m_vecPhysData[i])
		{
			PhysX_Update_Actor(m_vecPhysData[i]);
		}
	}
}

void CRigidBody::UpdatePhysDataVec()
{
	for (size_t i = 1; i < m_vecPhysData.size(); ++i)
	{
		m_vecPhysData[i]->BOWObj = m_vecPhysData[0]->BOWObj;
		m_vecPhysData[i]->CenterPoint = m_vecPhysData[0]->CenterPoint;
		m_vecPhysData[i]->Enter_Count = m_vecPhysData[0]->Enter_Count;
		m_vecPhysData[i]->Exit_Count = m_vecPhysData[0]->Exit_Count;

		m_vecPhysData[i]->isDinamic = m_vecPhysData[0]->isDinamic;
		m_vecPhysData[i]->isGravity = m_vecPhysData[0]->isGravity;
		m_vecPhysData[i]->isKinematic = m_vecPhysData[0]->isKinematic;
		m_vecPhysData[i]->mMeterial->MT_DynamicFriction = m_vecPhysData[0]->mMeterial->MT_DynamicFriction;
		m_vecPhysData[i]->mMeterial->MT_Restitution = m_vecPhysData[0]->mMeterial->MT_Restitution;
		m_vecPhysData[i]->mMeterial->MT_StaticFriction = m_vecPhysData[0]->mMeterial->MT_StaticFriction;
		m_vecPhysData[i]->MT_Mass = m_vecPhysData[0]->MT_Mass;
		m_vecPhysData[i]->m_vPxLinearVelocity = m_vecPhysData[0]->m_vPxLinearVelocity;
		m_vecPhysData[i]->Rotation = m_vecPhysData[0]->Rotation;
		m_vecPhysData[i]->WorldPosition = m_vecPhysData[0]->WorldPosition;
	}
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
	
	m_vecPhysData[0]->Rotation = (Vec4)Q_Rot;
}

void CRigidBody::SetTriangleCollider(int _iIdxSize, int _iVertexSize, UINT* IdxArray, Vector3* _vecVertexList, Vector3 _Size)
{
	Vec3 PxSize = _Size;
	PxSize.x /= 100.f;
	PxSize.y /= 100.f;
	PxSize.z /= 100.f;

	m_vecPhysData[0]->mCollider->CreateTriangle(_iIdxSize, _iVertexSize, IdxArray, _vecVertexList, PxSize);
}

void CRigidBody::SetMeshCollider()
{
	Ptr<CMesh> pMesh = MeshRender()->GetMesh();
	Vtx* pVtx = pMesh->GetVtxSysMem();

	int iAllVertexCount = pMesh->GetAllVtxCount();
	Vector3* vecVertexList = new Vector3[iAllVertexCount];

	for (int i = 0; i < iAllVertexCount; ++i)
	{
		vecVertexList[i].x = pVtx[i].vPos.x; // *(-1.f);
		vecVertexList[i].y = pVtx[i].vPos.z;
		vecVertexList[i].z = pVtx[i].vPos.y * (-1.f);
	}

	UINT iSubsetCount = pMesh->GetSubsetCount();
	vector<tIndexInfo> vecIndexInfo = pMesh->GetvecIndexInfo();

	UINT iAllIdxCount = 0;
	for (UINT i = 0; i < iSubsetCount; ++i)
	{
		iAllIdxCount += vecIndexInfo[i].iIdxCount;
	}
	
	UINT* IdxArray = new UINT[iAllIdxCount];
	UINT IdxCount = 0;

	Vec3 PxSize = Vec3(10.f, 10.f, 10.f);
	PxSize.x /= 100.f;
	PxSize.y /= 100.f;
	PxSize.z /= 100.f;

	if (10 < iSubsetCount)
	{
		int iCount = (int)ceil((float)iSubsetCount / 10.f);

		for (int i = 0; i < iCount; ++i)
		{
			if (0 != i)
			{
				m_vecPhysData.push_back(new PhysData);
			}

			IdxCount = 0;

			for (UINT j = 0; j < 10; ++j)
			{
				if (j + (i * 10) >= iSubsetCount)
					continue;
				
				UINT* IndexSysMem = (UINT*)vecIndexInfo[j + (i * 10)].pIdxSysMem;
				for (UINT k = 0; k < vecIndexInfo[j + (i * 10)].iIdxCount; ++k)
				{
					IdxArray[k + IdxCount] = (UINT)IndexSysMem[k];
				}
				IdxCount += vecIndexInfo[j + (i * 10)].iIdxCount;
			}

			m_vecPhysData[i]->mCollider->CreateTriangle(IdxCount, iAllVertexCount, IdxArray, vecVertexList, PxSize);
		}
	}
	else
	{
		IdxCount = 0;

		for (UINT i = 0; i < iSubsetCount; ++i)
		{
			UINT* IndexSysMem = (UINT*)vecIndexInfo[i].pIdxSysMem;
			for (UINT j = 0; j < vecIndexInfo[i].iIdxCount; ++j)
			{
				IdxArray[j + IdxCount] = (UINT)IndexSysMem[j];
			}

			IdxCount += vecIndexInfo[i].iIdxCount;
		}

		m_vecPhysData[0]->mCollider->CreateTriangle(iAllIdxCount, iAllVertexCount, IdxArray, vecVertexList, PxSize);
	}

	m_bMeshCollider = true;

	delete[] IdxArray;
	delete[] vecVertexList;
}

void CRigidBody::CallDebugDraw()
{
	Vec4 vColor = Vec4(1.f, 1.f, 0.f, 1.f);

	//FinalPos
	Vec3 vObjectPos = Transform()->GetWorldPos();
	Vec3 vColliderWorldOffset = XMVector3TransformNormal(m_vColOffSet, Transform()->GetRotMat());
	Vec3 vFinalPos = vColliderWorldOffset + vObjectPos;

	Vec3 vRot = {};
	QuaternionToEuler(GetWorldRoation(), vRot);

	Matrix matRot = XMMatrixRotationX(vRot.x);
	matRot *= XMMatrixRotationY(vRot.y);
	if (m_eRigidColliderType != COLLIDER_TYPE::COLLIDER_CAPSULE)
		matRot *= XMMatrixRotationZ(vRot.z);
	else
		matRot *= XMMatrixRotationZ(vRot.z + XM_PI * 0.5f);
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

	Vec3 vFinalRot = {};
	QuaternionToEuler(Q_Rot, vFinalRot);

	if (COLLIDER_TYPE::COLLIDER_CUBE == m_eRigidColliderType)
	{
		DebugDrawCube(vColor, vFinalPos, m_vBoxSize, vFinalRot);
	}
	else if (COLLIDER_TYPE::COLLIDER_SPHERE == m_eRigidColliderType)
	{
		DebugDrawSphere(vColor, vFinalPos, m_fShpereSize);
	}
	else if (COLLIDER_TYPE::COLLIDER_CAPSULE == m_eRigidColliderType)
	{
		if (CLevelMgr::GetInst()->GetLevelState() == LEVEL_STATE::PLAY)
			DebugDrawCylinder(vColor, vFinalPos, Vec3(m_vCapsuleSize.x * 2.f, m_vCapsuleSize.x * 2.f + m_vCapsuleSize.y, m_vCapsuleSize.x * 2.f), vFinalRot);
		else
			DebugDrawCylinder(vColor, vFinalPos, Vec3(m_vCapsuleSize.x * 2.f, m_vCapsuleSize.x * 2.f + m_vCapsuleSize.y, m_vCapsuleSize.x * 2.f), vFinalRot - Vec3(0.f, 0.f, XM_PI * 0.5f));
	}
}