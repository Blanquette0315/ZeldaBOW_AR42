#pragma once
#include "CEntity.h"

#include "Ptr.h"
#include "CTexture.h"

class CMRT :
    public CEntity
{
private:
    Ptr<CTexture>   m_arrTarget[8]; // ���� Ÿ���� �ִ� 8�� ������ �� �ֱ� ������ ���� ��ġ�̹Ƿ� �迭�� ����ߴ�.
                                    // ������ ���� m_arrTarget�� Ÿ�� �ؽ�ó�� ���� �� ������ �տ��� ���� ä��� ������ �־��־�� �Ѵ�.
                                    // �̴� �츮�� RTCount�� ����� ��ŭ�� ������ ����� ���̱� ������ ������ �̷��� ���� ���̴�.
    
    Vec4            m_arrClearColor[8]; // Ÿ�� Ŭ���� ���� ����

    Ptr<CTexture>   m_DSTex;        // DSTex�� �ִ� 1�� ������ �� �ֱ� ������ ������� �ʴ´ٸ� nullptr�� ä�� ���̴�.
    int             m_RTCount;      // m_arrTarget�� ��� �ؽ�ó�� �־��־������� �˱� ���� �����̴�.

    D3D11_VIEWPORT  m_tViewPort;

public:
    void Create(Ptr<CTexture>(&_arrTex)[8], Vec4 (&arrClear)[8], Ptr<CTexture> _pDesTex);
    void OMSet(); // �ش� MRT�� ���� ���� Ÿ���� ���� Ÿ������ �����ϴ� �Լ��̴�.

    const D3D11_VIEWPORT& GetViewPort() { return m_tViewPort; }
    void Clear();

    CLONE_DUMMY(CMRT);
public:
    CMRT();
    ~CMRT();
};