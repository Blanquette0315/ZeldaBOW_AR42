#pragma once
#include "CRenderComponent.h"

// HDR를 이용하려면, 하늘도 광원 처리를 해주어야 한다.
// 즉, 하늘이 자체발광을 하도록 이미시브를 잡아주어야 한다.
class CSkyBox :
    public CRenderComponent
{
private:
    SKYBOX_TYPE     m_eSkyBoxType;
    Ptr<CTexture>   m_pSkyBoxTex;

public:
    void SetSkyMesh();

    void SetType(SKYBOX_TYPE _type); // 스카이 박스 타입을 잡아주면, 해당 스카이 박스 타입에 알맞은 매쉬를 선택해 설정해주는 함수
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