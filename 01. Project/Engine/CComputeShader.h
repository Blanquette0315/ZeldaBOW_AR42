#pragma once
#include "CShader.h"

// ComputeShader를 추가할 때마다 해당 ComputeShader를 상속 받아 전용 Class를 만들어 사용하는 구조로 만들었다.

// ComputeShader는 GPU메모리에 직접 생성한 Texture를 이용해서 CPU - GPU간 자료 전달을 하지 않고,
// GPU 내에서 바로 값을 수정해 사용하여 CPU - GPU간 데이터 전달에서 발생하는 비용을 발생시키지 않는다.

// ComputeShader에서 사용할 수 있는 Texture는 아무런 Texture가 아니라 UnorderedAccess BindFlag로 만들어진 Texture만 사용이 가능하다.
// 해당 Texture는 u레지스터에 바인딩 걸어 Shader에서 사용하게 된다.

class CComputeShader :
    public CShader
{
private:
    ComPtr<ID3DBlob>                 m_CSBlob; // ComputeShader 블롭
    ComPtr<ID3D11ComputeShader>      m_CS;     // ComputeShader

protected:
    tMtrlConst                      m_Param;   // CS에 전달할 값

    UINT                            m_iGroupX; // 쓰레드 그룹 갯수 설정 X
    UINT                            m_iGroupY; // 쓰레드 그룹 갯수 설정 Y
    UINT                            m_iGroupZ; // 쓰레드 그룹 갯수 설정 Z

    const UINT                            m_iGroupPerThreadX; // const로 한 이유는 셰이더에서 한번 그룹당 스레드가 결정되면, 바뀔일이 없기 때문이다.
    const UINT                            m_iGroupPerThreadY;
    const UINT                            m_iGroupPerThreadZ;

public:
    // 인자로 경로와 HLSL 함수 이름을 받아 ComputeShader를 생성해준다.
    void CreateComputeShader(const wstring& _strRelativePath, const string& _strFuncName);
    void Execute();

    virtual void UpdateData() = 0;
    virtual void Clear() = 0;
public:
    CComputeShader(UINT _iGroupPerX, UINT _iGroupPerY, UINT _iGroupPerZ);
    ~CComputeShader();
};

