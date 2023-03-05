#pragma once
#include <Engine/CScript.h>
class CCameraScript :
    public CScript
{
private:
    float   m_fSpeed;
    bool    m_bCamMove;

public:
    void SetCamMove(bool _IsMove) { m_bCamMove = _IsMove; }

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

