#pragma once
#include <Engine/CScript.h>

class CPTScript :
    public CScript
{
private:
    CGameObject*    m_pMainCam;
    bool            m_bPT;
    bool            m_bToneMapping;
    bool            m_bGamma;
    bool            m_bToonShading;
    bool            m_bShadow;

public:
    virtual void begin() override;
    virtual void tick() override;

    virtual void BeginOverlap(CGameObject* _pOther) override;
    virtual void Overlap(CGameObject* _pOther) override;
    virtual void EndOverlap(CGameObject* _pOther) override;

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;


    CLONE(CPTScript);
public:
    CPTScript();
    ~CPTScript();
};