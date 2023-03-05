#pragma once
#include "ComponentUI.h"


class Light3DUI :
    public ComponentUI
{
private:
    tLightInfo m_tLightInfo;

    Vec4 m_vDiff;
    Vec4 m_vSpec;
    Vec4 m_Emb;

    float m_fRadius;
    float m_fInAngle;
    float m_fOutAngle;

    LIGHT_TYPE m_eLightType;
    int m_iCurItem;

public:
    virtual void update() override;
    virtual void render_update() override;

public:
    Light3DUI();
    ~Light3DUI();
};