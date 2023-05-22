#pragma once
#include <Engine/CScript.h>

enum class SCALER_OPT
{
    START_END,
    START_MIDDLE_END,
    ONLY_TIMER,
};

class CEffectScaler :
    public CScript
{
    Vec3 m_vScaleStart;
    Vec3 m_vScaleMiddle;
    Vec3 m_vScaleEnd;

    float m_fMiddleTime;
    float m_fMiddleDelayTime;
    float m_fDelayAccTIme;
    float m_fMaxTime;
    float m_fAccTime;

    bool m_bDelayFlag;
    bool m_bDelay;

    int m_iOption;     // 0 : Start - End, 1: Start - Middle - End 2 : Timer
    int m_iLerpOption; // 0 : default, 1 : cos, 2 : circle

private:
    void SelectLerpType(float& _fRatio);

    void Start_End();
    void Start_Middle_End();
    void Timer();

public:
    virtual void begin() override;
    virtual void tick() override;

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CEffectScaler);
public:
    CEffectScaler();
    CEffectScaler(const CEffectScaler& _origin);
    ~CEffectScaler();
};



