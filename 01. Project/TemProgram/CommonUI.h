#pragma once
#include "UI.h"

class CommonUI :
    public UI
{
private:
    static UINT iCommonUIID;
    static vector<bool> vecPickerChange;

public:
    static void vecPickerClear();

public:
    static void CreateColorPicker(Vec4& _vecColor);

    friend class CImGuiMgr;
};