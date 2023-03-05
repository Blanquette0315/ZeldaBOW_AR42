#pragma once
#include "CRes.h"

class CMesh :
    public CRes
{
private:
    ComPtr<ID3D11Buffer>    m_VB;
    D3D11_BUFFER_DESC       m_tVBDesc;

    ComPtr<ID3D11Buffer>    m_IB;
    D3D11_BUFFER_DESC       m_tIBDesc;
    UINT                    m_iIdxCount;

public:
    int Create(void* _pVtxSystem, size_t _iVtxCount, void* _pIdxSysmem, size_t _iIdxCount);
    void UpdateData();
    void render();
    // 인스턴싱용 render 함수
    void render_particle(UINT _iCount);

    virtual void Save(const wstring& _strRelativePath) override {}
    virtual int Load(const wstring& _strFilePath) override { return S_OK; }

    CLONE_ASSERT(CMesh);
public:
    CMesh(bool _bEngineRes = false);
    ~CMesh();
};

