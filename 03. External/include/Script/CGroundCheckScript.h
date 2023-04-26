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
    virtual void BeginOverlap(CGameObject* _pOther) override;
    virtual void Overlap(CGameObject* _pOther) override;
    virtual void EndOverlap(CGameObject* _pOther) override;

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CGroundCheckScript)
public:
    CGroundCheckScript();
    CGroundCheckScript(const CGroundCheckScript& _origin);
    ~CGroundCheckScript();

};

