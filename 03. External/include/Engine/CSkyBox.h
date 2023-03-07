#pragma once
#include "CRenderComponent.h"

// 스카이 박스는 큐브 매시와 Sphere 2가지로 만들 수 있다.
enum class SKYBOX_TYPE
{
    SPHERE,
    CUBE,
};

// HDR를 이용하려면, 하늘도 광원 처리를 해주어야 한다.
// 즉, 하늘이 자체발광을 하도록 이미시브를 잡아주어야 한다.
class CSkyBox :
    public CRenderComponent
{
private:
    SKYBOX_TYPE     m_eSkyBoxType;
    Ptr<CTexture>   m_pSkyBoxTex;

public:
    void SetType(SKYBOX_TYPE _type); // 스카이 박스 타입을 잡아주면, 해당 스카이 박스 타입에 알맞은 매쉬를 선택해 설정해주는 함수
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