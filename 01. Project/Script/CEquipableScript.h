#pragma once

#include <Engine/CScript.h>

class CGameObject;

class CEquipableScript :
    public CScript
{

    // not save
private:
    CGameObject*    m_pOwner;
    int             m_iOption;

public:
    void SetOption(int _iOption) { m_iOption = _iOption; }

public:
    virtual void begin() override;
    virtual void tick() override;
    virtual void finaltick() override;
    

    virtual void BeginOverlap(CGameObject* _pOther) override;
    virtual void Overlap(CGameObject* _pOther) override;
    virtual void EndOverlap(CGameObject* _pOther) override;

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CEquipableScript)

public:
    CEquipableScript();
    CEquipableScript(const CEquipableScript& _origin);
    ~CEquipableScript();
};

