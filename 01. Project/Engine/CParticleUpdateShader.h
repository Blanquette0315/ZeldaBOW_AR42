#pragma once
#include "CComputeShader.h"

class CStructuredBuffer;

class CParticleUpdateShader :
    public CComputeShader
{
public:
    CStructuredBuffer* m_ParticleBuffer;
    CStructuredBuffer* m_ParticleShare;

public:
    void SetParticleBuffer(CStructuredBuffer* _Buffer);
    void SetParticleShareData(CStructuredBuffer* _Buffer) { assert(_Buffer); m_ParticleShare = _Buffer; }

    void SetWorldSpawn(bool _bWorldSpawn) { m_Param.iArr[1] = (int)_bWorldSpawn; }
    void SetObjectWorldPos(Vec3 _vWorldPos) { m_Param.v4Arr[0] = _vWorldPos; }
    void SetSpawnRange(float _Range) { m_Param.fArr[0] = _Range; }
    void SetMinMaxSpeed(Vec2 _vSpeed) { m_Param.v2Arr[0] = _vSpeed; }
    void SetMinMaxLifeTime(Vec2 _vLifeTime) { m_Param.v2Arr[1] = _vLifeTime; }
    void Set3DParticle(int _iOption) { m_Param.iArr[2] = _iOption; }
    void SetWorldMat(const Matrix& _mat) { m_Param.matArr[0] = _mat; }

public:
    virtual void UpdateData() override;
    virtual void Clear() override;

public:
    CParticleUpdateShader();
    ~CParticleUpdateShader();
};

