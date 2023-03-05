#pragma once
#include "CScript.h"
class CCameraScript :
    public CScript
{
private:
    float m_fSpeed;

public:
    virtual void begin() override;
    virtual void tick() override;

private:
    void Move();

    CLONE(CCameraScript);
public:
    CCameraScript();
    ~CCameraScript();
};

