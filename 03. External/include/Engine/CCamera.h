#pragma once
#include "CComponent.h"

#include "CFrustum.h"

class CCamera :
    public CComponent
{
private:
    CFrustum    m_Frustum;
    bool        m_bFrustumRender;

    Matrix      m_matView;      // �� ���
    Matrix      m_matViewInv;   // �� �����
    Matrix      m_matProj;      // ���� ���
    Matrix      m_matProjInv;   // ���� �����

    PROJ_TYPE   m_eProjType;    // ���� ���
    float       m_fAspectRatio; // ��Ⱦ��

    float       m_fFar;         // ī�޶� �ִ� �þ�
    float       m_FOV;          // �þ߰�
    float       m_fScale;       // ���� ���� ����

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

    void SetFar(float _fFar) { m_fFar = _fFar; }
    float GetFar() { return m_fFar; }

    const Matrix& GetViewMat() { return m_matView; }
    const Matrix& GetProjMat() { return m_matProj; }
    const Matrix& GetViewMatInv() { return m_matViewInv; }
    const Matrix& GetProjMatInv() { return m_matProjInv; }

    void SetLayerMask(const wstring& _strLayerName);
    void SetLayerMask(int _iLayerIdx);
    void SetLayerMaskAll() { m_iLayerMask = 0xffffffff; }
    void SetLayerMaskZero() { m_iLayerMask = 0; }

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
    virtual void SaveToFile(FILE* _File);
    virtual void LoadFromFile(FILE* _File);

    CLONE(CCamera);

public:
    CCamera();
    ~CCamera();
};