#pragma once
#include "ComponentUI.h"
class LandScapeUI :
    public ComponentUI
{
private:
    int            m_iXFaceCount;
    int            m_iZFaceCount;
    int            m_iMod;

    Ptr<CTexture>  m_pBrushTex;
    Vec2           m_vBrushScale;
    bool           m_bCurDataCooking;

public:
    void SetTexture(DWORD_PTR _strTexKey);

public:
    virtual void update() override;
    virtual void render_update() override;

public:
    LandScapeUI();
    ~LandScapeUI();
};