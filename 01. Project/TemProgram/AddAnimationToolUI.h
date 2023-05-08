#pragma once
#include "UI.h"

class ComboBoxUI;

class AddAnimationToolUI :
    public UI
{
private:
    char m_FBXName[256];
    ComboBoxUI* m_ComoboBox;

public:
    virtual void Open() override;
    virtual void update() override;
    virtual void render_update() override;

public:
    AddAnimationToolUI();
    ~AddAnimationToolUI();
};

