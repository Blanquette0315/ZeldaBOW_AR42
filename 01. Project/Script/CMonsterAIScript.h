#pragma once
#include <Engine/CScript.h>

class CMonsterAIScript :
    public CScript
{
private:
    CGameObject* m_pPlayer;
    float m_fAttackRange;
    float m_fFindOutRange;
    Vec3 m_vInitialPos;
    bool m_bFind;
    bool m_bActDone;

public:
    void Done(bool _done = true) { m_bActDone = _done; }
    Vec3 FindPlayerDir();
    Vec3 FindInitialPosDir();
    float GetPlayerDistance();
    Vec3 FindPlayerPos() { return m_pPlayer->Transform()->GetRelativePos(); }
    CGameObject* GetPlayer() { return m_pPlayer; }

public:
    virtual void begin() override;
    virtual void tick() override;

    virtual void BeginOverlap(CGameObject* _pOther) override;
    virtual void Overlap(CGameObject* _pOther) override;
    virtual void EndOverlap(CGameObject* _pOther) override;

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;


    CLONE(CMonsterAIScript);
public:
    CMonsterAIScript();
    ~CMonsterAIScript();
};

