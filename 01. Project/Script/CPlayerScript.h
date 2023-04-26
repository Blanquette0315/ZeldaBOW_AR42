#pragma once

#include <Engine/CScript.h>

class CRayCastScript;

class CPlayerScript :
    public CScript
{
private:
    float   m_fAccTime;
    float   m_fSpeed;
    float   m_fJumpHeight;
    int     m_iTest;
    Vec2    m_v2Test;
    Vec3    m_v3Test;
    Vec4    m_v4Test;

    Ptr<CTexture> m_PTestTex;
    Ptr<CTexture> m_PTestTex1;
    Ptr<CPrefab> m_Prefab;
    Ptr<CPrefab> m_Prefab1;

    CRayCastScript* m_pRayScript;

    bool    m_bGround;
    bool    m_bJump;
    bool    m_bKeyRelease;
    bool    m_bWall;


public:
    virtual void begin() override;
    virtual void tick() override;

    virtual void BeginOverlap(CGameObject* _pOther) override;
    virtual void Overlap(CGameObject* _pOther) override;
    virtual void EndOverlap(CGameObject* _pOther) override;

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;


    CLONE(CPlayerScript);
public:
    CPlayerScript();
    ~CPlayerScript();
};