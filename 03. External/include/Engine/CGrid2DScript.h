#pragma once
#include "CScript.h"

class CCamera;

class CGrid2DScript :
    public CScript
{
private:
    CCamera*    m_pMainCam;

    Vec4        m_vColor;
    float       m_fThickness;
    float       m_fGridInterval;

public:
    virtual void begin() override;
    virtual void tick() override;

public:
    void SetMainCam(CCamera* _pCam) { m_pMainCam = _pCam; }
    void SetGridColor(Vec4 _vColor) { m_vColor = _vColor; }
    void SetThickness(float _fThickness) { m_fThickness = _fThickness; }
    void SetGridInterval(float _fGridInterval) { m_fGridInterval = _fGridInterval; }

    CLONE(CGrid2DScript);
public:
    CGrid2DScript();
    ~CGrid2DScript();
};

