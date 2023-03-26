#pragma once
#include "CComponent.h"
#include "CBoundingBox.h"

class CTransform :
    public CComponent
{
private:
    Vec3    m_vRelativePos;
    Vec3    m_vRelativeScale;
    Vec3    m_vRelativeRotation;

    Vec3    m_vRelativeDir[(UINT)DIR::END];
    Vec3	m_vWorldDir[(UINT)DIR::END];

    Matrix  m_matWorld;
    Matrix  m_matWorldInv; // 월드의 역행렬

    Matrix  m_matRot;
    Matrix  m_matRotInv;

    Matrix  m_matWorldRot;

    CBoundingBox m_BoundingBox;

    bool    m_bIgnParentScale;

public:
    void SetRelativePos(Vec3 _vPos) { m_vRelativePos = _vPos; }
    void SetRelativeScale(Vec3 _vScale) { m_vRelativeScale = _vScale; }
    void SetRelativeRotation(Vec3 _vRotation) { m_vRelativeRotation = _vRotation; }

    void SetRelativePos(float _x, float _y, float _z) { m_vRelativePos = Vec3(_x, _y, _z); }
    void SetRelativeScale(float _x, float _y, float _z) { m_vRelativeScale = Vec3(_x, _y, _z); }
    void SetRelativeRotation(float _x, float _y, float _z) { m_vRelativeRotation = Vec3(_x, _y, _z); }

    void SetRelativePos_XY(Vec2 _vPosXY) { m_vRelativePos.x = _vPosXY.x; m_vRelativePos.y = _vPosXY.y; }
    void SetRelativePos_XY(float _x, float _y) { m_vRelativePos.x = _x; m_vRelativePos.y = _y; }
    void SetRelativePos_Z(float _z) { m_vRelativePos.z = _z; }

    void SetWorldPos(Vec3 _vPos);
    void SetWorldScale(Vec3 _vScale);
    void SetWorldRotation(Vec3 _vRot);

    Vec3 GetRelativePos() { return m_vRelativePos; }
    Vec3 GetWorldPos() { return m_matWorld.Translation(); }

    Vec3 GetRelativeScale() { return m_vRelativeScale; }
    Vec3 GetWorldScale();

    RECT GetRectCoord();

    Vec3 GetRelativeRotation() { return m_vRelativeRotation; }
    Vec3 GetRelativeDir(DIR _eType) { return m_vRelativeDir[(UINT)_eType]; }
    Vec3 GetWorldDir(DIR _eType) { return m_vWorldDir[(UINT)_eType]; }

    void SetIgnoreParentScale(bool _bIgn) { m_bIgnParentScale = _bIgn; }
    bool GetIgnoreParentScale() { return m_bIgnParentScale; }

    const Matrix& GetWorldMat() { return m_matWorld; }
    const Matrix& GetRotMat() { return m_matRot; }
    const Matrix& GetWorldRotMat() { return m_matWorldRot; }

    const Matrix& GetWorldMatInv() { return m_matWorldInv; }
    const Matrix& GetRotMatInv() { return m_matRotInv; }

public:
    CGameObject* CheckRay(tRay _ray);
    static void ClearBoundingBoxDist() { CBoundingBox::ClearDist(); }

public:
    virtual void begin() override;
    virtual void tick() override;
    virtual void finaltick() override;

    void UpdateData();

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CTransform);
public:
    CTransform();
    virtual ~CTransform();
};

