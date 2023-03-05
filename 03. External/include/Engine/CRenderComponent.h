#pragma once
#include "CComponent.h"

#include "CMesh.h"
#include "CMaterial.h"

class CRenderComponent :
    public CComponent
{
private:
    Ptr<CMesh>              m_pMesh;

    Ptr<CMaterial>          m_pSharedMtrl; // 공유 재질
    Ptr<CMaterial>          m_pDynamicMtrl; // 동적 재질
    Ptr<CMaterial>          m_pCurMtrl;     // 현재 사용 중인 재질

public:
    virtual void render() = 0;

public:
    void SetMesh(Ptr<CMesh> _pMesh) { m_pMesh = _pMesh; }
    Ptr<CMesh> GetMesh() { return m_pMesh; }

    void SetSharedMaterial(Ptr<CMaterial> _pMtrl) { m_pSharedMtrl = _pMtrl; ; m_pCurMtrl = _pMtrl;}
    Ptr<CMaterial> GetSharedMaterial();

    Ptr<CMaterial> GetCurMaterial() { return m_pCurMtrl; }
    Ptr<CMaterial> GetDynamicMaterial();

public:
    virtual void SaveToFile(FILE* _File) override;
    virtual void LoadFromFile(FILE* _File) override;

public:
    virtual CRenderComponent* Clone() = 0;

public:
    CRenderComponent(COMPONENT_TYPE _eComponentType);
    CRenderComponent(const CRenderComponent& _origin);
    ~CRenderComponent();
};

