#pragma once
#include "ComponentUI.h"
class Collider2DUI :
    public ComponentUI
{
private:
    Vec3 m_vOffSet;
    Vec3 m_vScale;
    Vec3 m_vRot;

    COLLIDER2D_TYPE m_eType;
    int m_iCurItem;

    bool m_bIgnorObjectScale;

public:
    virtual void update() override;
    virtual void render_update() override;

public:
    Collider2DUI();
    ~Collider2DUI();
};

