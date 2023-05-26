#pragma once
#include "CRes.h"
#include "CGraphicsShader.h"

#include "CTexture.h"

class CMaterial :
    public CRes
{
private:
    tMtrlConst          m_tConst;
    Ptr<CTexture>       m_arrTex[TEX_END];
    Ptr<CGraphicsShader> m_pShader;

public:
    void SetLayerIdx(int _iIdx) { m_tConst.iLayerIdx = _iIdx; }
    void SetInstancing(bool _b) { m_tConst.bInstancing = _b; }
    void SetScalarParam(SCALAR_PARAM _eScalarType, void* _pData);
    void GetScalarParam(SCALAR_PARAM _eScalarType, void* _pOut);

    void SetTexParam(TEX_PARAM _eTex, Ptr<CTexture> _pTex);
    Ptr<CTexture> GetTexParam(TEX_PARAM _eTexParam) { return m_arrTex[(UINT)_eTexParam]; }

    void SetShader(Ptr<CGraphicsShader> _pShader) { m_pShader = _pShader; }
    Ptr<CGraphicsShader> GetShader() { return m_pShader; }

    void SetMaterialCoefficient(Vec4 _vDiff, Vec4 _vSpec, Vec4 _vAmb, Vec4 _vEmis)
    {
        m_tConst.mtrl.vDiff = _vDiff;
        m_tConst.mtrl.vAmb = _vAmb;
        m_tConst.mtrl.vSpec = _vSpec;
        m_tConst.mtrl.vEmv = _vEmis;
    }

    void SetAnim3D(bool _bTrue) { m_tConst.arrAnimData[0] = (int)_bTrue; }
    void SetBoneCount(int _iBoneCount) { m_tConst.arrAnimData[1] = _iBoneCount; }

    void SetUVCount(int _iUVCount) { m_tConst.iUVCount = _iUVCount; }

    void UpdateData();
    void UpdateData_Inst();
    static void Clear();

    virtual void Save(const wstring& _strRelativePath) override;
    virtual int Load(const wstring& _strFilePath) override;

    CLONE(CMaterial);
public:
    CMaterial(bool _bEngineRes = false);
    CMaterial(const CMaterial& _other);
    ~CMaterial();
};

