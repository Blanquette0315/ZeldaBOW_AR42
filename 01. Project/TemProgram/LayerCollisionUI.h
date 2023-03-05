#pragma once
#include "UI.h"
class LayerCollisionUI :
    public UI
{
private:
    WORD		    m_matrix[MAX_LAYER];
    CLevel*         m_TargetLevel;
    bool            m_vecSelected[MAX_LAYER][MAX_LAYER];
    bool            m_bChanged;

public:
    void SetTargetLevel(CLevel* _TargetLevel) { m_TargetLevel = _TargetLevel; }
    void ReSet() { m_bChanged = true; }

public:
    virtual void update() override;
    virtual void render_update() override;

public:
    LayerCollisionUI(string _UIName);
    ~LayerCollisionUI();
};

