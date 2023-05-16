#pragma once
#include <Engine/CScript.h>

enum class LINK_HITSCR_TYPE
{
    HIT,
    JUST,
    END,
};

class CLinkHitScript :
    public CScript
{
    // save
private:
    int m_eType;

    // not save
private:


public:
    bool IsThisType(LINK_HITSCR_TYPE _eType) { return _eType == (LINK_HITSCR_TYPE)m_eType; }
    CGameObject* GetLink() { return GetOwner()->GetParent(); }

public:
    virtual void begin() override;
    virtual void tick() override;

    virtual void BeginOverlap(CGameObject* _pOther) override;
    virtual void Overlap(CGameObject* _pOther) override;
    virtual void EndOverlap(CGameObject* _pOther) override;

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CLinkHitScript)

public:
    CLinkHitScript();
    CLinkHitScript(const CLinkHitScript& _origin);
    ~CLinkHitScript();
};

