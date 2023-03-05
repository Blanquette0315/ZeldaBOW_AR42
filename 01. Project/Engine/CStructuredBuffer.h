#pragma once
#include "CEntity.h"

// ����ȭ ���۴� ���� ������ ������ �ִ� ��� ���ۿ� ���� ������ ������ ���� �ʱ� ������ CDevice���� �������� �ʰ�,
// ����ȭ ���۸� ������ ����� �ʿ��� �Ҹ��Ű���� �������.
// ���� -> ����ϴ� ���� CRenderMgr�̴�. : render()���� ī�޶� ������ rendering ���ִµ�, ����ȿ���� �̶� ����ǹǷ�.

class CStructuredBuffer :
    public CEntity
{
private:
    ComPtr<ID3D11Buffer>                m_SB;
    ComPtr<ID3D11ShaderResourceView>    m_SRV;
    ComPtr<ID3D11UnorderedAccessView>   m_UAV;

    ComPtr<ID3D11Buffer>                m_WriteBuffer;  // ���� ���� ���� (m_SB�� main buffer�̴�.)
    ComPtr<ID3D11Buffer>                m_ReadBuffer;   // �б� ���� ���� (m_SB�� main buffer�̴�.)

    D3D11_BUFFER_DESC                   m_tDesc;
    SB_TYPE                             m_eType;

    UINT                                m_iElementSize;
    UINT                                m_iElementCount;

    UINT                                m_iRecentRegisterNum;   // �ֱ� ����� t�������� ��ȣ
    UINT                                m_iRecentRegisterNumRW; // �ֱ� ����� u�������� ��ȣ
                                                                // u���������� ��� 0~7�������� ����� �����ϴ�.


public:
    int Create(UINT _iElementSize, UINT _iElementCount, SB_TYPE _eType, void* _pInitial, bool _CPUAccess = false);
    UINT GetElementSize() { return m_iElementSize; }
    UINT GetElementCount() { return m_iElementCount; }

    void SetData(void* _pSrc, UINT _iElementCount);
    void GetData(void* _pDst, UINT _iSizeByte = 0);

    // ���������� ���������� ���ε� �ɱ�
    void UpdateData(UINT _iRegisterNum, UINT _PipelineStage);
    // t�������Ϳ� ���ε� �� ���̸� true��, u�������Ϳ� ���ε��� �����̶�� false�� �־��ش�.
    void UpdateData_CS(UINT _iRegisterNum, bool _bShaderRes);

    void Clear();

public:
    CLONE(CStructuredBuffer); // ������ ���� ���簡 �ʿ����̹Ƿ� ����Ʈ ���� �����ڸ� ��� ���̴�.

public:
    CStructuredBuffer();
    ~CStructuredBuffer();
};

