#pragma once
#include <Engine/CScript.h>
class CBonesocketScript :
    public CScript
{
private:
    Vec3     m_vOffsetPos;
    Vec3     m_vOffsetRot;
    UINT     m_iBoneIdx;

public:
    void setOffsetPos(Vec3 _vOffsetPos) { m_vOffsetPos = _vOffsetPos; }
    void setOffsetRot(Vec3 _vOffsetRot) { m_vOffsetRot = _vOffsetRot; }
    void setBoneIdx(UINT _iBoneIdx) { m_iBoneIdx = _iBoneIdx; }

public:
    virtual void tick() override;
    virtual void finaltick() override;

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CBonesocketScript);
public:
    CBonesocketScript();
    ~CBonesocketScript();
};

