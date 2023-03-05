#pragma once
#include "CEntity.h"

class CGameObject;

class CLayer :
    public CEntity
{
private:
    vector<CGameObject*> m_vecParent;   // 최상위 부모만
    vector<CGameObject*> m_vecObjects;  // 부모, 자식 상관없이 모든 오브젝트

    int                  m_iLayerIdx;

public:
    void begin();
    void tick();
    void finaltick();

public:
    void AddGameObject(CGameObject* _pObject);
    int GetLayerIdx() { return m_iLayerIdx; }

    const vector<CGameObject*>& GetParentObject() { return m_vecParent; }
    const vector<CGameObject*>& GetObjects() { return m_vecObjects; }

private:
    void RegisterObject(CGameObject* _pObj) { m_vecObjects.push_back(_pObj); }
    void DeregisterObject(CGameObject* _pObj);
    void clear() { m_vecObjects.clear(); }

    CLONE_DUMMY(CLayer);

public:
    CLayer();
    ~CLayer();

    friend class CLevel;
    friend class CGameObject;
};

