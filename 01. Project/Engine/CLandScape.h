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
    UINT                    m_iXFaceCount;      // ���� �� ����
    UINT                    m_iZFaceCount;      // ���� �� ����

    Vec2                    m_vBrushScale;      // �귯�� ũ��(��ü ������� ũ�� ������)
    Ptr<CTexture>           m_pBrushTex;        // �귯���� �ؽ���

    Ptr<CTexture>           m_pTileArrTex;      // Ÿ�� �迭 �ؽ���

    Ptr<CRaycastShader>     m_pCSRaycast;       // ��ŷ ���̴�
    CStructuredBuffer*      m_pCrossBuffer;	    // ���콺 ��ŷ�Ǵ� ���� ���� �޴� ����

    Ptr<CHeightMapShader>   m_pCSHeightMap;     // ���̸� ���̴�
    Ptr<CTexture>           m_pHeightMap;       // ���̸� �ؽ���

    Ptr<CRaymapShader>      m_pCSRayMap;
    Ptr<CTexture>           m_pRayMap;


    Ptr<CWeightMapShader>   m_pCSWeightMap;     // ����ġ ���̴�
    CStructuredBuffer*      m_pWeightMapBuffer; // ����ġ ���� ����
    UINT					m_iWeightWidth;		// ����ġ ���� ���μ��� ��� ��
    UINT					m_iWeightHeight;	// ����ġ ���� ���μ��� ��� ��
    UINT                    m_iWeightIdx;		// ���� ��ų ����ġ ����

    LANDSCAPE_MOD           m_eMod; 	        // ���� ����忡�� ���°�

    Ptr<CMaterial>          m_LandScapeMtrl;
    ComPtr<ID3D11Buffer>    m_pSOBuffer;        // Stream Out Buffer

    ComPtr<ID3D11Buffer>    m_pCopyBuffer;      // CopyBuffer
    D3D11_BUFFER_DESC       m_CopyBufferDesc;
    D3D11_MAPPED_SUBRESOURCE    m_MappedResource;
    Vector3*                    m_arrVertexPos;
    UINT*                       m_arrIndice;
    Ptr<CMaterial>              m_pMaxTessMtrl;
    int                         m_iMaxVtxCount;
    UINT                        m_inumVertices;
    bool                        m_bCurDataCooking;


public:
    virtual void begin() override;
    virtual void finaltick() override;
    virtual void render() override;

public:
    void SetFaceCount(UINT _X, UINT _Z);
    void SetLandMod(LANDSCAPE_MOD _eMod);
    void SetBrushScale(Vec2 _Scale) { m_vBrushScale = _Scale; }
    void SetBrushTexture(Ptr<CTexture> _BrushTex) { m_pBrushTex = _BrushTex; }

    UINT GetFaceCountX() { return m_iXFaceCount; }
    UINT GetFaceCountZ() { return m_iZFaceCount; }
    LANDSCAPE_MOD GetLandMod() { return m_eMod; }
    Vec2 GetBrushScale() { return m_vBrushScale; }
    Ptr<CTexture> GetBrushTex() { return m_pBrushTex; }
    bool IsCurDataCooking() { return m_bCurDataCooking; }

    void SaveHightImage();

    void SaveWightData();
    void LoadWightData();

    void SaveCookingData();
    void LoadCookingData();

    void Cooking();
    void CreateActor();

    void LandStreamOut();

private:
    void CreateMesh();
    void CreateMaterial();
    void CreateTexture();
    void Raycasting();

    void BrushAreaShow(bool _Show);

    void CreateCpBuffer(UINT _byteWidth);
    void CreateSOBuffer();

    CLONE(CLandScape);
public:
    CLandScape();
    ~CLandScape();
};