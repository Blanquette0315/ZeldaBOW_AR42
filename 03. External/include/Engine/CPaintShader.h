#pragma once
#include "CComputeShader.h"

#include "Ptr.h"
#include "CTexture.h"

// 2차원을 사용하기 때문에 스레드의 z는 언제나 1이다.

class CPaintShader :
    public CComputeShader
{
private:
    Ptr<CTexture>   m_pTargetTex;

public:
    void SetTexture(Ptr<CTexture> _pTarget)
    {
        m_pTargetTex = _pTarget;
        m_Param.v2Arr[0] = Vec2(_pTarget->GetWidth(), _pTarget->GetHeight());
    }

    void SetColor(Vec4 _vColor) { m_Param.v4Arr[0] = _vColor; }

public:
    virtual void UpdateData() override;
    virtual void Clear() override;

public:
    CPaintShader();
    ~CPaintShader();
};

