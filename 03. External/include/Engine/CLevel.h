#pragma once
#include "CEntity.h"
#include "CLayer.h"

class CGameObject;

class CLevel :
    public CEntity
{
private:
    CLayer      m_arrLayer[MAX_LAYER];
    wstring     m_strRelativePath;
    LEVEL_STATE m_State;

    vector<tLayerCollision> m_vecLayerCollsionSet; // Level ������ �浹 ���̾� ������.

public:
    // ���� �Լ�
    void begin();
    void tick();
    void finaltick();

    void ClearLayer();

public:
    void RefreshCollsionSet();

public:
    void SetState(LEVEL_STATE _State) { m_State = _State; }
    LEVEL_STATE GetState() { return m_State; }

    CLayer* GetLayer(UINT _iIdx) { return &m_arrLayer[_iIdx]; }
    CLayer* GetLayer(const wstring& _strLayerName);

    void AddGameObject(CGameObject* _pObjet, UINT _iLayerIdx);
    void AddGameObejct(CGameObject* _pObject, const wstring& _strLayerName);

    void ChangeObjectLayer(CGameObject* _pObject, int _Idx);

    CGameObject* FindObjectByName(const wstring& _name);
    void FindObjectByName(const wstring& _name, vector<CGameObject*>& _out);

    const vector<CGameObject*> GetGameObjects();
    const vector<CGameObject*> GetRigidObjects();

    vector<tLayerCollision> GetLayerCollsionSet() { return m_vecLayerCollsionSet; }
    void SetLayerCollsionSet(tLayerCollision _tLayerCollsion) { m_vecLayerCollsionSet.push_back(_tLayerCollsion); }
    void ClearLayerCollision() { m_vecLayerCollsionSet.clear(); }

    void SetRelativePath(const wstring& _strRelativePath) { m_strRelativePath = _strRelativePath; }
    const wstring& GetRelativePath() { return m_strRelativePath; }

public:
    void RegisterObjects();

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE_DUMMY(CLevel);
 
public:
    CLevel();
    virtual ~CLevel();

    friend class CLevelMgr;
};

