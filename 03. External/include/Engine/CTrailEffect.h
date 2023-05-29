#pragma once
#include "CRenderComponent.h"

class CStructuredBuffer;
class CTrailUpdateShader;

class CTrailEffect :
    public CRenderComponent
{
private:
    vector<tTwoDot>     m_vecPosData;
    bool                m_bTrail;

    CTrailUpdateShader* m_pUpdateShader;
    CStructuredBuffer*  m_VecPosBuffer;
    CStructuredBuffer*  m_OutputBuffer;

    int                 m_iDividePoint;
    float               m_fRatio;

public:
    void PushData(tTwoDot _twoDot);
    void Clear() { m_vecPosData.clear(); }

    void SetTrailStart() { m_bTrail = true; }
    void SetTrailSTop() { m_bTrail = false; Clear(); }

    void SetRatio(float _fRatio) { m_fRatio = _fRatio; }

    virtual void SetMesh(Ptr<CMesh> _pMesh) override;

private:
    void EditIdxBuffer();

public:
    virtual void tick() override;
    virtual void finaltick() override;
    void render();
    virtual void render(UINT _iSubset) override;

    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CTrailEffect);

public:
    CTrailEffect();
    CTrailEffect(const CTrailEffect& _origin);
    virtual ~CTrailEffect();
};

