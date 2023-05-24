#pragma once
#include "CComponent.h"

#include "CMesh.h"
#include "CMaterial.h"

struct tMtrlSet
{
    Ptr<CMaterial>  pCurMtrl;       // 현재 사용 할 메테리얼
    Ptr<CMaterial>  pSharedMtrl;    // 공유 메테리얼
    Ptr<CMaterial>  pDynamicMtrl;   // 공유 메테리얼의 복사본    
};

class CRenderComponent :
    public CComponent
{
protected:
    Ptr<CMesh>              m_pMesh;
    vector<tMtrlSet>        m_vecMtrls;

    bool                    m_bIsDynamicMtrl;
    bool                    m_bDynamicShadow;
    bool                    m_bStaticShadow;

    bool                    m_bIsRender;

public:
    virtual void render() = 0;
    virtual void render(UINT _iSubset) = 0;
    virtual void render_depthmap();

public:
    virtual void SetMesh(Ptr<CMesh> _pMesh);
    Ptr<CMesh> GetMesh() { return m_pMesh; }

    UINT GetMtrlCount() { return (UINT)m_vecMtrls.size(); }

    void SetSharedMaterial(Ptr<CMaterial> _pMtrl, UINT _iIdx = 0);

    Ptr<CMaterial> GetCurMaterial(UINT _iIdx = 0);
    Ptr<CMaterial> GetSharedMaterial(UINT _idx = 0);
    Ptr<CMaterial> GetDynamicMaterial(UINT _iIdx = 0);

    bool IsDynamicMtrl() { return m_bIsDynamicMtrl; }

    void SetDynamicShadow(bool _bSet) { m_bDynamicShadow = _bSet; }
    bool IsDynamicShadow() { return m_bDynamicShadow; }
    bool IsStaticShadow() { return m_bStaticShadow; }

    ULONG64 GetInstID(UINT _iMtrlIdx);

    void SetRender(bool _b) { m_bIsRender = _b; }
    bool IsRender() { return m_bIsRender; }

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

public:
    virtual CRenderComponent* Clone() = 0;

public:
    CRenderComponent(COMPONENT_TYPE _eComponentType);
    CRenderComponent(const CRenderComponent& _origin);
    ~CRenderComponent();
};

