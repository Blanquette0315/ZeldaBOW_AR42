#pragma once
#include <Engine/CScript.h>

class CNavTestSCR :
    public CScript
{
private:
    float m_fSpeed;
    bool m_bOnce;

public:
    virtual void tick() override;


    CLONE(CNavTestSCR)
public:
    CNavTestSCR();
    CNavTestSCR(const CNavTestSCR& _origin);
    ~CNavTestSCR();
};

