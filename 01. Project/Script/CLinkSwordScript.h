#pragma once
#include <Engine/CScript.h>

class CGameObject;
class CLinkAnimScript;

class CLinkSwordScript :
    public CScript
{
    // not save
private:
    CLinkAnimScript* m_pLinkAnimScr;
    vector<CGameObject*> m_vecObjHit;

private:
    bool IsAttackAnim();

public:
    virtual void begin() override;
    virtual void tick() override;

    virtual void BeginOverlap(CGameObject* _pOther) override;
    virtual void Overlap(CGameObject* _pOther) override;
    virtual void EndOverlap(CGameObject* _pOther) override;

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CLinkSwordScript)

public:
    CLinkSwordScript();
    ~CLinkSwordScript();
};

