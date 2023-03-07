#pragma once
#include "CComponent.h"

// �ش� ������Ʈ�� ������ �ִ� GameObject���� ���� ȿ���� ���� ������Ʈ�̴�.
// 2D���� ����ȭ ���۴� t13 �������Ϳ� ���ε� �Ǵ�.
// 3D���� ����ȭ ���۴� t14 �������Ϳ� ���ε� �Ǵ�.

class CLight2D :
    public CComponent
{
private:
    tLightInfo  m_Info; // ���� ����

public:
    const tLightInfo& GetLightInfo() { return m_Info; }

    // ����ü�� ������ �������ְ�, �������� �Լ��� 
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

