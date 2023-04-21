#pragma once
#include "CComponent.h"

class PhysData;
class CCollider :
    public CComponent
{
private:
    PhysData*       m_pPhysData;
    COLLIDER_TYPE   m_eType;
    Vec3            m_vOffsetPos;
    Vec3            m_vScale;
    Vec3            m_vRot;

    Vec3            m_vFinalPos;
    Vec3            m_vFinalRot;

    int             m_iOverlapCount;
public:
    void SetType(COLLIDER_TYPE _type) { m_eType = _type; }
    void SetOffsetPos(Vec3 _vOffset) { m_vOffsetPos = _vOffset; }
    void SetScale(Vec3 _vScale) { m_vScale = _vScale; }
    void SetRadius(float _fRadius) { m_vScale = Vec3(_fRadius, _fRadius, _fRadius); }
    void SetCapsuleSize(float _fRadius, float _fHeight) { m_vScale = Vec3(_fRadius, _fHeight, _fRadius); }

    void SetRotationX(float _fRadian) { m_vRot.x = _fRadian; }
    void SetRotationY(float _fRadian) { m_vRot.y = _fRadian; }
    void SetRotationZ(float _fRadian) { m_vRot.z = _fRadian; }

    float GetRotationX() { return m_vRot.x; }
    float GetRotationY() { return m_vRot.y; }
    float GetRotationZ() { return m_vRot.z; }

    void SetColliderType(COLLIDER_TYPE _type) { m_eType = _type; }
    COLLIDER_TYPE GetColliderType() { return m_eType; }

    Vec3 GetOffsetPos() { return m_vOffsetPos; }
    Vec3 GetScale() { return m_vScale; }
    Vec3 GetRotation() { return m_vRot; }

    Vec3 GetFinalPos() { return m_vFinalPos; }

    PhysData* GetPhysData() { return m_pPhysData; }

    void UpdatePhys();

    void CreateColliderActor();
    void DeleteColliderActor();
public:
    virtual void begin() override;
    virtual void finaltick() override;

public:
    void BeginOverlap(CCollider* _pOther);
    void Overlap(CCollider* _pOther);
    void EndOverlap(CCollider* _pOther);

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CCollider);
public:
    CCollider();
    CCollider(const CCollider& _origin);
    ~CCollider();
};

