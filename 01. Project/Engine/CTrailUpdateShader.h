#pragma once
#include "CComputeShader.h"

class CStructuredBuffer;

class CTrailUpdateShader :
    public CComputeShader
{
private:
    CStructuredBuffer*  m_VecPosBuffer;
    CStructuredBuffer*  m_OutputBuffer;

public:
    

public:
    void SetSize(int _iSize){ m_Param.iArr[0] = _iSize; }
    void SetVecPosBuffer(CStructuredBuffer* _pBuffer) { m_VecPosBuffer = _pBuffer;}
    void SetOutputBuffer(CStructuredBuffer* _pBuffer) { m_OutputBuffer = _pBuffer; }

public:
    virtual void UpdateData() override;
    virtual void Clear() override;

public:
    CTrailUpdateShader();
    ~CTrailUpdateShader();
};

