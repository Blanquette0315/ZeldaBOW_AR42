#pragma once
#include "CRenderComponent.h"

#include "CRaycastShader.h"
#include "CHeightMapShader.h"
#include "CWeightMapShader.h"
#include "CRaymapShader.h"

class CLandScape :
    public CRenderComponent
{
private:
    UINT                    m_iXFaceCount;      // 지형 면 개수
    UINT                    m_iZFaceCount;      // 지형 면 개수

    Vec2                    m_vBrushScale;      // 브러쉬 크기(전체 지형대비 크기 비율값)
    Ptr<CTexture>           m_pBrushTex;        // 브러쉬용 텍스쳐

    Ptr<CTexture>           m_pTileArrTex;      // 타일 배열 텍스쳐

    Ptr<CRaycastShader>     m_pCSRaycast;       // 픽킹 쉐이더
    CStructuredBuffer*      m_pCrossBuffer;	    // 마우스 피킹되는 지점 정보 받는 버퍼

    Ptr<CHeightMapShader>   m_pCSHeightMap;     // 높이맵 쉐이더
    Ptr<CTexture>           m_pHeightMap;       // 높이맵 텍스쳐

    Ptr<CRaymapShader>      m_pCSRayMap;
    Ptr<CTexture>           m_pRayMap;


    Ptr<CWeightMapShader>   m_pCSWeightMap;     // 가중치 쉐이더
    CStructuredBuffer*      m_pWeightMapBuffer; // 가중치 저장 버퍼
    UINT					m_iWeightWidth;		// 가중치 버퍼 가로세로 행렬 수
    UINT					m_iWeightHeight;	// 가중치 버퍼 가로세로 행렬 수
    UINT                    m_iWeightIdx;		// 증가 시킬 가중치 부위

    LANDSCAPE_MOD           m_eMod; 	        // 지형 툴모드에서 상태값

    ComPtr<ID3D11Buffer>    m_pCopyBuffer;      // CopyBuffer
    D3D11_BUFFER_DESC       m_CopyBufferDesc;
    D3D11_MAPPED_SUBRESOURCE    m_MappedResource;
    Vector3* m_arrVertexPos;
    UINT* m_arrIndice;


public:
    virtual void finaltick() override;
    virtual void render() override;

public:
    void SetFaceCount(UINT _X, UINT _Z);
    void CreateActor();

private:
    void CreateMesh();
    void CreateMaterial();
    void CreateTexture();
    void Raycasting();

    void BrushAreaShow(bool _Show);

    void CreateCpBuffer(UINT _byteWidth);

    CLONE(CLandScape);
public:
    CLandScape();
    ~CLandScape();
};