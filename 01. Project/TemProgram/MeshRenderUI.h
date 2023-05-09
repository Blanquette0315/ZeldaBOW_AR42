#pragma once
#include "ComponentUI.h"

#include <Engine/CMeshRender.h>

class ComboBoxUI;

class MeshRenderUI :
    public ComponentUI
{
private:
    TEX_PARAM   m_eSelectTexParam;

    Ptr<CMesh>      m_Mesh;
    Ptr<CMaterial>  m_Material;
    
    bool            m_bIsDyanmicMtrl;
    int             m_iOption;

public:
    virtual void update() override;
    virtual void render_update() override;

public:
    void SetMesh(DWORD_PTR _strMeshKey);
    void SetMaterial(DWORD_PTR _strMaterialKey);

    void SetTexture(DWORD_PTR _strTexKey);
    void ShowShaderParam(CMaterial* _pMtrl);

public:
    MeshRenderUI();
    ~MeshRenderUI();
};

