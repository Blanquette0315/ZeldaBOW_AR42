#pragma once
#include "CRenderComponent.h"

#include "Ptr.h"

class CStructuredBuffer;

class CTileMap :
    public CRenderComponent
{
private:
    Ptr<CTexture>           m_AtlasTex;     // 타일맵 아틀라스 이미지
    Vec2                    m_TexWH;
    Vec2                    m_vTileCount;   // 타일맵 가로 세로 개수

    vector<tTile>           m_vecTile;      
    CStructuredBuffer*      m_TileBuffer;   // 각 타일별 아틀라스 정보

    bool                    m_bDataChanged;

private:
    void Initialize();

public:
    void SetTileAtlas(Ptr<CTexture> _AtlasTex);
    void SetTileCount(UINT _iWidth, UINT _iHeight);
    void SetTileVector(const vector<tTile>& _vecTile) { m_vecTile = _vecTile; }

    Vec2 GetTileCount() { return m_vTileCount; }
    const wstring& GetTextureName();
    
    const vector<tTile>& GetTileVector() { return m_vecTile; }
    Vec2 GetTexWH() { return m_TexWH; }

public:
    virtual void finaltick() override;
    virtual void render() override;

public:
    virtual void SaveToFile(FILE* _File) override;
    virtual void LoadFromFile(FILE* _File) override;
    CLONE(CTileMap);
public:
    CTileMap();
    ~CTileMap();
};