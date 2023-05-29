#pragma once
#include <Engine/CScript.h>

class CLinkAnimScript;

class CTrailEffectScript :
    public CScript
{
private:
    Vec3                m_vFirstDotOffset;  // far
    Vec3                m_vSecondDotOffset; // near

    CLinkAnimScript*    m_pLinkAnimScr;

public:
    virtual void begin() override;
    virtual void tick() override;

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CTrailEffectScript);
public:
    CTrailEffectScript();
    CTrailEffectScript(const CTrailEffectScript& _origin);
    ~CTrailEffectScript();
};

