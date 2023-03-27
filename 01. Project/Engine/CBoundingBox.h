#pragma once
#include "CEntity.h"

class CGameObject;

class CBoundingBox :
    public CEntity
{
private:
    static float m_fDist;

    CGameObject* m_pOwner;

    Vec3 m_vCenterPos;
    Vec3 m_vCenterOffset;
    Vec3 m_vAxis[(UINT)DIR::END];

    Vec4 m_Point[8];
    Vec4 m_Plane[(UINT)FACE_TYPE::END];
    float m_fExtent[(UINT)DIR::END];

public:
    void SetCenterOffset(Vec3 _offset) { m_vCenterOffset = _offset; }
    void SetExtent(DIR _dir, float _extent) { m_fExtent[(UINT)_dir] = _extent; }
    void SetOwner(CGameObject* _owner) { m_pOwner = _owner; }

private:
    CGameObject* CheckRay(tRay _ray);
    static void ClearDist(){m_fDist = 3.402823466e+38;}

public:
    CLONE(CBoundingBox)

public:
    CBoundingBox();
    ~CBoundingBox();

    friend class CTransform;
};

