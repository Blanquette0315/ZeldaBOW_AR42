#pragma once
#include "CComponent.h"

#include "CKeyMgr.h"
#include "CTimeMgr.h"
#include "CResMgr.h"

#include "GlobalComponent.h"


// 스크립트의 공개 파라미터를 출력하기 위한 파라미터 enum class
// 파라미터의 용도이다.
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

// 파라미터를 UI에 공개하기 위한 구조체
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
    const int               m_iScriptType; // UI의 메뉴바에 에디터용 스크립트는 보이면 안되기 때문에 -1을 넣어줄 것이다.

    vector<tScriptParam>    m_vecParam;

public:
    int GetScriptType() { return m_iScriptType; }
    void AddScriptParam(SCRIPT_PARAM _eParam, const string& _Name, void* _pParam) { m_vecParam.push_back(tScriptParam{_eParam, _Name, _pParam }); }
    const vector<tScriptParam>& GetScriptParam() { return m_vecParam; }

public:
    virtual void tick() = 0;
    virtual void finaltick() final {};

public:
    virtual void BeginOverlap(CCollider2D* _pOther) {}
    virtual void Overlap(CCollider2D* _pOther) {}
    virtual void EndOverlap(CCollider2D* _pOther) {}

public:
    virtual CScript* Clone() = 0;

public:
    CScript(int _iScriptType);
    ~CScript();
};

