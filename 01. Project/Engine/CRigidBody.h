#pragma once
#include "CComponent.h"

#include <PhysEngine/Export/PhysX_API.h>

class CRigidBody :
    public CComponent
{
private:
    vector<PhysData*> m_vecPhysData;
    COLLIDER_TYPE m_eRigidColliderType;
    Vec3 m_vColOffSet;
    Vec3 m_vVelocity;
    Vec3 m_vSaveVelocity;
    Vec3 m_vForce;
    bool m_bKeyRelease;
    Vec3 m_vBoxSize;
    float m_fShpereSize;
    Vec2 m_vCapsuleSize;
    bool m_bGround;
    bool m_bColScaleSize;
    bool m_bMeshCollider;
    bool m_bCreateActor;
    bool m_bUsePhysRot;

public:
    vector<PhysData*> GetRigidData() { return m_vecPhysData; }

    void UpdateTransformData(COLLIDER_TYPE _eColliderType, bool _bKinematick, bool _bDinamic = false);
    void SetColliderType(COLLIDER_TYPE _eColliderType) { m_eRigidColliderType = _eColliderType; }
    COLLIDER_TYPE GetRigidColliderType() { return m_eRigidColliderType; }
    void CreateActor();
    void UpdateActor();
    void UpdatePhysResult();
    void UpdatePhysDataVec();
    void SetKeyRelease(bool _b) { m_bKeyRelease = _b; }
    void SetGround(bool _bGround) { m_bGround = _bGround; }
    bool IsGround();

    void SetColOffSet(Vec3 _vColOffSet) { m_vColOffSet = _vColOffSet; }
    Vec3 GetColOffSet() { return m_vColOffSet; }

    void SetUsePhysRot(bool _bUsePhysRot) { m_bUsePhysRot = _bUsePhysRot; }
    bool IsUsePhysRot() { return m_bUsePhysRot; }

    // ===== Collider Filter Setting =====
    void SetQueryColliderFilter(FILTER_GROUP _eGroup) { m_vecPhysData[0]->SetFilterData0(_eGroup); }
    void SetQueryColliderFilter(uint32_t _Filter) { m_vecPhysData[0]->SetFilterData0(_Filter); }
    uint32_t GetQueryColliderFilter() { return m_vecPhysData[0]->GetFilterData0(); }

    void SetQueryMskColFilter(FILTER_GROUP _eGroup) { m_vecPhysData[0]->SetFilterData1(_eGroup); }
    void SetQueryMskColFilter(uint32_t _Filter) { m_vecPhysData[0]->SetFilterData1(_Filter); }
    uint32_t GetQueryMskColFilter() { return m_vecPhysData[0]->GetFilterData1(); }

    void SetSimulationColFilter(FILTER_GROUP _eGroup) { m_vecPhysData[0]->SetSimulationFData0(_eGroup); }
    void SetSimulationColFilter(uint32_t _Filter) { m_vecPhysData[0]->SetSimulationFData0(_Filter); }
    uint32_t GetSimulationColFilter() { return m_vecPhysData[0]->GetSimulationFData0(); }

    void SetSimulationMskColFilter(FILTER_GROUP _eGroup) { m_vecPhysData[0]->SetSimulationFData1(_eGroup); }
    void SetSimulationMskColFilter(uint32_t _Filter) { m_vecPhysData[0]->SetSimulationFData1(_Filter); }
    uint32_t GetSimulationMskColFilter() { return m_vecPhysData[0]->GetSimulationFData1(); }

    // ===== PhysX Data Setting =====
    // -- Mass Setting --
    // Mass Setting
    void SetMass(float _Mass) { m_vecPhysData[0]->MT_Mass = _Mass; }
    float GetMass() { return m_vecPhysData[0]->MT_Mass; }
    // -- Phys Mtrl Setting --
    void SetStaticFriction(float _fStaticFriction) { m_vecPhysData[0]->mMeterial->MT_StaticFriction = _fStaticFriction; }
    void SetDynamicFriction(float _fDynamicFriction) { m_vecPhysData[0]->mMeterial->MT_DynamicFriction = _fDynamicFriction; }
    void SetRestitution(float _fRestitution) { m_vecPhysData[0]->mMeterial->MT_Restitution = _fRestitution; }
    float GetStaticFricttion() { return m_vecPhysData[0]->mMeterial->MT_StaticFriction; }
    float GetDynamicFriction() { return m_vecPhysData[0]->mMeterial->MT_DynamicFriction; }
    float GetRestitution() { return m_vecPhysData[0]->mMeterial->MT_Restitution; }


    // -- Transform Data Setting --
    // World Pos Setting
    // warning!: Position about Engine to PhysX 1/100
    void SetWorldPosition(float _x, float _y, float _z) { m_vecPhysData[0]->SetWorldPosition(_x / 100.f, _y / 100.f, _z / 100.f); }
    void SetWorldPosition(Vec3 _vWorldPos) { m_vecPhysData[0]->SetWorldPosition(_vWorldPos.x / 100.f, _vWorldPos.y / 100.f, _vWorldPos.z / 100.f); }

    Vec3 GetWorldPosition() { Vec3 WorldPos = m_vecPhysData[0]->WorldPosition; return Vec3(WorldPos.x * 100.f, WorldPos.y * 100.f, WorldPos.z * 100.f); }

    // Local Pos Setting (when hierarchical) : Temp
    void SetLocalPosition(float _x, float _y, float _z) { m_vecPhysData[0]->SetLocalPosition(_x / 100.f, _y / 100.f, _z / 100.f); }
    void SetLocalPosition(Vec3 _LocalPos) { m_vecPhysData[0]->SetLocalPosition(_LocalPos.x / 100.f, _LocalPos.y / 100.f, _LocalPos.z / 100.f); }

    // Rotation Setting
    void SetWorldRotation(float _x, float _y, float _z) { m_vecPhysData[0]->SetRotation(_x, _y, _z); }
    void SetWorldRotation(Vec3 _vWorldPos);
    // Rotate Setting : Origin Rotation + Rotate
    /*void SetRotate(float _x, float _y, float _z) { m_pPhysData->SetRotate(_x, _y, _z); }
    void SetRotate(Vec3 _vRot) { m_pPhysData->SetRotate(_vRot.x, _vRot.y, _vRot.z); }*/
    
    Vec4 GetWorldRoation() { return m_vecPhysData[0]->Rotation; }
    
    // Center Point Setting
    void SetCenterPoint(float _x, float _y, float _z) { m_vecPhysData[0]->CenterPoint = Vec3(_x, _y, _z); }
    void SetCenterPoint(Vec3 _vPoint) { m_vecPhysData[0]->CenterPoint = Vec3(_vPoint.x, _vPoint.y, _vPoint.z); }
    Vec3 GetCenterPoint() { return m_vecPhysData[0]->CenterPoint; }

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
    void SetGravityOption(bool _bGravity) { m_vecPhysData[0]->isGravity = _bGravity; }
    bool GetGravityOption() { return m_vecPhysData[0]->isGravity; }

    // Kinematic OPtion Setting
    void SetKinematicOption(bool _bKinematic) { m_vecPhysData[0]->isKinematic = _bKinematic; }
    bool GetKinematicOption() { return m_vecPhysData[0]->isKinematic; }

    // Dinamic, Static Option Setting
    void SetDinamicOption(bool _bDynamic) { m_vecPhysData[0]->isDinamic = _bDynamic; }
    bool GetDinamicOption() { return m_vecPhysData[0]->isDinamic; }

    // LockAxis_Position Setting
    void SetLockAxis_Pos(bool _x, bool _y, bool _z) { m_vecPhysData[0]->SetLockAxis_Position(_x, _y, _z); }
    Vec3 GetLockAxis_Pos() { return m_vecPhysData[0]->GetLockAxis_Position(); }

    // LockAxis_Rotation Setting
    void SetLockAxis_Rot(bool _x, bool _y, bool _z) { m_vecPhysData[0]->SetLockAxis_Rotation(_x, _y, _z); }
    Vec3 GetLockAxis_Rot() { return m_vecPhysData[0]->GetLockAxis_Rotation(); }

    // ===== RigidBody Collider Setting =====
    void SetColldierScaleSize(bool _bYes) { m_bColScaleSize = _bYes; }
    bool GetColliderScaleSize() { return m_bColScaleSize; }

    void SetBoxCollider(float _fSizex, float _fSizey, float _fSizez) { m_vecPhysData[0]->mCollider->SetBoxCollider(_fSizex / 100.f, _fSizey / 100.f, _fSizez / 100.f); }
    void SetBoxCollider(Vec3 _vSize) { m_vecPhysData[0]->mCollider->SetBoxCollider(_vSize.x / 100.f, _vSize.y / 100.f, _vSize.z / 100.f); }
    void SetBoxCollider(float _fRadius) { m_vecPhysData[0]->mCollider->SetBoxCollider(_fRadius / 100.f); }

    void SetBoxSize(Vec3 _vSize) { m_vBoxSize = _vSize; }
    void SetBoxSize(float _x, float _y, float _z) { m_vBoxSize = Vec3(_x, _y, _z); }
    void SetBoxSize(float _fRadius) { m_vBoxSize = Vec3(_fRadius, _fRadius, _fRadius); }
    Vec3 GetBoxSize() { return m_vBoxSize; }

    void SetSphereCollider(float _fRadius) { m_vecPhysData[0]->mCollider->SetSphereCollider(_fRadius / 100.f); }
    
    void SetSphereSize(float _fRadius) { m_fShpereSize = _fRadius; }
    float GetSphereSize() { return  m_fShpereSize; }

    void SetCapsuleCollider(float _fRadius, float _fHeight) { m_vecPhysData[0]->mCollider->SetCapsuleCollider(_fRadius / 100.f, _fHeight / 100.f); }
    void SetCapsuleHeight(float _fHeight) { m_vCapsuleSize.y = _fHeight; }
    void SetCapsuleRadius(float _fRadius) { m_vCapsuleSize.x = _fRadius; }
    void SetCapsuleSize(float _fRadius, float _fHeight) { m_vCapsuleSize = Vec2(_fRadius, _fHeight); }
    void SetCapsuleSize(Vec2 _vSize) { m_vCapsuleSize = _vSize; }
    Vec2 GetCapsuleSize() { return m_vCapsuleSize; }

    void SetTriangleCollider(int _iIdxSize, int _iVertexSize, UINT* IdxArray, Vector3* _vecVertexList, Vector3 _Size);
    
    void SetMeshCollider();
    void SetTerrainCollider(int _iVertexSize, Vector3* _vecVertexList, Vector3 _Size);

private:
    void CallDebugDraw();

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