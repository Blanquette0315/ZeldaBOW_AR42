#pragma once

#include <Engine/CScript.h>
class CPlayerScript :
    public CScript
{
private:
    float   m_fAccTime;
    float   m_fSpeed;
    float   m_fJumpHeight;

    Ptr<CPrefab> m_Prefab;


public:
    virtual void begin() override;
    virtual void tick() override;

    virtual void BeginOverlap(CCollider2D* _pOther) override;
    virtual void Overlap(CCollider2D* _pOther) override;
    virtual void EndOverlap(CCollider2D* _pOther) override;

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;


    CLONE(CPlayerScript);
public:
    CPlayerScript();
    ~CPlayerScript();
};