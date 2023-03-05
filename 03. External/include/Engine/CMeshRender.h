#pragma once
#include "CRenderComponent.h"

#include "CMesh.h"
#include "CMaterial.h"

class CMeshRender :
    public CRenderComponent
{
public:
    virtual void tick() override;
    virtual void finaltick() override;
    void render();

    CLONE(CMeshRender);

public:
    CMeshRender();
    virtual ~CMeshRender();
};

