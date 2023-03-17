#pragma once
#include <Engine/CScript.h>

class NavTestSCR :
    public CScript
{
private:
    float m_fSpeed;

public:
    virtual void tick() override;


    CLONE(NavTestSCR)
public:
    NavTestSCR();
    NavTestSCR(const NavTestSCR& _origin);
    ~NavTestSCR();
};

