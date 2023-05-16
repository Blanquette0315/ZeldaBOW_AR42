#pragma once
#include <Engine/CScript.h>
class CMonHeadScript :
    public CScript
{
private:
    float m_fRadius;

public:
    bool HeadCheck(Vec3 _vPos);
public:
    virtual void tick() override {};

    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CMonHeadScript);
public:
    CMonHeadScript();
    CMonHeadScript(const CMonHeadScript& _origin);
    ~CMonHeadScript();
};

