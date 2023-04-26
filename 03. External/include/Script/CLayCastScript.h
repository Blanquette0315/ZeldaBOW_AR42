#pragma once
#include <Engine/CScript.h>

class CLayCastScript :
    public CScript
{
private:
    map<wstring, PhysRayCast*> m_mapRay;

public:
    PhysRayCast* CrateRay(const wstring& _strKey);
    PhysRayCast* FindRay(const wstring& _strKey);

    void RayCastAll();
    bool RayCast(const wstring& _strKey);

public:
    bool SetRayStartPoint(const wstring& _strKey, Vec3 _vStartPos);
    bool SetRayDirPoint(const wstring& _strKey, Vec3 _vRayDir);
    bool SetMaxDistance(const wstring& _strKey, float _fDistance);
    bool SetRayFilter(const wstring& _strKey, FILTER_GROUP _Filter);

public:
    virtual void begin() override;
    virtual void tick() override;

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CLayCastScript);
public:
    CLayCastScript();
    ~CLayCastScript();
};