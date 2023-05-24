#pragma once
#include "CMonsterScript.h"
class CBossScript :
    public CMonsterScript
{
private:
    int             m_iMaxHP;
    UINT            m_iAttackMotion;
    CGameObject*    m_pFireball;
    CGameObject*    m_pFlame;
    Ptr<CPrefab>    m_pFireball_small;
    Ptr<CPrefab>    m_pFireball_big;
    Ptr<CPrefab>    m_pBossHP;
    CGameObject*    m_pBossName;
    CGameObject*    m_pBossHPUI;
    CGameObject*    m_pBossHPMaxUI;

private:
    void Attack_Cross();
    void Attack_Rotate();
    void Attack_Straight();
    void Attack_Chemical();
    void Attack_Chemical_Big();

public:
    virtual void Damage(int _iNumber, Vec3 _vPos = Vec3(0.f, 0.f, 10000.f)) override;

public:
    virtual void begin() override;
    virtual void tick() override;

    virtual void BeginOverlap(CGameObject* _pOther) override;
    virtual void Overlap(CGameObject* _pOther) override;
    virtual void EndOverlap(CGameObject* _pOther) override;

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CBossScript);
public:
    CBossScript();
    ~CBossScript();
};