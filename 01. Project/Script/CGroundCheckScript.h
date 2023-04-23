#pragma once
#include <Engine/CScript.h>
class CGroundCheckScript :
    public CScript
{
private:
    bool m_bIsGround;

public:
    bool IsGround() { return m_bIsGround; }

public:
    virtual void tick() override;
    virtual void begin() override;

public:
    virtual void BeginOverlap(CCollider* _pOther) override;
    virtual void Overlap(CCollider* _pOther) override;
    virtual void EndOverlap(CCollider* _pOther) override;

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CGroundCheckScript)
public:
    CGroundCheckScript();
    CGroundCheckScript(const CGroundCheckScript& _origin);
    ~CGroundCheckScript();

};

