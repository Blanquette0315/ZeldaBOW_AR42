#pragma once
#include "CCamScript.h"

enum class LINK_CAM_MODE
{
    GENERAL,
    LOCKON,
    END,
};

class CGameObject;

class CLinkCamScript :
    public CCamScript
{
    // save
private:
    float           m_fDistFromLink;
    float           m_fDistFromLinkMin;
    float           m_fFrontDistFromLink;
    float           m_fSpeedLockOnZoomIn;
    // speed of camera rotation back to the Link
    float           m_fSpeedLockOnRad;

    float           m_fMaxDistLockOn;

    // not save
private:
    CGameObject*    m_pLinkObj;
    CGameObject*    m_pLockOnObj;

    LINK_CAM_MODE   m_eMode;
    float           m_fDistLockonFromLink;

    bool            m_bLockOn;
    bool            m_bLockOnStart;

public:
    void GeneralMove();

public:
    void SetLockOnObj(CGameObject* _pLockOnObj) { m_pLockOnObj = _pLockOnObj; }
    void ClearLockOnObj() { m_pLockOnObj = nullptr; }

public:
    virtual void init() override;
    virtual void begin() override;
    virtual void tick() override;

    virtual void BeginOverlap(CGameObject* _pOther) override;
    virtual void Overlap(CGameObject* _pOther) override;
    virtual void EndOverlap(CGameObject* _pOther) override;

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CLinkCamScript)

public:
    CLinkCamScript();
    CLinkCamScript(const CLinkCamScript& _origin);
    ~CLinkCamScript();
};

