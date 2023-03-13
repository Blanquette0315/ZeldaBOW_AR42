#pragma once
#include "CComponent.h"

#include <PhysEngine/Export/PhysX_API.h>

enum class RIGIDCOLLIDER_TYPE
{
    CUBE,
    SPHERE,
    CAPSULE,
    TRIANGLE,
};

class CRigidBody :
    public CComponent
{
private:
    PhysData* const m_pPhysData;
    Vec3 m_vVelocity;
    Vec3 m_vForce;
    bool m_bKeyRelease;

public:
    PhysData* GetRigidData() { return m_pPhysData; }

    void UpdateTransformData(RIGIDCOLLIDER_TYPE _eColliderType, bool _bKinematick, bool _bDinamic = false);
    void CreateActor() { PhysX_Create_Actor(m_pPhysData); }
    void SetKeyRelease(bool _b) { m_bKeyRelease = _b; }

    // ===== ������ ������ ���� =====
    // -- ���� ���� --
    // ���� ����
    void SetMass(float _Mass) { m_pPhysData->MT_Mass = _Mass; }
    float GetMass() { return m_pPhysData->MT_Mass; }
    // -- ���� ���� --
    void SetStaticFriction(float _fStaticFriction) { m_pPhysData->mMeterial->MT_StaticFriction = _fStaticFriction; }
    void SetDynamicFriction(float _fDynamicFriction) { m_pPhysData->mMeterial->MT_DynamicFriction = _fDynamicFriction; }
    void SetRestitution(float _fRestitution) { m_pPhysData->mMeterial->MT_Restitution = _fRestitution; }
    float GetStaticFricttion() { return m_pPhysData->mMeterial->MT_StaticFriction; }
    float GetDynamicFriction() { return m_pPhysData->mMeterial->MT_DynamicFriction; }
    float GetRestitution() { return m_pPhysData->mMeterial->MT_Restitution; }


    // -- ��ġ ������ ���� --
    // World Pos ����
    // Position ������ �츮�� 1px�� 100m�� �� ��. 1/100
    void SetWorldPosition(float _x, float _y, float _z) { m_pPhysData->SetWorldPosition(_x / 100.f, _y / 100.f, _z / 100.f); }
    void SetWorldPosition(Vec3 _vWorldPos) { m_pPhysData->SetWorldPosition(_vWorldPos.x / 100.f, _vWorldPos.y / 100.f, _vWorldPos.z / 100.f); }

    Vec3 GetWorldPosition() { Vec3 WorldPos = m_pPhysData->WorldPosition; return Vec3(WorldPos.x * 100.f, WorldPos.y * 100.f, WorldPos.z * 100.f); }

    // Local Pos ���� ( ���� ������ ��) : �켱�� Setting �� ��. �������� �Լ��� ����.
    void SetLocalPosition(float _x, float _y, float _z) { m_pPhysData->SetLocalPosition(_x / 100.f, _y / 100.f, _z / 100.f); }
    void SetLocalPosition(Vec3 _LocalPos) { m_pPhysData->SetLocalPosition(_LocalPos.x / 100.f, _LocalPos.y / 100.f, _LocalPos.z / 100.f); }

    // Rotation ����
    void SetWorldRotation(float _x, float _y, float _z) { m_pPhysData->SetRotation(_x, _y, _z); }
    void SetWorldRotation(Vec3 _vWorldPos);
    // Rotate ���� : ���� �����̼ǿ� +���ִ� ��
    void SetRotate(float _x, float _y, float _z) { m_pPhysData->SetRotate(_x, _y, _z); }
    void SetRotate(Vec3 _vRot) { m_pPhysData->SetRotate(_vRot.x, _vRot.y, _vRot.z); }
    // SimpleMath quaternion�� Ȯ���� ������.
    Vec4 GetWorldRoation() { return m_pPhysData->Rotation; }
    
    // �ʱ� ��ġ���� Center Point ����
    //void SetCenterPoint(float _x, float _y, float _z) { m_pPhysData->CenterPoint = Vec3(_x, _y, _z); }
    //void SetCenterPoint(Vec3);

    // Velocity ����
    void SetVelocity(float _x, float _y, float _z) { m_vVelocity = Vec3(_x, _y, _z); }
    void SetVelocity(Vec3 _vVelocity) { m_vVelocity = _vVelocity; }
    void AddVelocity(float _x, float _y, float _z) { m_vVelocity += Vec3(_x, _y, _z); }
    void AddVelocity(Vec3 _vVelocity) { m_vVelocity += Vec3(_vVelocity.x, _vVelocity.y, _vVelocity.z); }

    // Physx���� �̵����� ��������
    Vec3 GetPxVeloctiy() { return m_pPhysData->DVelocity; }//m_pPhysData->GetVelocity(); }

    // Force ����
    void AddForce(float _x, float _y, float _z) { m_vForce = Vec3(_x, _y, _z); }
    void AddForce(Vec3 _vForce) { m_pPhysData->AddForce(_vForce.x, _vForce.y, _vForce.z); }

    // ===== ������ ������ �ɼ� (��ü �ɼ�) =====
    // �߷� ���� ���� ����
    void SetGravityOption(bool _bGravity) { m_pPhysData->isGravity = _bGravity; }
    bool GetGravityOption() { return m_pPhysData->isGravity; }

    // Kinematic ���� ����
    void SetKinematicOption(bool _bKinematic) { m_pPhysData->isKinematic = _bKinematic; }
    bool GetKinematicOption() { return m_pPhysData->isKinematic; }

    // �����̴� ��ü���� Dinamic, Static ����
    void SetDinamicOption(bool _bDynamic) { m_pPhysData->isDinamic = _bDynamic; }
    bool GetDinamicOption() { return m_pPhysData->isDinamic; }

    // ���ϴ� �� �����̵� ��� ����
    void SetLockAxis_Pos(bool _x, bool _y, bool _z) { m_pPhysData->SetLockAxis_Position(_x, _y, _z); }
    Vec3 GetLockAxis_Pos() { return m_pPhysData->GetLockAxis_Position(); }

    // ���ϴ� �� ����ȸ�� ��� ����
    void SetLockAxis_Rot(bool _x, bool _y, bool _z) { m_pPhysData->SetLockAxis_Rotation(_x, _y, _z); }
    Vec3 GetLockAxis_Rot() { return m_pPhysData->GetLockAxis_Rotation(); }

    // ===== ��ü �ݶ��̴� ���� =====
    void SetBoxCollider(float _fSizex, float _fSizey, float _fSizez) { m_pPhysData->mCollider->SetBoxCollider(_fSizex / 100.f, _fSizey / 100.f, _fSizez / 100.f); }
    void SetBoxCollider(Vec3 _vSize) { m_pPhysData->mCollider->SetBoxCollider(_vSize.x / 100.f, _vSize.y / 100.f, _vSize.z / 100.f); }
    void SetBoxCollider(float _fRadius) { m_pPhysData->mCollider->SetBoxCollider(_fRadius / 100.f); }

    void SetSphereCollider(float _fRadius) { m_pPhysData->mCollider->SetSphereCollider(_fRadius / 100.f); }
    
    void SetCapsuleCollider(float _fRadius, float _fHeight) { m_pPhysData->mCollider->SetCapsuleCollider(_fRadius / 100.f, _fHeight / 100.f); }

    void SetTriangleCollider(int _iIdxSize, int _iVertexSize, UINT* IdxArray, Vector3* _vecVertexList);

public:
    virtual void tick() override;
    virtual void finaltick() override;
    void render();

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CRigidBody);
public:
    CRigidBody();
    ~CRigidBody();
};