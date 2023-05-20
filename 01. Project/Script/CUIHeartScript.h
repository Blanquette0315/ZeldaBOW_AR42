#pragma once
#include "CUIScript.h"

class CLinkAnimScript;

class CUIHeartScript :
    public CUIScript
{
private:
    CLinkAnimScript*        m_pLink;
    vector<CGameObject*>    m_vecHeartObj;

    bool                    m_bReflect;
    
public:
    void ReflectHP(int _iHP);
    void SetReflect(bool _b) { m_bReflect = _b; }

public:
    virtual void begin() override;
    virtual void tick() override;

    virtual void BeginOverlap(CGameObject* _pOther) override;
    virtual void Overlap(CGameObject* _pOther) override;
    virtual void EndOverlap(CGameObject* _pOther) override;

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CUIHeartScript)

public:
    CUIHeartScript();
    CUIHeartScript(const CUIHeartScript& _origin);
    ~CUIHeartScript();
};

