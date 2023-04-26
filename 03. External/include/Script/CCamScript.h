#pragma once
#include <Engine/CScript.h>
class CCamScript :
    public CScript
{
protected:
    CAMERA_SELECTION m_eCameraType;

public:
    CAMERA_SELECTION GetCamType() { return m_eCameraType; }

protected:
    void RegisterCamera();

public:
    // i wanted to make this function to pure virtual function.
    // but pure virtual function is not available because of codegen.exe...
    // allocate m_eCameraType before RegisterCamera();
    virtual void init() { assert(nullptr); }; 

    virtual void begin() override;
    virtual void tick() override;

    virtual void BeginOverlap(CGameObject* _pOther) override;
    virtual void Overlap(CGameObject* _pOther) override;
    virtual void EndOverlap(CGameObject* _pOther) override;

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CCamScript)

public:
    CCamScript();
    CCamScript(SCRIPT_TYPE _type);
    CCamScript(const CCamScript& _origin);
    ~CCamScript();
};

