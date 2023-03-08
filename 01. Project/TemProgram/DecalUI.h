#pragma once
#include "ComponentUI.h"
class DecalUI :
    public ComponentUI
{
private:
    Ptr<CMesh>      m_Mesh;
    Ptr<CMaterial>  m_Material;
    Ptr<CTexture>   m_DecalTexture;
    TEX_PARAM       m_eSelectTexParam;
    bool            m_bIsDyanmicMtrl;

    bool            m_bDeferred;

    bool            m_bShowDebugDraw;

public:
    virtual void update() override;
    virtual void render_update() override;

public:
    void SetMesh(DWORD_PTR _strMeshKey);
    void SetMaterial(DWORD_PTR _strMaterialKey);

    void SetTexture(DWORD_PTR _strTexKey);
    void ShowShaderParam(CMaterial* _pMtrl);

public:
    DecalUI();
    ~DecalUI();
};

