#pragma once
#include "ComponentUI.h"

class RigidBodyUI :
    public ComponentUI
{
private:
    float           m_fMass;
    float           m_fStaticFriction;
    float           m_fDynamicFriction;
    float           m_fRestitution;

    Vec3            m_vCenterPoint;
    bool            m_bGravity;
    bool            m_bKinematic;
    bool            m_bDynamic;

    bool            m_arrLockAxisPos[3];
    bool            m_arrLockAxisRot[3];

    COLLIDER_TYPE   m_eColliderType;
    int             m_iCurItem;
    bool            m_bColScaleSize;
    Vec3            m_vBoxSize;
    float           m_fSphereSize;
    Vec2            m_vCapsuleSize;
    Vec3            m_vColOffSet;

    Vec3            m_vVelocity;
    Vec3            m_vSaveVelocity;
    Vec3            m_vForce;

public:

public:
    virtual void update() override;
    virtual void render_update() override;

public:
    RigidBodyUI();
    ~RigidBodyUI();
};