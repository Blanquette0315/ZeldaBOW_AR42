#pragma once
#include "CEntity.h"

class CComponent;
class CTransform;
class CRenderComponent;
class CMeshRender;
class CCollider;
class CAnimator2D;
class CAnimator3D;
class CCamera;
class CScript;
class CLight2D;
class CLight3D;
class CParticleSystem;
class CTileMap;
class CDecal;
class CSkyBox;
class CLandScape;
class CRigidBody;

#define GET_COMPONENT(classname, CLASSNAME ) C##classname* classname() { return ( C##classname*)GetComponent(COMPONENT_TYPE::CLASSNAME); }

class CGameObject :
    public CEntity
{
private:
    CGameObject*                m_pParent;

    CComponent*                 m_arrCom[(UINT)COMPONENT_TYPE::END];
    vector<CScript*>            m_vecScripts;

    CRenderComponent*           m_pRenderComponent;

    int                         m_iLayerIdx;
    bool                        m_bDead;
    bool                        m_bRender;  // 랜더를 할 지 말지 정해주는 변수 기본은 true
    bool                        m_bFrustumCul; // CameraFrustum Culling을 할지 말지

    Ptr<CPrefab>                m_OwnerPrefab;

    int m_iTestCount = 0;

public:
    void SetTestCount(int _iCount) { m_iTestCount = _iCount; }
    int GetTestCount() { return m_iTestCount; }

protected:
    vector<CGameObject*>        m_vecChild;

public:
    void begin();
    void tick();
    void finaltick();
    void finaltick_module();    // 레벨에 포함되지 않고 사용 하는 경우
    void render();

public:
    void SetFrustumCul(bool _Culling) { m_bFrustumCul = _Culling; }
    bool IS_FrustumCul() { return m_bFrustumCul; }

// 프리펩 전환
public:
    void ChangePrefab();
    const Ptr<CPrefab> GetOwnerPrefab() { return m_OwnerPrefab; }
    
    // 랜더링을 할지 안할지 정해준다.
    void SetRender(bool _render) { m_bRender = _render; }

public:
    void AddComponent(CComponent* _pComponent);
    void ReleaseComponent(CComponent* _pComponent);
    CComponent* GetComponent(COMPONENT_TYPE _eComType) { return m_arrCom[(UINT)_eComType]; }
    void AddChild(CGameObject* _pChild);

    // CCardMgr에서만 사용 해야한다.
    void ClearChild() { m_vecChild.clear(); }

    template<typename T>
    T* GetScript();

    GET_COMPONENT(Transform, TRANSFORM);
    GET_COMPONENT(MeshRender, MESHRENDER);
    GET_COMPONENT(Camera, CAMERA);
    GET_COMPONENT(Collider, COLLIDER);
    GET_COMPONENT(Animator2D, ANIMATOR2D);
    GET_COMPONENT(Animator3D, ANIMATOR3D);
    GET_COMPONENT(Light2D, LIGHT2D);
    GET_COMPONENT(Light3D, LIGHT3D);
    GET_COMPONENT(ParticleSystem, PARTICLESYSTEM);
    GET_COMPONENT(TileMap, TILEMAP);
    GET_COMPONENT(Decal, DECAL);
    GET_COMPONENT(SkyBox, SKYBOX);
    GET_COMPONENT(LandScape, LANDSCAPE);
    GET_COMPONENT(RigidBody, RIGIDBODY);

    CRenderComponent* GetRenderComponent() { return m_pRenderComponent; }

    CGameObject* GetParent() { return m_pParent; }
    const vector<CGameObject*>& GetChildObject()const { return m_vecChild; }
    CGameObject* GetChildObjByName(const wstring& _strName);

    // 자식 이름으로 자식을 찾아주는 기능 : 봐서 지워줘야 할듯.
    CGameObject* GetChildObject(const wstring& _strName);

    void DisconnectFromParent(); // 부모의 자식 vector에서 본인을 빼준다.

    const vector<CScript*>& GetScripts()const { return m_vecScripts; }

    // Script enum값 (Script의 ID)로 Script를 찾아서 반환해 준다.
    CScript* GetScript(int _ScriptType);

    int GetLayerIdx() { return m_iLayerIdx; }
    void SetLayerIdx(int _LayerIdx) { m_iLayerIdx = _LayerIdx; }
    void SetAllLayer(int _LayerIdx);

    bool IsDead() { return m_bDead; }
    void Destroy();

    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CGameObject);

public:
    CGameObject();
    CGameObject(const CGameObject& _origin);
    virtual ~CGameObject();

    friend class CLayer;
    friend class CEventMgr;
    friend class CPrefab;
};

template<typename T>
inline T* CGameObject::GetScript()
{
    for (size_t i = 0; i < m_vecScripts.size(); ++i)
    {
        T* pScript = dynamic_cast<T*>(m_vecScripts[i]);
        if (nullptr != pScript)
            return pScript;
    }
    return nullptr;
}
