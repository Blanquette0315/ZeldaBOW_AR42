#pragma once
#include "CRes.h"

#include <DirectXTex/DirectXTex.h>

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex/DirectXTex_debug")
#else
#pragma comment(lib, "DirectXTex/DirectXTex")
#endif

// Texture Class�� ���� ���� GPU�޸𸮿� ���� Texture�� ���� �� ���� �ֵ��� ������ ����־���.
// �ش� Texture�� �������� �뵵�� ������� �� �ֱ� ������ ������ View�� ����� ���� �� �ֵ��� �������.

// �Ϲ����� ShaderReource BindFlag�� ������� Texture�� t�������Ϳ� ���ε��� �ɸ��µ�, �̴� �б⸸ �����ϴ�.
// UnorderedAccess BindFlag�� ������� Texture�� u �������Ϳ� ���ε��� �ɷ� �а�, ���� ���� �����ϴ�.
// ���� ComputeShader�� ����ϴ� Texture�� UnorderedAccess BindFlag�� ������� Texture�� ����� �����ϴ�.

// D3D11_BIND_SHADER_RESOURCE�� �÷��׸� ��� ����� t�������� �� �ƴ϶� u�������Ϳ��� ����� �����ϴ�.
// ������ ComputeShader�� �������� �ϱ� ���� Shader�� �ƴϱ� ������ �ش� �̹����� �ʿ��� �� �� �ֱ� �����̴�.
// t�������Ϳ� ���ε� �ɸ� VS, HS, DS, GS, PS �� �ƴ϶� CS������ ����� �����ϴ�.
// u�������Ϳ� ���ε� �� Texture�� ������ CS������ ����� �����ϴ�.

// UVA�� ���� ���� �����̶�� ���̴�.

class CTexture :
    public CRes
{
private:
    ScratchImage                        m_Image;    // System Mem �� �ε��� �̹��� �ȼ������� ����
    ComPtr<ID3D11Texture2D>             m_Tex2D;    // GPU Mem �� �ű� ������ �����ϴ� ��ü

    ComPtr<ID3D11RenderTargetView>      m_RTV; // Tex2D �� OM(OutputMerge) RenderTarget ���� ������ �� ���
    ComPtr<ID3D11DepthStencilView>      m_DSV; // Tex2D �� OM(OutputMerge) DepthStencil �� ������ �� ���
    ComPtr<ID3D11ShaderResourceView>    m_SRV; // Tex2D �� t �������Ϳ� ���ε��� �� ���
    ComPtr<ID3D11UnorderedAccessView>   m_UAV; // Tex2D �� ��ǻƮ ���̴� u �������Ϳ� ������ �� ���

    D3D11_TEXTURE2D_DESC                m_Desc;

public:
    //Test�� �̹��� �����ϱ�
    ScratchImage& GetScratchImage() { return m_Image; }

public:
    UINT GetWidth() { return m_Desc.Width; }
    UINT GetHeight() { return m_Desc.Height; }
    UINT GetArraySize() { return m_Desc.ArraySize; }

    // �ȼ� ���� ���� ��������
    void GetPixelVector(vector<vector<tBGRA>>& _inVec);

    // �ؽ�ó ��������
    ComPtr<ID3D11Texture2D> GetTex2D() { return m_Tex2D; }

    // ������ View ��������
    ComPtr<ID3D11RenderTargetView>      GetRTV() { return m_RTV; }
    ComPtr<ID3D11DepthStencilView>      GetDSV() { return m_DSV; }
    ComPtr<ID3D11ShaderResourceView>    GetSRV() { return m_SRV; }
    ComPtr<ID3D11UnorderedAccessView>   GetUAV() { return m_UAV; }
    const D3D11_TEXTURE2D_DESC&         GetDesc() { return m_Desc; }
    void*                               GetSysMem() { return m_Image.GetPixels(); }
    UINT                                GetRowPitch() const { return (UINT)m_Image.GetImages()->rowPitch; }
    UINT                                GetSlicePitch()const { return (UINT)m_Image.GetImages()->slicePitch; }

    void GenerateMip(UINT _iMipLevel);
    void SaveTexture(const wstring& _strRelativePath);

    // PIPELINE_STAGE
    void UpdateData(UINT _iRegisterNum, UINT _iPipelineStage);
    static void Clear(UINT _iRegisterNum);

    void UpdateData_CS(UINT _iRegisterNum, bool _bShaderRes = false);
    static void Clear_CS(UINT _iRegisterNum);

private:
    virtual void Save(const wstring& _strRelativePath);

    // ���Ϸ� ���� �ε�
    virtual int Load(const wstring& _strFilePath);
    int Load(const wstring& _strFilePath, int _iMipLevel);

    // GPU�޸𸮿� ���� Texture�� ����
    void Create(UINT _iWidth, UINT _iHeight, DXGI_FORMAT _Format, UINT _iBindFlag);
    void Create(ComPtr<ID3D11Texture2D> _Tex2D); // �̹� �����ϴ� ID3D11Texture2D�� �޾ƿ� �����ϰ�, View�� ����
    int CreateArrayTexture(const vector<Ptr<CTexture>>& _vecTex, int _iMapLevel);

public:
    CLONE_ASSERT(CTexture);

public:
    CTexture(bool _bEngineRes = false);
    ~CTexture();

    friend class CResMgr;
};

