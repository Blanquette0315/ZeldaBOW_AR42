#pragma once
#include <Engine/CScript.h>

class CGameObject;

class CLockOnScript :
    public CScript
{
    // save
private:
    float           m_fColRadius;

    // not save
private:
    CGameObject*    m_pLink;
    CGameObject*    m_pLockOnTarget;
    float           m_fDistFromTarget;

public:
    CGameObject* GetLockOnTarget() { return m_pLockOnTarget; }

public:
    void ClearTarget() { m_pLockOnTarget = nullptr; m_fDistFromTarget = m_fColRadius; }

public:
    virtual void init() override;
    virtual void begin() override;
    virtual void tick() override;

    virtual void BeginOverlap(CCollider* _pOther) override;
    virtual void Overlap(CCollider* _pOther) override;
    virtual void EndOverlap(CCollider* _pOther) override;

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CLockOnScript)

public:
    CLockOnScript();
    CLockOnScript(const CLockOnScript& _origin);
    ~CLockOnScript();
};
