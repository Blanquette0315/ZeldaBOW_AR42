#pragma once
#include <Engine/CScript.h>

class CLinkAnimScript;
class CPrefab;
class CGameObject;

class CLinkBowScript :
    public CScript
{
    // not save
private:
    CLinkAnimScript* m_pLinkAnimScr;
    Ptr<CPrefab> m_pArrowPref;
    CGameObject* m_pArrowObj;

public:
    virtual void begin() override;
    virtual void tick() override;

    virtual void BeginOverlap(CGameObject* _pOther) override;
    virtual void Overlap(CGameObject* _pOther) override;
    virtual void EndOverlap(CGameObject* _pOther) override;

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CLinkBowScript)

public:
    CLinkBowScript();
    CLinkBowScript(const CLinkBowScript& _origin);
    ~CLinkBowScript();
};

