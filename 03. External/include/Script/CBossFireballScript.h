#pragma once
#include "CMonsterScript.h"
class CBossFireballScript :
    public CMonsterScript
{
private:
    int         m_iSize;
    Vec3        m_vDir;
    Vec3        m_vBossPos;

public:
    void Fire(Vec3 _vPlayerPos);
    virtual void Parrying() override { m_eCurrentState = Monster_State::MISS; m_iMotion = 0; }
    void setBossPos(Vec3 _vPos) { m_vBossPos = _vPos + Vec3(0.f, 5.f, 0.f); }

public:
    virtual void begin() override;
    virtual void tick() override;

    virtual void BeginOverlap(CGameObject* _pOther) override;
    virtual void Overlap(CGameObject* _pOther) override;
    virtual void EndOverlap(CGameObject* _pOther) override;

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CBossFireballScript);
public:
    CBossFireballScript();
    ~CBossFireballScript();

};