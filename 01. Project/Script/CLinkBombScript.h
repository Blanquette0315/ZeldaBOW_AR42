#pragma once
#include <Engine/CScript.h>
class CLinkBombScript :
    public CScript
{
    // save
private:
    float m_fSpeed;
    float m_fMaxLifeTime;


    // not save
private:
    Vec3 m_vDir;

    bool m_bOnce;


    float m_fAccLifeTime;



public:
    void SetDir(Vec3 _vDir) { m_vDir = _vDir; }

public:
    virtual void begin() override;
    virtual void tick() override;

    virtual void BeginOverlap(CGameObject* _pOther) override;
    virtual void Overlap(CGameObject* _pOther) override;
    virtual void EndOverlap(CGameObject* _pOther) override;

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CLinkBombScript)

public:
    CLinkBombScript();
    CLinkBombScript(const CLinkBombScript& _origin);
    ~CLinkBombScript();
};

