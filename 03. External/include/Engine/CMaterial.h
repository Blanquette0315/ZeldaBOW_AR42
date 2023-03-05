#pragma once
#include "CRes.h"
#include "CGrapicsShader.h"

#include "CTexture.h"

class CMaterial :
    public CRes
{
private:
    tMtrlConst          m_tConst;
    Ptr<CTexture>       m_arrTex[TEX_END];
    Ptr<CGrapicsShader> m_pShader;

public:
    void SetScalarParam(SCALAR_PARAM _eScalarType, void* _pData);
    void GetScalarParam(SCALAR_PARAM _eScalarType, void* _pOut);

    void SetTexParam(TEX_PARAM _eTex, Ptr<CTexture> _pTex);
    Ptr<CTexture> GetTexParam(TEX_PARAM _eTexParam) { return m_arrTex[(UINT)_eTexParam]; }

    void SetShader(Ptr<CGrapicsShader> _pShader) { m_pShader = _pShader; }
    Ptr<CGrapicsShader> GetShader() { return m_pShader; }

    void UpdateData();
    static void Clear();

    virtual void Save(const wstring& _strRelativePath) override;
    virtual int Load(const wstring& _strFilePath) override;

    CLONE(CMaterial);
public:
    CMaterial(bool _bEngineRes = false);
    CMaterial(const CMaterial& _other);
    ~CMaterial();
};

