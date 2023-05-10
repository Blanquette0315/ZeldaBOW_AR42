#pragma once
#include <Engine/CScript.h>
class CMonWeaponScript :
    public CScript
{
private:
    UINT                m_iDamage;
    bool                m_bReady;

public:
    void setDamage(UINT _iDamage) { m_iDamage = _iDamage; }
    void setReady(bool _bReady) { m_bReady = _bReady; }

public:
    virtual void tick() override;

    virtual void BeginOverlap(CGameObject* _pOther) override;
    virtual void Overlap(CGameObject* _pOther) override;
    virtual void EndOverlap(CGameObject* _pOther) override;

    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CMonWeaponScript);
public:
    CMonWeaponScript();
    ~CMonWeaponScript();
};

