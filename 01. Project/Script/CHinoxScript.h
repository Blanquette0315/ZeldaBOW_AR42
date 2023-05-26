#pragma once
#include "CMonsterScript.h"

enum class Hinox_State
{
    SLEEP,
    WAKE,
    SEARCH_TREE,
    NONE,
};

class CHinoxScript :
    public CMonsterScript
{
private:
    int                 m_iMaxHP;
    Hinox_State         m_eHinoxState;
    bool                m_bGetTree;
    float               m_fGuardtime;
    int                 m_iGuardMotion;
    bool                m_bEyeGuard;
    Ptr<CPrefab>        m_pBossHP;
    CGameObject*        m_pBossName;
    CGameObject*        m_pBossHPUI;
    CGameObject*        m_pBossHPMaxUI;

private:
    void SearchTree();
    void GuardReset();

public:
    virtual void Damage(int _iNumber, Vec3 _vPos = Vec3(0.f, 0.f, 10000.f)) override;
    virtual void Parrying() override {}

public:
    virtual void begin() override;
    virtual void tick() override;

    virtual void BeginOverlap(CGameObject* _pOther) override;
    virtual void Overlap(CGameObject* _pOther) override;
    virtual void EndOverlap(CGameObject* _pOther) override;

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;


    CLONE(CHinoxScript);
public:
    CHinoxScript();
    //CHinoxScript(const CHinoxScript& _origin);
    ~CHinoxScript();
};