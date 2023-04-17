#pragma once
#include "CCamScript.h"

class CGameObject;

class CLinkCamScript :
    public CCamScript
{
private:
    CGameObject*    m_pLinkObj;

    Vec2            m_vDirXZ;
    float           m_fDistFromLink;

public:
    void Move();

    const Vec2& GetLinkDir() { return m_vDirXZ; }

public:
    virtual void begin() override;
    virtual void tick() override;

    virtual void BeginOverlap(CCollider* _pOther) override;
    virtual void Overlap(CCollider* _pOther) override;
    virtual void EndOverlap(CCollider* _pOther) override;

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CLinkCamScript)

public:
    CLinkCamScript();
    CLinkCamScript(const CLinkCamScript& _origin);
    ~CLinkCamScript();
};

