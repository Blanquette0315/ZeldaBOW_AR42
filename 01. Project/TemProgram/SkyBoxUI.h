#pragma once
#include "ComponentUI.h"
class SkyBoxUI :
    public ComponentUI
{
private:
    Ptr<CTexture> m_SkyBoxTex;
    SKYBOX_TYPE m_eType;

public:
    virtual void update() override;
    virtual void render_update() override;

private:
    void MtrlBtn();
    void TextureBtn();

private:
    void SetMesh(DWORD_PTR _strMeshKey);
    void SetMaterial(DWORD_PTR _strMaterialKey);
    void SetDynMaterialTex(DWORD_PTR _strTextureKey);
    void SetTexture(DWORD_PTR _strTexKey);

public:
    SkyBoxUI();
    ~SkyBoxUI();
};

