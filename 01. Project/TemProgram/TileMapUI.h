#pragma once
#include "ComponentUI.h"


class TileMapUI :
    public ComponentUI
{
private:
    string          m_TexName;
    vector<tTile>   m_vecTile;
    Vec2            m_TexWH;
    int             m_TileCount;
    int             m_TileIdx[2];
    int             m_CurIdx[2];


    bool            m_bFillOnce;
    int             m_StartIdx[2];
    int             m_EndIdx[2];

    bool            m_bFillSelectIdx;
    Vec2            m_SelectIdxLeftTop;

public:
    void SetAtlasTex(DWORD_PTR _strTextureKey);

    void SetLeftTop(DWORD_PTR _Left, DWORD_PTR _Top);

    void FillSelectIdx(DWORD_PTR _vecIdx);

public:
    virtual void update() override;
    virtual void render_update() override;

public:
    TileMapUI();
    ~TileMapUI();
};

