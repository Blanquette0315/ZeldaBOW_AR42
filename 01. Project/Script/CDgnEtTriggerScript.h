#pragma once
#include <Engine/CScript.h>

class CDgnEtTriggerScript :
    public CScript
{
private:
    bool m_bAnimPlay;

public:
    virtual void begin() override;
    virtual void tick() override;

    virtual void BeginOverlap(CGameObject* _pOther) override;
    virtual void Overlap(CGameObject* _pOther) override;
    virtual void EndOverlap(CGameObject* _pOther) override;

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CDgnEtTriggerScript)

public:
    CDgnEtTriggerScript();
    ~CDgnEtTriggerScript();
};