#pragma once
#include "CRenderComponent.h"

// ��ī�� �ڽ��� ť�� �Žÿ� Sphere 2������ ���� �� �ִ�.
enum class SKYBOX_TYPE
{
    SPHERE,
    CUBE,
};

// HDR�� �̿��Ϸ���, �ϴõ� ���� ó���� ���־�� �Ѵ�.
// ��, �ϴ��� ��ü�߱��� �ϵ��� �̹̽ú긦 ����־�� �Ѵ�.
class CSkyBox :
    public CRenderComponent
{
private:
    SKYBOX_TYPE     m_eSkyBoxType;
    Ptr<CTexture>   m_pSkyBoxTex;

public:
    void SetType(SKYBOX_TYPE _type); // ��ī�� �ڽ� Ÿ���� ����ָ�, �ش� ��ī�� �ڽ� Ÿ�Կ� �˸��� �Ž��� ������ �������ִ� �Լ�
    void SetSkyBoxTex(Ptr<CTexture> _Tex) { m_pSkyBoxTex = _Tex; }

public:
    virtual void finaltick() override;
    virtual void render() override;

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;
    CLONE(CSkyBox);
public:
    CSkyBox();
    ~CSkyBox();
};