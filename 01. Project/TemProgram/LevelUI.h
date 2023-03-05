#pragma once
#include "UI.h"

#include <Engine/CLevel.h>

class LevelUI :
    public UI
{
private:
    CLevel* m_TargetLevel; // 해당 UI가 어떤 Level을 참조하고 있는지 주소를 지닌다.

public:
    void SetTarget(CLevel* _pTarget) { m_TargetLevel = _pTarget; }
    CLevel* GetTarget() { return m_TargetLevel; }

    virtual void render_update() override;

public:
    LevelUI(const string& _UIName);
    ~LevelUI();
};

