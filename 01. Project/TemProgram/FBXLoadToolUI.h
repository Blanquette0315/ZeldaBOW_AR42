#pragma once
#include "UI.h"
class FBXLoadToolUI :
    public UI
{
private:
    char    m_FBXName[256];
    int     m_iOption;

public:
    virtual void update() override;
    virtual void render_update() override;

public:
    FBXLoadToolUI();
    ~FBXLoadToolUI();
};

