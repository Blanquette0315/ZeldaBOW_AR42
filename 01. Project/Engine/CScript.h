#pragma once
#include "CComponent.h"

#include "CKeyMgr.h"
#include "CTimeMgr.h"
#include "CResMgr.h"

#include "GlobalComponent.h"


// ��ũ��Ʈ�� ���� �Ķ���͸� ����ϱ� ���� �Ķ���� enum class
// �Ķ������ �뵵�̴�.
enum class SCRIPT_PARAM
{
    INT,
    FLOAT,
    VEC2,
    VEC3,
    VEC4,
    TEXTURE,
    MATERIAL,
    PREFAB,
    SOUND,
};

// �Ķ���͸� UI�� �����ϱ� ���� ����ü
struct tScriptParam
{
    SCRIPT_PARAM    Type;
    string          strParamName;
    void*           pParam;
};

class CScript :
    public CComponent
{
private:
    const int               m_iScriptType; // UI�� �޴��ٿ� �����Ϳ� ��ũ��Ʈ�� ���̸� �ȵǱ� ������ -1�� �־��� ���̴�.

    vector<tScriptParam>    m_vecParam;

public:
    int GetScriptType() { return m_iScriptType; }
    void AddScriptParam(SCRIPT_PARAM _eParam, const string& _Name, void* _pParam) { m_vecParam.push_back(tScriptParam{_eParam, _Name, _pParam }); }
    const vector<tScriptParam>& GetScriptParam() { return m_vecParam; }

public:
    virtual void tick() = 0;
    virtual void finaltick() final {};

public:
    virtual void BeginOverlap(CGameObject* _pOther) {}
    virtual void Overlap(CGameObject* _pOther) {}
    virtual void EndOverlap(CGameObject* _pOther) {}

public:
    virtual CScript* Clone() = 0;

public:
    CScript(int _iScriptType);
    ~CScript();
};

