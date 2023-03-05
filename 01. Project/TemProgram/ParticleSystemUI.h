#pragma once
#include "ComponentUI.h"

#include <Engine/CParticleUpdateShader.h>

class ParticleSystemUI :
    public ComponentUI
{
private:
    int m_iMaxCount;
    int m_iAliveCount;

    Vec4 m_vStartScale;
    Vec4 m_vEndScale;

    Vec4 m_vStartColor;
    Vec4 m_vEndColor;

    Vec2 m_vMinMaxSpeed;
    Vec2 m_vMinMaxLifeTime;

    float m_fSpawnRange;

    float m_Frequency;
    bool  m_WorldSpawn;

    Ptr<CParticleUpdateShader>  m_UpdateCS;
    Ptr<CMaterial>  m_Material;

public:
    virtual void update() override;
    virtual void render_update() override;

public:
    void SetTexture(DWORD_PTR _strTextureKey);
    void SetMaterial(DWORD_PTR _strMaterialKey);
    void SetComputeShader(DWORD_PTR _strMaterialKey);

public:
    ParticleSystemUI();
    ~ParticleSystemUI();
};

