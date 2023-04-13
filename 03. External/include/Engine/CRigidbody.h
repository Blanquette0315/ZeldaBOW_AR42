#pragma once
#include "CComponent.h"

#include <PhysEngine/Export/PhysX_API.h>

class CRigidBody :
    public CComponent
{
private:
    PhysData* const m_pPhysData;
    PhysRayCast* const m_pToGroundRay;
    COLLIDER_TYPE m_eRigidColliderType;
    Vec3 m_vVelocity;
    Vec3 m_vSaveVelocity;
    Vec3 m_vForce;
    bool m_bKeyRelease;
    Vec3 m_vBoxSize;
    float m_fShpereSize;
    Vec2 m_vCapsuleSize;
    bool m_bGround;
    bool m_bColScaleSize;

public:
    PhysData* GetRigidData() { return m_pPhysData; }

    void UpdateTransformData(COLLIDER_TYPE _eColliderType, bool _bKinematick, bool _bDinamic = false);
    void SetColliderType(COLLIDER_TYPE _eColliderType) { m_eRigidColliderType = _eColliderType; }
    COLLIDER_TYPE GetRigidColliderType() { return m_eRigidColliderType; }
    void CreateActor() { PhysX_Create_Actor(m_pPhysData); }
    void SetKeyRelease(bool _b) { m_bKeyRelease = _b; }
    void SetGround(bool _bGround) { m_bGround = _bGround; }
    bool IsGround();

    // Hypothalamic Normal Vector Check
    Vec3 GetHitNormal();

    // ===== Collider Filter Setting =====
    void SetColliderFilter(FILTER_GROUP _eGroup) { m_pPhysData->SetFilterData0(_eGroup); }
    void SetColliderFilter(uint32_t _Filter) { m_pPhysData->SetFilterData0(_Filter); }

    // ===== PhysX Data Setting =====
    // -- Mass Setting --
    // Mass Setting
    void SetMass(float _Mass) { m_pPhysData->MT_Mass = _Mass; }
    float GetMass() { return m_pPhysData->MT_Mass; }
    // -- Phys Mtrl Setting --
    void SetStaticFriction(float _fStaticFriction) { m_pPhysData->mMeterial->MT_StaticFriction = _fStaticFriction; }
    void SetDynamicFriction(float _fDynamicFriction) { m_pPhysData->mMeterial->MT_DynamicFriction = _fDynamicFriction; }
    void SetRestitution(float _fRestitution) { m_pPhysData->mMeterial->MT_Restitution = _fRestitution; }
    float GetStaticFricttion() { return m_pPhysData->mMeterial->MT_StaticFriction; }
    float GetDynamicFriction() { return m_pPhysData->mMeterial->MT_DynamicFriction; }
    float GetRestitution() { return m_pPhysData->mMeterial->MT_Restitution; }


    // -- Transform Data Setting --
    // World Pos Setting
    // warning!: Position about Engine to PhysX 1/100
    void SetWorldPosition(float _x, float _y, float _z) { m_pPhysData->SetWorldPosition(_x / 100.f, _y / 100.f, _z / 100.f); }
    void SetWorldPosition(Vec3 _vWorldPos) { m_pPhysData->SetWorldPosition(_vWorldPos.x / 100.f, _vWorldPos.y / 100.f, _vWorldPos.z / 100.f); }

    Vec3 GetWorldPosition() { Vec3 WorldPos = m_pPhysData->WorldPosition; return Vec3(WorldPos.x * 100.f, WorldPos.y * 100.f, WorldPos.z * 100.f); }

    // Local Pos Setting (when hierarchical) : Temp
    void SetLocalPosition(float _x, float _y, float _z) { m_pPhysData->SetLocalPosition(_x / 100.f, _y / 100.f, _z / 100.f); }
    void SetLocalPosition(Vec3 _LocalPos) { m_pPhysData->SetLocalPosition(_LocalPos.x / 100.f, _LocalPos.y / 100.f, _LocalPos.z / 100.f); }

    // Rotation Setting
    void SetWorldRotation(float _x, float _y, float _z) { m_pPhysData->SetRotation(_x, _y, _z); }
    void SetWorldRotation(Vec3 _vWorldPos);
    // Rotate Setting : Origin Rotation + Rotate
    /*void SetRotate(float _x, float _y, float _z) { m_pPhysData->SetRotate(_x, _y, _z); }
    void SetRotate(Vec3 _vRot) { m_pPhysData->SetRotate(_vRot.x, _vRot.y, _vRot.z); }*/
    
    Vec4 GetWorldRoation() { return m_pPhysData->Rotation; }
    
    // Center Point Setting
    void SetCenterPoint(float _x, float _y, float _z) { m_pPhysData->CenterPoint = Vec3(_x, _y, _z); }
    void SetCenterPoint(Vec3 _vPoint) { m_pPhysData->CenterPoint = Vec3(_vPoint.x, _vPoint.y, _vPoint.z); }
    Vec3 GetCenterPoint() { return m_pPhysData->CenterPoint; }

    // Velocity Setting
    void SetVelocity(float _x, float _y, float _z) { m_vVelocity = Vec3(_x, _y, _z); }
    void SetVelocity(Vec3 _vVelocity) { m_vVelocity = _vVelocity; }
    void AddVelocity(float _x, float _y, float _z) { m_vVelocity += Vec3(_x, _y, _z); }
    void AddVelocity(Vec3 _vVelocity) { m_vVelocity += Vec3(_vVelocity.x, _vVelocity.y, _vVelocity.z); }

    Vec3 GetVelocity() { return m_vVelocity; }
    Vec3 GetSaveVelocity() { return m_vSaveVelocity; }

    // Get direction of movement on Physx side
    //Vec3 GetPxVeloctiy() { return m_pPhysData->GetVelocity(); }

    // Force Setting
    void SetForce(float _x, float _y, float _z) { m_vForce = Vec3(_x, _y, _z); }
    void SetForce(Vec3 _vForce) { m_vForce = _vForce; }
    void AddForce(float _x, float _y, float _z) { m_vForce += Vec3(_x, _y, _z); }
    void AddForce(Vec3 _vForce) { m_vForce += _vForce; }

    Vec3 GetForce() { return m_vForce; }

    // ===== PhysX Data Option =====
    // Gravity Option Setting
    void SetGravityOption(bool _bGravity) { m_pPhysData->isGravity = _bGravity; }
    bool GetGravityOption() { return m_pPhysData->isGravity; }

    // Kinematic OPtion Setting
    void SetKinematicOption(bool _bKinematic) { m_pPhysData->isKinematic = _bKinematic; }
    bool GetKinematicOption() { return m_pPhysData->isKinematic; }

    // Dinamic, Static Option Setting
    void SetDinamicOption(bool _bDynamic) { m_pPhysData->isDinamic = _bDynamic; }
    bool GetDinamicOption() { return m_pPhysData->isDinamic; }

    // LockAxis_Position Setting
    void SetLockAxis_Pos(bool _x, bool _y, bool _z) { m_pPhysData->SetLockAxis_Position(_x, _y, _z); }
    Vec3 GetLockAxis_Pos() { return m_pPhysData->GetLockAxis_Position(); }

    // LockAxis_Rotation Setting
    void SetLockAxis_Rot(bool _x, bool _y, bool _z) { m_pPhysData->SetLockAxis_Rotation(_x, _y, _z); }
    Vec3 GetLockAxis_Rot() { return m_pPhysData->GetLockAxis_Rotation(); }

    // ===== RigidBody Collider Setting =====
    void SetColldierScaleSize(bool _bYes) { m_bColScaleSize = _bYes; }
    bool GetColliderScaleSize() { return m_bColScaleSize; }

    void SetBoxCollider(float _fSizex, float _fSizey, float _fSizez) { m_pPhysData->mCollider->SetBoxCollider(_fSizex / 100.f, _fSizey / 100.f, _fSizez / 100.f); }
    void SetBoxCollider(Vec3 _vSize) { m_pPhysData->mCollider->SetBoxCollider(_vSize.x / 100.f, _vSize.y / 100.f, _vSize.z / 100.f); }
    void SetBoxCollider(float _fRadius) { m_pPhysData->mCollider->SetBoxCollider(_fRadius / 100.f); }

    void SetBoxSize(Vec3 _vSize) { m_vBoxSize = _vSize; }
    void SetBoxSize(float _x, float _y, float _z) { m_vBoxSize = Vec3(_x, _y, _z); }
    void SetBoxSize(float _fRadius) { m_vBoxSize = Vec3(_fRadius, _fRadius, _fRadius); }
    Vec3 GetBoxSize() { return m_vBoxSize; }

    void SetSphereCollider(float _fRadius) { m_pPhysData->mCollider->SetSphereCollider(_fRadius / 100.f); }
    
    void SetSphereSize(float _fRadius) { m_fShpereSize = _fRadius; }
    float GetSphereSize() { return  m_fShpereSize; }

    void SetCapsuleCollider(float _fRadius, float _fHeight) { m_pPhysData->mCollider->SetCapsuleCollider(_fRadius / 100.f, _fHeight / 100.f); }
    void SetCapsuleHeight(float _fHeight) { m_vCapsuleSize.y = _fHeight; }
    void SetCapsuleRadius(float _fRadius) { m_vCapsuleSize.x = _fRadius; }
    void SetCapsuleSize(float _fRadius, float _fHeight) { m_vCapsuleSize = Vec2(_fRadius, _fHeight); }
    void SetCapsuleSize(Vec2 _vSize) { m_vCapsuleSize = _vSize; }
    Vec2 GetCapsuleSize() { return m_vCapsuleSize; }

    void SetTriangleCollider(int _iIdxSize, int _iVertexSize, UINT* IdxArray, Vector3* _vecVertexList, Vector3 _Size);

public:
    virtual void begin() override;
    virtual void tick() override;
    virtual void finaltick() override;
    void render();

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CRigidBody);
public:
    CRigidBody();
    CRigidBody(const CRigidBody& _origin);
    ~CRigidBody();
};