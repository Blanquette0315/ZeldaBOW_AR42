#pragma once
#include "ResUI.h"

class PreFabUI :
    public ResUI
{
private:
    virtual void update() override;
    virtual void render_update() override;

public:
    PreFabUI();
    ~PreFabUI();
};