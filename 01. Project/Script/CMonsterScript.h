#pragma once
#include <Engine/CScript.h>
#include "CScriptMgr.h"
#include "CMonsterAIScript.h"
#define AI GetOwner()->GetChildObject()[0]->GetScript<CMonsterAIScript>()

enum class Monster_State
{
    IDLE,
    WALK,
    RUN,
    FIND,
    ATTACK,
    RETURN,
};

class CMonsterScript :
    public CScript
{
protected:
    UINT                m_iHP;
    Monster_State       m_eCurrentState;
    float               m_fAcctime;
    float               m_fSpeed;
    int                 m_iMotion;
    Vec3                m_vFront;

public:
    void setState(Monster_State _eState) { m_eCurrentState = _eState; }

public:
    virtual void tick() override {};

    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CMonsterScript);
public:
    CMonsterScript(int _iScriptType = MONSTERSCRIPT);
    ~CMonsterScript();
};

