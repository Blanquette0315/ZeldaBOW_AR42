#pragma once
#include "ComponentUI.h"
class ColliderUI :
    public ComponentUI
{
private:
    Vec3 m_vOffSet;
    Vec3 m_vScale;
    Vec3 m_vRot;

    float m_fRadius;
    Vec2 m_vCapsuleSize;

    COLLIDER_TYPE m_eType;
    int m_iCurItem;

    bool m_bDebugDraw;
public:
    virtual void update() override;
    virtual void render_update() override;

public:
    ColliderUI();
    ~ColliderUI();
};

