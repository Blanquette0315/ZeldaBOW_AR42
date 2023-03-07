#pragma once
#include "UI.h"

class ComboBoxUI;
class ListUI_EX;

class TestUI :
    public UI
{
public:
    ComboBoxUI* m_Combo;
    ListUI_EX* m_ListEX;

public:
    virtual void render_update() override;
    virtual void Open() override;
    virtual void Close() override;

    TestUI();
    ~TestUI();
};

