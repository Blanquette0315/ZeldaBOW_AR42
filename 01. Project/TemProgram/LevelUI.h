#pragma once
#include "UI.h"

#include <Engine/CLevel.h>

class LevelUI :
    public UI
{
private:
    CLevel* m_TargetLevel; // �ش� UI�� � Level�� �����ϰ� �ִ��� �ּҸ� ���Ѵ�.

public:
    void SetTarget(CLevel* _pTarget) { m_TargetLevel = _pTarget; }
    CLevel* GetTarget() { return m_TargetLevel; }

    virtual void render_update() override;

public:
    LevelUI(const string& _UIName);
    ~LevelUI();
};

