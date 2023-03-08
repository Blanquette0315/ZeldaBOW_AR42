#pragma once
#include "UI.h"

#include <Engine/CLevel.h>

class LevelUI :
    public UI
{
private:
    CLevel* m_TargetLevel;              // if Selected Set CurLevel then m_pTargetLevel is CurLevel's Address
    const wstring* m_TargetLevelPath;   // Selected Target Level's Relative Path

    bool m_bUpdateOnce;
    string m_strLevelName;
    string m_strLevelPath;

    bool m_bIsCurLevel;

public:
    void SetTarget(const wstring* _pTargetLevelPath) { m_TargetLevelPath = _pTargetLevelPath; m_bUpdateOnce = false; }
    const wstring* GetTarget() { return m_TargetLevelPath; }

    virtual void update() override;
    virtual void render_update() override;

public:
    LevelUI(const string& _UIName);
    ~LevelUI();
};

