#pragma once
#include "CShader.h"

// ComputeShader�� �߰��� ������ �ش� ComputeShader�� ��� �޾� ���� Class�� ����� ����ϴ� ������ �������.

// ComputeShader�� GPU�޸𸮿� ���� ������ Texture�� �̿��ؼ� CPU - GPU�� �ڷ� ������ ���� �ʰ�,
// GPU ������ �ٷ� ���� ������ ����Ͽ� CPU - GPU�� ������ ���޿��� �߻��ϴ� ����� �߻���Ű�� �ʴ´�.

// ComputeShader���� ����� �� �ִ� Texture�� �ƹ��� Texture�� �ƴ϶� UnorderedAccess BindFlag�� ������� Texture�� ����� �����ϴ�.
// �ش� Texture�� u�������Ϳ� ���ε� �ɾ� Shader���� ����ϰ� �ȴ�.

class CComputeShader :
    public CShader
{
private:
    ComPtr<ID3DBlob>                 m_CSBlob; // ComputeShader ���
    ComPtr<ID3D11ComputeShader>      m_CS;     // ComputeShader

protected:
    tMtrlConst                      m_Param;   // CS�� ������ ��

    UINT                            m_iGroupX; // ������ �׷� ���� ���� X
    UINT                            m_iGroupY; // ������ �׷� ���� ���� Y
    UINT                            m_iGroupZ; // ������ �׷� ���� ���� Z

    const UINT                            m_iGroupPerThreadX; // const�� �� ������ ���̴����� �ѹ� �׷�� �����尡 �����Ǹ�, �ٲ����� ���� �����̴�.
    const UINT                            m_iGroupPerThreadY;
    const UINT                            m_iGroupPerThreadZ;

public:
    // ���ڷ� ��ο� HLSL �Լ� �̸��� �޾� ComputeShader�� �������ش�.
    void CreateComputeShader(const wstring& _strRelativePath, const string& _strFuncName);
    void Execute();

    virtual void UpdateData() = 0;
    virtual void Clear() = 0;
public:
    CComputeShader(UINT _iGroupPerX, UINT _iGroupPerY, UINT _iGroupPerZ);
    ~CComputeShader();
};

