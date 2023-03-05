#pragma once
#include "CRes.h"

#include <DirectXTex/DirectXTex.h>

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex/DirectXTex_debug")
#else
#pragma comment(lib, "DirectXTex/DirectXTex")
#endif

// Texture Class를 직접 만들어서 GPU메모리에 직접 Texture를 생성 할 수도 있도록 구조를 잡아주었다.
// 해당 Texture는 여러가지 용도로 만들어질 수 있기 때문에 각각의 View를 멤버로 지닐 수 있도록 만들었다.

// 일반적인 ShaderReource BindFlag로 만들어진 Texture는 t레지스터에 바인딩이 걸리는데, 이는 읽기만 가능하다.
// UnorderedAccess BindFlag로 만들어진 Texture는 u 레지스터에 바인딩이 걸려 읽고, 쓰는 것이 가능하다.
// 따라서 ComputeShader시 사용하는 Texture는 UnorderedAccess BindFlag로 만들어진 Texture만 사용이 가능하다.

// D3D11_BIND_SHADER_RESOURCE로 플레그를 잡아 만들면 t레지스터 뿐 아니라 u레지스터에도 사용이 가능하다.
// 하지만 ComputeShader는 랜더링을 하기 위한 Shader는 아니기 때문에 해당 이미지가 필요할 수 도 있기 때문이다.
// t레지스터에 바인딩 걸면 VS, HS, DS, GS, PS 뿐 아니라 CS에서도 사용이 가능하다.
// u레지스터에 바인딩 건 Texture는 오로지 CS에서만 사용이 가능하다.

// UVA는 순서 없는 접근이라는 뜻이다.

class CTexture :
    public CRes
{
private:
    ScratchImage                        m_Image;    // System Mem 에 로딩된 이미지 픽셀데이터 관리
    ComPtr<ID3D11Texture2D>             m_Tex2D;    // GPU Mem 에 옮긴 데이터 관리하는 객체

    ComPtr<ID3D11RenderTargetView>      m_RTV; // Tex2D 를 OM(OutputMerge) RenderTarget 으로 전달할 때 사용
    ComPtr<ID3D11DepthStencilView>      m_DSV; // Tex2D 를 OM(OutputMerge) DepthStencil 로 전달할 때 사용
    ComPtr<ID3D11ShaderResourceView>    m_SRV; // Tex2D 를 t 레지스터에 바인딩할 때 사용
    ComPtr<ID3D11UnorderedAccessView>   m_UAV; // Tex2D 를 컴퓨트 쉐이더 u 레지스터에 전달할 때 사용

    D3D11_TEXTURE2D_DESC                m_Desc;

public:
    //Test용 이미지 전달하기
    ScratchImage& GetScratchImage() { return m_Image; }

public:
    UINT GetWidth() { return m_Desc.Width; }
    UINT GetHeight() { return m_Desc.Height; }

    // 픽셀 색상 정보 가져오기
    void GetPixelVector(vector<vector<tBGRA>>& _inVec);

    // 텍스처 가져오기
    ComPtr<ID3D11Texture2D> GetTex2D() { return m_Tex2D; }

    // 각각의 View 가져오기
    ComPtr<ID3D11RenderTargetView> GetRTV() { return m_RTV; }
    ComPtr<ID3D11DepthStencilView> GetDSV() { return m_DSV; }
    ComPtr<ID3D11ShaderResourceView> GetSRV() { return m_SRV; }
    ComPtr<ID3D11UnorderedAccessView> GetUAV() { return m_UAV; }

    // PIPELINE_STAGE
    void UpdateData(UINT _iRegisterNum, UINT _iPipelineStage);
    static void Clear(UINT _iRegisterNum);


    void UpdateData_CS(UINT _iRegisterNum, bool _bShaderRes = false);
    static void Clear_CS(UINT _iRegisterNum);

private:
    virtual void Save(const wstring& _strRelativePath);

    // 파일로 부터 로딩
    virtual int Load(const wstring& _strFilePath);

    // GPU메모리에 직접 Texture를 생성
    void Create(UINT _iWidth, UINT _iHeight, DXGI_FORMAT _Format, UINT _iBindFlag);
    void Create(ComPtr<ID3D11Texture2D> _Tex2D); // 이미 존재하는 ID3D11Texture2D를 받아와 저장하고, View를 생성
    
public:
    CLONE_ASSERT(CTexture);

public:
    CTexture(bool _bEngineRes = false);
    ~CTexture();

    friend class CResMgr;
};

