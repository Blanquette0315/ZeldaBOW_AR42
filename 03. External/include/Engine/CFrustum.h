#pragma once
#include "CEntity.h"

class CCamera;

class CFrustum :
    public CEntity
{
private:
    static vector<CGameObject*> m_pInFrustumObj;

private:
    CCamera*    m_pOwnerCam;

    Matrix      m_matInv;

    Vec3        m_ProjPos[8];
    Vec3        m_WorldPos[8];
    Vec4        m_Plane[(UINT)FACE_TYPE::END];

public:
    void finaltick();
    bool CheckFrustum(Vec3 _vPos);
    bool CheckFrustumRadius(Vec3 _vPos, float _fRadius);

    Matrix& GetMatInv() { return m_matInv; }

    static const vector<CGameObject*>& GetInFrustumObjs() { return m_pInFrustumObj; }
    static void PushBackInFrustumObjs(CGameObject* _pObj) { m_pInFrustumObj.push_back(_pObj); }
    static void ClearInFrustumObjs() { m_pInFrustumObj.clear(); }
public:

    CLONE(CFrustum);
public:
    CFrustum(CCamera* _OwnerCam);
    ~CFrustum();
};