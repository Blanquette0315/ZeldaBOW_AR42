#pragma once
#include "UI.h"

class ComboBoxUI;

class AnimInjectionToolUI :
    public UI
{
private:
    char m_FBXPathBuf[256];
    ComboBoxUI* m_ComoboSrc;
    ComboBoxUI* m_ComoboTarget;

public:
    virtual void Open() override;
    virtual void update() override;
    virtual void render_update() override;

public:
    AnimInjectionToolUI();
    ~AnimInjectionToolUI();
};

