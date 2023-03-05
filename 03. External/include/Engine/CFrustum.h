#pragma once
#include "CEntity.h"

// 절도체 6면을 이루기 위해 필요한 면 정의
enum class FACE_TYPE
{
    FT_NEAR,
    FT_FAR,
    FT_UP,
    FT_DOWN,
    FT_LEFT,
    FT_RIGHT,

    END,
};

class CCamera;

class CFrustum :
    public CEntity
{
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

public:

    CLONE(CFrustum);
public:
    CFrustum(CCamera* _OwnerCam);
    ~CFrustum();
};