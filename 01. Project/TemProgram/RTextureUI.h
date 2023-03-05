#pragma once
#include "ResUI.h"

class RTextureUI :
    public ResUI
{
private:
    virtual void update() override;
    virtual void render_update() override;

public:
    RTextureUI();
    ~RTextureUI();
};

