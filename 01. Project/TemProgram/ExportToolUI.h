#pragma once
#include "UI.h"

class ListUI_EX;

class ExportToolUI :
    public UI
{
private:
    ListUI_EX* m_List;

public:
    virtual void Open() override;
    virtual void update() override;
    virtual void render_update() override;

private:
    void ExportSelectedObject(DWORD_PTR _selectedIdx);

public:
    ExportToolUI();
    ~ExportToolUI();
};

