#pragma once
#include "UI.h"

class MenuUI :
    public UI
{
private:
    LEVEL_STATE m_ePrevState;

public:
    virtual void render() override;
    virtual void render_update() override;

public:
    MenuUI();
    ~MenuUI();
};

