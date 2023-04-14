#pragma once
#include "CRes.h"

#include "CFBXLoader.h"
#include "CStructuredBuffer.h"

struct tIndexInfo
{
    ComPtr<ID3D11Buffer>    pIB;
    D3D11_BUFFER_DESC       tIBDesc;
    UINT				    iIdxCount;
    void* pIdxSysMem;
};

class CMesh :
    public CRes
{
private:
    ComPtr<ID3D11Buffer>    m_VB;
    D3D11_BUFFER_DESC       m_tVBDesc;
    UINT                    m_iVtxCount;
    void*                   m_pVtxSys;

    vector<tIndexInfo>		m_vecIdxInfo;

    // Animation3D ����
    vector<tMTAnimClip>		m_vecAnimClip;
    vector<tMTBone>			m_vecBones;

    CStructuredBuffer*      m_pBoneFrameData;   // ��ü �� ������ ����(ũ��, �̵�, ȸ��) (������ ������ŭ)
    CStructuredBuffer*      m_pBoneOffset;	    // �� ���� offset ���(�� ���� ��ġ�� �ǵ����� ���) (1�� ¥��)


public:
    Vtx* GetVtxSysMem() { return (Vtx*)m_pVtxSys; }
    UINT GetSubsetCount() { return (UINT)m_vecIdxInfo.size(); }
    const vector<tMTBone>* GetBones() { return &m_vecBones; }
    UINT GetBoneCount() { return (UINT)m_vecBones.size(); }
    const vector<tMTAnimClip>* GetAnimClip() { return &m_vecAnimClip; }
    bool IsAnimMesh() { return !m_vecAnimClip.empty(); }

    CStructuredBuffer* GetBoneFrameDataBuffer() { return m_pBoneFrameData; } // ��ü �� ������ ����
    CStructuredBuffer* GetBoneOffsetBuffer() { return  m_pBoneOffset; }	   // �� ���� offset ���	    

    static CMesh* CreateFromContainer(CFBXLoader& _loader);
    void AddAnimationFromContainer(const wstring _strFBXPath);

public:
    int Create(void* _pVtxSys, size_t _iVtxCount, void* _pIdxSys, size_t _iIdxCount);
    void UpdateData(UINT _iSubset = 0);
    void render(UINT _iSubset = 0);
    // �ν��Ͻ̿� render �Լ�
    void render_particle(UINT _iCount);

    virtual void Save(const wstring& _strRelativePath) override;
    virtual int Load(const wstring& _strFilePath) override;

    CLONE_ASSERT(CMesh);

public:
    CMesh(bool _bEngineRes = false);
    ~CMesh();

public:
    ComPtr<ID3D11Buffer> GetVB() { return m_VB; }
    ComPtr<ID3D11Buffer> GetIB(UINT _iSubset = 0) 
    {
        return m_vecIdxInfo[_iSubset].pIB;
    }
};

