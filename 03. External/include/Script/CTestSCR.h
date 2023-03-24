#pragma once
#include <Engine/CScript.h>
class CTestSCR :
    public CScript
{
private:

public:
    virtual void tick() override;


    CLONE(CTestSCR)
public:
    CTestSCR();
    CTestSCR(const CTestSCR& _origin);
    ~CTestSCR();
};

