#pragma once
#include <Engine/CScript.h>
class CLinkScript :
    public CScript
{
private:
    float m_fSpeed;

public:
    virtual void begin() override;
    virtual void tick() override;

    virtual void BeginOverlap(CCollider* _pOther) override;
    virtual void Overlap(CCollider* _pOther) override;
    virtual void EndOverlap(CCollider* _pOther) override;

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CLinkScript)

public:
    CLinkScript();
    ~CLinkScript();
};

