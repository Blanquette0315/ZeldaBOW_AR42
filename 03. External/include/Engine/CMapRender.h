#pragma once
#include "CRenderComponent.h"

#include "CMesh.h"
#include "CMaterial.h"

class CMapRender :
    public CRenderComponent
{
public:
    virtual void tick() override;
    virtual void finaltick() override;
    void render();
    virtual void render(UINT _iSubset) override;

    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CMapRender);

public:
    CMapRender();
    virtual ~CMapRender();
};