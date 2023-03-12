#pragma once

#include <Engine/CScript.h>
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


public:
    virtual void begin() override;
    virtual void tick() override;

    virtual void BeginOverlap(CCollider* _pOther) override;
    virtual void Overlap(CCollider* _pOther) override;
    virtual void EndOverlap(CCollider* _pOther) override;

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;


    CLONE(CPlayerScript);
public:
    CPlayerScript();
    ~CPlayerScript();
};