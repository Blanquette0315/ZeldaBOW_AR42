#pragma once
#include "CEntity.h"

// 구조화 버퍼는 생성 갯수가 정해져 있는 상수 버퍼와 생성 갯수가 정해져 있지 않기 때문에 CDevice에서 관리하지 않고,
// 구조화 버퍼를 생성해 사용한 쪽에서 소멸시키도록 만들었다.
// 광원 -> 사용하는 곳은 CRenderMgr이다. : render()에서 카메라를 취합해 rendering 해주는데, 광원효과가 이때 적용되므로.

class CStructuredBuffer :
    public CEntity
{
private:
    ComPtr<ID3D11Buffer>                m_SB;
    ComPtr<ID3D11ShaderResourceView>    m_SRV;
    ComPtr<ID3D11UnorderedAccessView>   m_UAV;

    ComPtr<ID3D11Buffer>                m_WriteBuffer;  // 쓰기 전용 버퍼 (m_SB가 main buffer이다.)
    ComPtr<ID3D11Buffer>                m_ReadBuffer;   // 읽기 전용 버퍼 (m_SB가 main buffer이다.)

    D3D11_BUFFER_DESC                   m_tDesc;
    SB_TYPE                             m_eType;

    UINT                                m_iElementSize;
    UINT                                m_iElementCount;

    UINT                                m_iRecentRegisterNum;   // 최근 사용한 t레지스터 번호
    UINT                                m_iRecentRegisterNumRW; // 최근 사용한 u레지스터 번호
                                                                // u레지스터의 경우 0~7번까지만 사용이 가능하다.


public:
    int Create(UINT _iElementSize, UINT _iElementCount, SB_TYPE _eType, void* _pInitial, bool _CPUAccess = false);
    UINT GetElementSize() { return m_iElementSize; }
    UINT GetElementCount() { return m_iElementCount; }

    void SetData(void* _pSrc, UINT _iElementCount);
    void GetData(void* _pDst, UINT _iSizeByte = 0);

    // 파이프라인 스테이지에 바인딩 걸기
    void UpdateData(UINT _iRegisterNum, UINT _PipelineStage);
    // t레지스터에 바인딩 할 것이면 true를, u레지스터에 바인딩걸 목적이라면 false를 넣어준다.
    void UpdateData_CS(UINT _iRegisterNum, bool _bShaderRes);

    void Clear();

public:
    CLONE(CStructuredBuffer); // 아직은 깊은 복사가 필요없어보이므로 디폴트 복사 생성자를 사용 중이다.

public:
    CStructuredBuffer();
    ~CStructuredBuffer();
};

