#pragma once
#include "CEntity.h"

#include "CGameObject.h"

#define GET_OTHER_COMPONENT(ComponentName) C##ComponentName* ComponentName() { return m_pOwnerObject->ComponentName(); }


class CComponent :
    public CEntity
{
private:
    CGameObject*            m_pOwnerObject;
    const COMPONENT_TYPE    m_eType;
    bool                    m_bActive;
    bool                    m_bShowDebugDraw;   // Show DebugDraw : true is draw, false is nondraw Defulat is false

public:
    virtual void begin() {}
    virtual void tick() {}
    virtual void finaltick() = 0;

public:
    COMPONENT_TYPE GetType() { return m_eType; }
    CGameObject* GetOwner() { return m_pOwnerObject; }

    void Activate() { m_bActive = true; }
    void Deactivate() { m_bActive = false; }
    bool IsActivate() { return m_bActive; }

    void Destroy() { m_pOwnerObject->Destroy(); }

    GET_OTHER_COMPONENT(Transform);
    GET_OTHER_COMPONENT(MeshRender);
    GET_OTHER_COMPONENT(Camera);
    GET_OTHER_COMPONENT(Collider2D);
    GET_OTHER_COMPONENT(Animator2D);
    GET_OTHER_COMPONENT(Light2D);
    GET_OTHER_COMPONENT(Light3D);
    GET_OTHER_COMPONENT(ParticleSystem);
    GET_OTHER_COMPONENT(TileMap);
    GET_OTHER_COMPONENT(Decal);
    GET_OTHER_COMPONENT(SkyBox);
    GET_OTHER_COMPONENT(LandScape);

public:
    // Debug Draw Function
    void ShowDebugDraw(bool _ShowDebugDraw) { m_bShowDebugDraw = _ShowDebugDraw; }
    bool Is_ShowDebugDraw() { return m_bShowDebugDraw; }

    virtual CComponent* Clone() = 0;

public:
    CComponent(COMPONENT_TYPE _eType);
    CComponent(const CComponent& _origin);
    virtual ~CComponent();

    friend class CGameObject;
};

