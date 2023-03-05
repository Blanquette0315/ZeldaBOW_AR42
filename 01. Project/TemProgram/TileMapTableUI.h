#pragma once
#include "UI.h"
class TileMapTableUI :
    public UI
{
private:
    vector<bool> m_selected;
    vector<int>  m_SelectedIdx;

    Vec2         m_vIdxCount;

    UI*          m_SBCInst;
    FUNC_1       m_SBCFunc;

public:
    void SetIdxCount(Vec2 _IdxCount);

    void AddSetClicked(UI* _Inst, FUNC_1 _Func)
    {
        m_SBCInst = _Inst;

        m_SBCFunc = _Func;
    }

public:
    virtual void update() override;
    virtual void render_update() override;

public:
    TileMapTableUI(string _UIName);
    ~TileMapTableUI();
};

