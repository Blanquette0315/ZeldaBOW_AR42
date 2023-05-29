#pragma once
#include "CComponent.h"

#include "Ptr.h"
#include "CMesh.h"
#include "CMaterial.h"

class CLight3D :
    public CComponent
{
private:
    tLightInfo      m_Info;         // 광원 정보
    Ptr<CMesh>      m_pVolumeMesh;  // 광원의 범위를 표현해주는 매쉬
    Ptr<CMaterial>  m_pLightMtrl;

    int             m_iLightIdx;    // 광원 버퍼에서의 인덱스를 알기 위함. (MRT 구조로 랜더링을 바꾸면서 for문을 돌지 않으니 알아주어야 한다.)

    CGameObject*    m_pLightCam;    // 광원 시점용 카메라
    
    bool            m_bToon_ONOFF;
    bool            m_bShadow_ONOFF;

public:
    const tLightInfo& GetLightInfo() { return m_Info; }

    void SetLightDirection(Vec3 _vDir);
    void SetLightColor(Vec3 _vColor) { m_Info.vDiff = _vColor; }
    void SetLightSpecular(Vec3 _vSpec) { m_Info.vSpec = _vSpec; }
    void SetLightAmbient(Vec3 _vColor) { m_Info.vEmb = _vColor; }
    void SetLightType(LIGHT_TYPE _eType);

    void SetRadius(float _fRadius) { m_Info.fRadius = _fRadius; }
    void SetInAngle(float _fInAngle) { m_Info.fInAngle = _fInAngle; }
    void SetOutAngle(float _fOutAngle) { m_Info.fOutAngle = _fOutAngle; }

    Vec3 GetLightColor() { return m_Info.vDiff; }
    Vec3 GetLightSpec() { return m_Info.vSpec; }
    Vec3 GetLightEmbient() { return m_Info.vEmb; }
    LIGHT_TYPE GetLightType() { return m_Info.iLightType; }
    float GetRadius() { return m_Info.fRadius; }
    float GetInAngle() { return m_Info.fInAngle; }
    float GetOutAngle() { return m_Info.fOutAngle; }

    void CalcuRelativeScale();

public:
    void SetPTOption_Shadow(bool _bShadoow) { m_bShadow_ONOFF = _bShadoow; }
    void SetPTOption_Toon(bool _bToon) { m_bToon_ONOFF = _bToon; }

public:
    virtual void finaltick() override;
    void render();
    void render_depthmap();
    void render_staticdepthmap();

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CLight3D);
public:
    CLight3D();
    CLight3D(const CLight3D& _origin);
    ~CLight3D();
};