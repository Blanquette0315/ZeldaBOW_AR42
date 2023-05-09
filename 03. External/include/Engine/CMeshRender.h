#pragma once
#include "CRenderComponent.h"

#include "CMesh.h"
#include "CMaterial.h"

class CMeshRender :
    public CRenderComponent
{
private:
    // 0 -> none | 1 -> not render overlaped material
    int m_iOption; 
    vector<bool> m_vecCheck;
    vector<CGameObject*> m_vecPrevChild;

public:
    int GetOption() { return m_iOption; }
    void SetOption(int _iOption) { m_iOption = _iOption; }

public:
    virtual void tick() override;
    virtual void finaltick() override;
    void render();

    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CMeshRender);

public:
    CMeshRender();
    virtual ~CMeshRender();


};

