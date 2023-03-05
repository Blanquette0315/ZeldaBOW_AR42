#pragma once
#include "ComponentUI.h"

#include <Engine/CMeshRender.h>

class MeshRenderUI :
    public ComponentUI
{
private:
    Ptr<CMesh>      m_Mesh;
    Ptr<CMaterial>  m_Material;

public:
    virtual void update() override;
    virtual void render_update() override;

public:
    void SetMesh(DWORD_PTR _strMeshKey);
    void SetMaterial(DWORD_PTR _strMaterialKey);

public:
    MeshRenderUI();
    ~MeshRenderUI();
};

