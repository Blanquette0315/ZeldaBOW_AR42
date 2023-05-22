#pragma once
#include <Engine/CScript.h>
class CFlashEffectScript :
    public CScript
{
private:
    float m_fMaxTime;
    float m_fAccTime;

    Vec4 m_vStartColor;

    int m_iDestroyOption; //  0 : Destroy / 1 : Not Destroy

public:
    virtual void begin() override;
    virtual void tick() override;

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CFlashEffectScript)

public:
    CFlashEffectScript();
    CFlashEffectScript(const CFlashEffectScript& _origin);
    ~CFlashEffectScript();
};

