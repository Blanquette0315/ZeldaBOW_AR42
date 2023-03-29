#pragma once
#include "CComponent.h"

#include "CFrustum.h"

class CCamera :
    public CComponent
{
private:
    CFrustum    m_Frustum;
    tRay        m_ray;

    Matrix      m_matView;     
    Matrix      m_matViewInv;  
    Matrix      m_matProj;     
    Matrix      m_matProjInv;  

    PROJ_TYPE   m_eProjType;
    float       m_fWidth;
    float       m_fAspectRatio;

    float       m_fNear;
    float       m_fFar;        
    float       m_FOV;         
    float       m_fScale;      

    UINT        m_iLayerMask;

    vector<CGameObject*>    m_vecDeferred;
    vector<CGameObject*>    m_vecDeferredDecal;
    vector<CGameObject*>    m_vecQpaque;
    vector<CGameObject*>    m_vecMask;
    vector<CGameObject*>    m_vecDecal;
    vector<CGameObject*>    m_vecTransparent;
    vector<CGameObject*>    m_vecPostProcess;
    vector<CGameObject*>    m_vecDynamicShadow;

    int                     m_iCamIdx;

public:
    float GetOrthographicScale() { return m_fScale; }
    void SetOrthographicScale(float _fScale) { m_fScale = _fScale; }

    void SetProjType(PROJ_TYPE _eType) { m_eProjType = _eType; }
    PROJ_TYPE GetProjType() { return m_eProjType; }

    void SetAspectRatio(float _fRatio) { m_fAspectRatio = _fRatio; }
    float GetAspectRatio() { return m_fAspectRatio; }

    void SetWidth(float _fWidth) { m_fWidth = _fWidth; }
    float GetWidth() { return m_fWidth; }

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

    const tRay& GetRay() { return m_ray; }

protected:
    void CalcViewMat();
    void CalcProjMat();
    void CalRay();

public:
    void SortObject();
    void SortShadowObject();
    void render_deferred();
    void render_deferreddecal();
    void render_opaque();
    void render_mask();
    void render_decal();
    void render_transparent();
    void render_postprocess();
    void render_depthmap();

public:
    virtual void finaltick();
    virtual void finaltick_module();
    void render();

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CCamera);

public:
    CCamera();
    ~CCamera();
};