#pragma once
#include "CRenderComponent.h"

#include "Ptr.h"

class CStructuredBuffer;

class CTileMap :
    public CRenderComponent
{
private:
    Ptr<CTexture>           m_AtlasTex;     // Ÿ�ϸ� ��Ʋ�� �̹���
    Vec2                    m_TexWH;
    Vec2                    m_vTileCount;   // Ÿ�ϸ� ���� ���� ����

    vector<tTile>           m_vecTile;      
    CStructuredBuffer*      m_TileBuffer;   // �� Ÿ�Ϻ� ��Ʋ�� ����

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