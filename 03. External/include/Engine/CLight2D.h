#pragma once
#include "CComponent.h"

// 해당 컴포넌트를 가지고 있는 GameObject들은 광원 효과를 갖는 오브젝트이다.
// 2D광원 구조화 버퍼는 t13 레지스터에 바인딩 건다.
// 3D광원 구조화 버퍼는 t14 레지스터에 바인딩 건다.

class CLight2D :
    public CComponent
{
private:
    tLightInfo  m_Info; // 광원 정보

public:
    const tLightInfo& GetLightInfo() { return m_Info; }

    // 구조체의 정보를 셋팅해주고, 가져오는 함수들 
    void SetLightColor(Vec3 _vColor) { m_Info.vDiff = _vColor; }
    void SetLightSpec(Vec3 _vColor) { m_Info.vSpec = _vColor; }
    void SetLightEmbient(Vec3 _vColor) { m_Info.vEmb = _vColor; }

    void SetLightType(LIGHT_TYPE _eType) { m_Info.iLightType = _eType; }

    void SetRadius(float _fRadius) { m_Info.fRadius = _fRadius; }
    void SetAngle(float _fAngle) { m_Info.fInAngle = _fAngle; }

    Vec3 GetLightColor() { return m_Info.vDiff; }
    Vec3 GetLightSpec() { return m_Info.vSpec; }
    Vec3 GetLightEmbient() { return m_Info.vEmb; }
    LIGHT_TYPE GetLightType() { return m_Info.iLightType; }
    float GetRadius() { return m_Info.fRadius; }
    float GetAngle() { return m_Info.fInAngle; }

public:
    virtual void finaltick() override;

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;
    CLONE(CLight2D);

public:
    CLight2D();
    ~CLight2D();
};

