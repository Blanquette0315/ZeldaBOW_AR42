#pragma once
#include "CRes.h"

class CGameObject;

typedef void (*SAVE_GAMEOBJECT)(CGameObject*, FILE*);
typedef CGameObject* (*LOAD_GAMEOBJECT)(FILE*);

class CPrefab :
    public CRes
{
public:
    static SAVE_GAMEOBJECT Save_GameObject_Func;
    static LOAD_GAMEOBJECT Load_GameObject_Func;

private:
    CGameObject* m_pProtoObj;

public:
    CGameObject* Instantiate();

    virtual void Save(const wstring& _strRelativePath) override;
    virtual int Load(const wstring& _strFilePath) override;

    CLONE_DUMMY(CPrefab);

public:
    CPrefab();
    CPrefab(CGameObject* _pProto);
    ~CPrefab();
};