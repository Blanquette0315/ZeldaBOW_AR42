#pragma once
#include "CRenderComponent.h"

// 지금 데칼은 세로로 겹친 영역을 표시해주는 데칼로 설계해 주었기 떄문에
// 가로로 출력해야할 때는 데칼 오브젝트를 회전시켜주어야 제대로 나온다.
// 이유는 데칼의 UV를 x,y 축으로 계산했기 때문이다.
// 만약 회전을 안한다면 세로일 경우 z,x로 계산해주어야 한다.
// Unreal Engine의 경우도 데칼 오브젝트를 회전해 사용하는 방식이다.

class CDecal :
    public CRenderComponent
{
private:
    Ptr<CTexture>   m_pDecalTex;
    bool            m_bLighting;    // 정확히는 Forward로 랜더링할지, Deferred로 랜더링할지

public:
    void SetRenderType(bool _bSet); // m_bLgihting을 즉, forward방식으로 혹은 Deferred 방식으로 랜더링할지에 따라 알맞은 재질을 선택해준다.
    bool IsDeferred() { return m_bLighting; }
    void SetDecalTexture(Ptr<CTexture> _Tex) { m_pDecalTex = _Tex; }

public:
    virtual void finaltick() override;
    virtual void render() override;

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;
    CLONE(CDecal);
public:
    CDecal();
    ~CDecal();
};