#pragma once

#include <Engine\CScript.h>

class CRigidBody;
class CCollider;

class CLinkArrowScript :
    public CScript
{
    float m_fGravity;
    float m_vVelocity;
    Vec3  m_vDir;

    float m_fCurGravity;
    float m_fMaxGravity;

    Vec3 m_vPrevPos;
    
    // not save
private:


public:
    void SetDir(Vec3 _vDir) { m_vDir = _vDir; }

public:
    virtual void begin() override;
    virtual void tick() override;

    virtual void BeginOverlap(CGameObject* _pOther) override;
    virtual void Overlap(CGameObject* _pOther) override;
    virtual void EndOverlap(CGameObject* _pOther) override;

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CLinkArrowScript)

public:
    CLinkArrowScript();
    CLinkArrowScript(const CLinkArrowScript& _origin);
    ~CLinkArrowScript();
};

