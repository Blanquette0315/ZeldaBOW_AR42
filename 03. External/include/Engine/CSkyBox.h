#pragma once
#include "CRenderComponent.h"

// HDR�� �̿��Ϸ���, �ϴõ� ���� ó���� ���־�� �Ѵ�.
// ��, �ϴ��� ��ü�߱��� �ϵ��� �̹̽ú긦 ����־�� �Ѵ�.
class CSkyBox :
    public CRenderComponent
{
private:
    SKYBOX_TYPE     m_eSkyBoxType;
    Ptr<CTexture>   m_pSkyBoxTex;

public:
    void SetSkyMesh();

    void SetType(SKYBOX_TYPE _type); // ��ī�� �ڽ� Ÿ���� ����ָ�, �ش� ��ī�� �ڽ� Ÿ�Կ� �˸��� �Ž��� ������ �������ִ� �Լ�
    SKYBOX_TYPE GetType() { return m_eSkyBoxType; }

    void SetSkyBoxTex(Ptr<CTexture> _Tex) { m_pSkyBoxTex = _Tex; }
    Ptr<CTexture> GetSkyBoxTex() { return m_pSkyBoxTex; }

    void SetTexture(Ptr<CTexture> _pTex);

public:
    virtual void finaltick() override;
    virtual void render() override;

public:

    CLONE(CSkyBox);
public:
    CSkyBox();
    ~CSkyBox();
};