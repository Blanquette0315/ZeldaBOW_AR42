#pragma once
#include "CScript.h"
class CMonsterScript :
    public CScript
{
private:
    CGameObject* m_pTargetObject;

public:
    virtual void tick() override;

    CLONE(CMonsterScript);
public:
    CMonsterScript();
    ~CMonsterScript();
};

