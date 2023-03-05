#pragma once
#include "ComponentUI.h"

class Light2DUI :
    public ComponentUI
{
private:
    tLightInfo m_tLightInfo;

    Vec4 m_vDiff;
    Vec4 m_vSpec;
    Vec4 m_Emb;

    float m_fRadius;
    float m_fAngle;

    LIGHT_TYPE m_eLightType;
    int m_iCurItem;

public:
    virtual void update() override;
    virtual void render_update() override;

public:
    Light2DUI();
    ~Light2DUI();
};

