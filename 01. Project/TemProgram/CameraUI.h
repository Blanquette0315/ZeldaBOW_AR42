#pragma once
#include "ComponentUI.h"

class CameraUI :
    public ComponentUI
{
private:
    float       m_fFar;
    float       m_fScale;
    PROJ_TYPE   m_eProjType;
    int         m_iCurItem;

public:
    virtual void update() override;
    virtual void render_update() override;

public:
    CameraUI();
    ~CameraUI();
};

