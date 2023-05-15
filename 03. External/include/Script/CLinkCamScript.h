#pragma once
#include "CCamScript.h"

enum class LINK_CAM_MODE
{
    GENERAL,
    GENERAL_START,
    LOCKON,
    LOCKON_START,
    BOW_CHARGE_START,
    BOW_CHARGE,
    END,
};

class CGameObject;

class CLinkAnimScript;

class CLinkCamScript :
    public CCamScript
{
    // save
private:
    float               m_fCameraRadiusMax;    // camera radius
    float               m_fCameraRadiusMin;    // camera radius min

    float               m_fMaxDistLockOn;      // begin : take LockOnObj's collider radius

    float               m_fFrontDistFromLink;  // front dist from link that assumed as link's position. 

    float               m_fPosUp; // additional offset Y

    float               m_fLockOnMaxTime;
    float               m_fGeneralMaxTime;
    float               m_fBowMaxTime;
    // not save
private:
    CLinkAnimScript*    m_pLinkScr;
    CGameObject*        m_pLockOnObj;

    LINK_CAM_MODE       m_eMode;
    float               m_fDistLockonFromLink;

    bool                m_bLockOn;
    bool                m_bLockOnStart;

    float               m_fLockOnAccTime;

    float               m_fGeneralAccTime;

    float               m_fBowAccTime;

    bool                m_bOnce;

    Vec3                m_vLerpStartPos;
    Vec3                m_vLerpStartRot;
    Vec3                m_vLerpEndRot;
    float               m_fLerpStartRadius; // radius

    float               m_fCameraRadiusCur;

    Vec3                m_vAddRot;

public:
    void GeneralMove();
    void GeneralStartMove();
    void LockOnMove();
    void LockOnStartMove(); // rotate + zoom
    void BowChargeMove();
    void BowChargeStartMove();

public:
    void SetLockOnObj(CGameObject* _pLockOnObj) { m_pLockOnObj = _pLockOnObj; }
    void ClearLockOnObj() { m_pLockOnObj = nullptr; }

    void SetLockOnStart(bool _b) { m_bLockOnStart = _b; }
    void SetMode(LINK_CAM_MODE _eMode) { m_eMode = _eMode; }

private:
    void DetermineRotDir(const Vec3& _vStart, const Vec3& _vEnd, Vec3& _vOutStart, Vec3& _vOutEnd);

private:
    // set rotation
    void RBTNMove(bool _bRestrictX = true, float _fMaxX = XM_2PI / 6.f, float _fMinX = 0.f, bool _bRestrictY = false, float _fMaxY = XM_2PI, float _fMinY = 0.f);
    // add rotation
    void LBTNMove(bool _bRestrictX = true, float _fMaxX = XM_2PI / 6.f, float _fMinX = 0.f, bool _bRestrictY = false, float _fMaxY = XM_2PI, float _fMinY = 0.f);
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

