#pragma once
#include "CComponent.h"

#include "Ptr.h"
#include "CMesh.h"
#include "CMaterial.h"

class CLight3D :
    public CComponent
{
private:
    tLightInfo      m_Info;         // ���� ����
    Ptr<CMesh>      m_pVolumeMesh;  // ������ ������ ǥ�����ִ� �Ž�
    Ptr<CMaterial>  m_pLightMtrl;

    int             m_iLightIdx;    // ���� ���ۿ����� �ε����� �˱� ����. (MRT ������ �������� �ٲٸ鼭 for���� ���� ������ �˾��־�� �Ѵ�.)

public:
    const tLightInfo& GetLightInfo() { return m_Info; }

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
    virtual void finaltick() override;
    void render();

public:
    virtual void SaveToFile(FILE* _File) override;
    virtual void LoadFromFile(FILE* _File) override;

    CLONE(CLight3D);
public:
    CLight3D();
    ~CLight3D();
};