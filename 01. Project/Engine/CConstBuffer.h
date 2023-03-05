#pragma once
#include "CEntity.h"

class CConstBuffer :
    public CEntity
{
private:
    ComPtr<ID3D11Buffer>   m_CB;
    D3D11_BUFFER_DESC       m_tDesc;
    const CB_TYPE           m_eCBType;

private:
    int Create(size_t _iBufferSize);

public:
    void SetData(void *_pSrc, size_t _iSize);

    void SetData(void* _pSrc);

    void UpdateData(UINT _iPipelineStage);
    void UpdateData_CS();

    CLONE_ASSERT(CConstBuffer);
public:
    CConstBuffer(CB_TYPE _CBType);
    ~CConstBuffer();

    friend class CDevice;
};

