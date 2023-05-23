#pragma once
#include "ComponentUI.h"

class ListUI_EX;

class CameraUI :
    public ComponentUI
{
private:
    float       m_fFar;
    float       m_fNear;
    float       m_fFOV;
    float       m_fScale;
    PROJ_TYPE   m_eProjType;
    UINT        m_iVisibleLayer;
    int         m_iCamIdx;

    ListUI_EX*  m_uiList;

    bool        m_bInit;
    bool        m_bShowDebugDraw;

public:
    virtual void Open() override;
    virtual void update() override;
    virtual void render_update() override;

public:
    void RenderPerspective();
    void RenderOrtho();
    void RenderGeneral();

private:
    void SetLayerOnOff();

public:
    CameraUI();
    ~CameraUI();
};

