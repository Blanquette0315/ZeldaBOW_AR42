#pragma once
#include "CComponent.h"

#include "CFrustum.h"

class CCamera :
    public CComponent
{
private:
    CFrustum    m_Frustum;
    bool        m_bFrustumRender;

    Matrix      m_matView;      // 뷰 행렬
    Matrix      m_matViewInv;   // 뷰 역행렬
    Matrix      m_matProj;      // 투영 행렬
    Matrix      m_matProjInv;   // 투영 역행렬

    PROJ_TYPE   m_eProjType;    // 투영 방식
    float       m_fAspectRatio; // 종횡비

    float       m_fNear;
    float       m_fFar;         // 카메라 최대 시야
    float       m_FOV;          // 시야각
    float       m_fScale;       // 투영 범위 배율

    UINT        m_iLayerMask;

    vector<CGameObject*>    m_vecDeferred;
    vector<CGameObject*>    m_vecDeferredDecal;
    vector<CGameObject*>    m_vecQpaque;
    vector<CGameObject*>    m_vecMask;
    vector<CGameObject*>    m_vecDecal;
    vector<CGameObject*>    m_vecTransparent;
    vector<CGameObject*>    m_vecPostProcess;

    int                     m_iCamIdx;

public:
    float GetOrthographicScale() { return m_fScale; }
    void SetOrthographicScale(float _fScale) { m_fScale = _fScale; }

    void SetProjType(PROJ_TYPE _eType) { m_eProjType = _eType; }
    PROJ_TYPE GetProjType() { return m_eProjType; }

    void SetAspectRatio(float _fRatio) { m_fAspectRatio = _fRatio; }
    float GetAspectRatio() { return m_fAspectRatio; }

    void SetNear(float _fNear) { m_fNear = _fNear; }
    float GetNear() { return m_fNear; }

    void SetFar(float _fFar) { m_fFar = _fFar; }
    float GetFar() { return m_fFar; }

    void SetFOV(float _fFOV) { m_FOV = m_FOV; }
    float GetFOV() { return m_FOV; }

    void SetCamIdx(int _idx) { m_iCamIdx = _idx; }
    int GetCamIdx() { return m_iCamIdx; }

    const Matrix& GetViewMat() { return m_matView; }
    const Matrix& GetProjMat() { return m_matProj; }
    const Matrix& GetViewMatInv() { return m_matViewInv; }
    const Matrix& GetProjMatInv() { return m_matProjInv; }

    void SetLayerMask(const wstring& _strLayerName);
    void SetLayerMask(int _iLayerIdx);
    void SetLayerMaskAll() { m_iLayerMask = 0xffffffff; }
    void SetLayerMaskZero() { m_iLayerMask = 0; }
    void SetLayerVisible(int _iLayerIdx);
    void SetLayerInvisible(int _iLayerIdx);

    UINT GetLayerMask() { return m_iLayerMask; }

    CFrustum& GetFrustum() { return m_Frustum; }
    void SetFrustumRender(bool _Render) { m_bFrustumRender = _Render; }

protected:
    void CalcViewMat();
    void CalcProjMat();

private:
    void SortObject();
    void render_deferred();
    void render_deferreddecal();
    void render_opaque();
    void render_mask();
    void render_decal();
    void render_transparent();
    void render_postprocess();

public:
    virtual void finaltick();
    void render();

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CCamera);

public:
    CCamera();
    ~CCamera();
};