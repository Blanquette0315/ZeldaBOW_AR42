#pragma once
#include "CEntity.h"

#include "Ptr.h"
#include "CTexture.h"

class CMRT :
    public CEntity
{
private:
    Ptr<CTexture>   m_arrTarget[8]; // 랜더 타겟을 최대 8개 지정할 수 있기 때문에 고정 수치이므로 배열을 사용했다.
                                    // 주의할 점은 m_arrTarget에 타겟 텍스처를 넣을 때 무조건 앞에서 부터 채우는 순서로 넣어주어야 한다.
                                    // 이는 우리가 RTCount로 저장된 만큼만 가져다 사용할 것이기 때문에 구조를 이렇게 잡은 것이다.
    
    Vec4            m_arrClearColor[8]; // 타겟 클리어 색상 저장

    Ptr<CTexture>   m_DSTex;        // DSTex는 최대 1개 지정할 수 있기 때문에 사용하지 않는다면 nullptr로 채울 것이다.
    int             m_RTCount;      // m_arrTarget에 몇개의 텍스처를 넣어주었는지를 알기 위한 변수이다.

    D3D11_VIEWPORT  m_tViewPort;

public:
    void Create(Ptr<CTexture>(&_arrTex)[8], Vec4 (&arrClear)[8], Ptr<CTexture> _pDesTex);
    void OMSet(); // 해당 MRT가 지닌 랜더 타겟을 랜더 타겟으로 지정하는 함수이다.

    const D3D11_VIEWPORT& GetViewPort() { return m_tViewPort; }
    void Clear();

    CLONE_DUMMY(CMRT);
public:
    CMRT();
    ~CMRT();
};