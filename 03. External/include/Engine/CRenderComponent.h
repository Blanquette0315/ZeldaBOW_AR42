#pragma once
#include "CComponent.h"

#include "CMesh.h"
#include "CMaterial.h"

class CRenderComponent :
    public CComponent
{
private:
    Ptr<CMesh>              m_pMesh;

    Ptr<CMaterial>          m_pSharedMtrl;      // ���� ����
    Ptr<CMaterial>          m_pDynamicMtrl;     // ���� ����
    Ptr<CMaterial>          m_pCurMtrl;         // ���� ��� ���� ����

    bool                    m_bIsDynamicMtrl;
    bool                    m_bDynamicShadow;

public:
    virtual void render() = 0;
    virtual void render_depthmap();

public:
    void SetMesh(Ptr<CMesh> _pMesh) { m_pMesh = _pMesh; }
    Ptr<CMesh> GetMesh() { return m_pMesh; }

    void SetSharedMaterial(Ptr<CMaterial> _pMtrl) { m_pSharedMtrl = _pMtrl; ; m_pCurMtrl = _pMtrl;}
    Ptr<CMaterial> GetSharedMaterial();

    Ptr<CMaterial> GetCurMaterial() { return m_pCurMtrl; }
    Ptr<CMaterial> GetDynamicMaterial();

    bool IsDynamicMtrl() { return m_bIsDynamicMtrl; }

    void SetDynamicShadow(bool _bSet) { m_bDynamicShadow = _bSet; }
    bool IsDynamicShadow() { return m_bDynamicShadow; }

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

