#pragma once
#include "UI.h"

#include <Engine/ptr.h>
#include <Engine/CTexture.h>

#include "UIdefine.h"

class CTexture;

class TextureUI :
    public UI
{
private:
    Ptr<CTexture>   m_pTexture;
    TEXUI_TYPE      m_eTexUIType;

    Vec2            m_vTexWH;

    UI*             m_DBCInst;
    FUNC_2          m_DBCFunc;
    UI*             m_OBCInst;
    FUNC_2          m_OBCFunc;

    // Animation
    Vec2           m_vFullSize;
    Vec2           m_vSlicePixel;
    Vec2           m_vLeftTopUV;
    Vec2           m_vSliceUV;

    Vec2           m_vTemLeftTop;
    Vec2           m_vTemRigthBot;
    Vec2           m_vSliceLeftTop;
    Vec2           m_vSliceRightBot;
    ImVec2         m_vAnimCursor;

public:
    void SetTexture(Ptr<CTexture> _Tex) { m_pTexture = _Tex; }
    void SetTextureWH(Vec2 _WH) { m_vTexWH = _WH; }
    void SetTexUIType(TEXUI_TYPE _eType) { m_eTexUIType = _eType; }
    void SetFullSize(Vec2 _FullSize) { m_vFullSize = _FullSize; }

    void AddDynamicDBClicked(UI* _Inst, FUNC_2 _Func)
    {
        m_DBCInst = _Inst;
        m_DBCFunc = _Func;
    }

    void AddDynamicOBClicked(UI* _Inst, FUNC_2 _Func)
    {
        m_OBCInst = _Inst;
        m_OBCFunc = _Func;
    }

public:
    virtual void update() override;
    virtual void render_update() override;

public:
    TextureUI();
    ~TextureUI();
};

