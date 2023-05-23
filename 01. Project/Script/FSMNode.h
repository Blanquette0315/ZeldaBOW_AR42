#pragma once

#include "CLinkAnimScript.h"

class CAnimation3D;

class tTransition
{
public:
    CAnimation3D*               pTargetAnim;
    wstring                     pTargetAnimKey;

    UINT                        iTranCond;    // determine specific value each of bit
    UINT                        iExcludeCond; // determine position of bit that should be compared

public:
    // _Include : bit that should be 1, _IncludeZero : bit that should be 0
    void AddInclude(UINT _Include, UINT _IncludeZero = 0) { AddBit(iTranCond, _Include); AddBit(iExcludeCond, ~(_Include | _IncludeZero)); }

public:
    tTransition(CAnimation3D* _pAnim);
    ~tTransition();

    friend class CLinkAnimScript;
};

class tAnimNode
{
public:
    CAnimation3D*           pAnim;
    wstring                 pAnimKey;

    UINT                    iPreferences;

    vector<tTransition*>    vecTranAnim;

    Link_Func               Func_Start;
    Link_Func               Func_Steady;
    Link_Func               Func_End;
    Link_Func               Func_Exit;

    // tAnimNode* arrReserveAnim[2];

public:
    void AddPreferences(UINT _Preferences) { AddBit(iPreferences, _Preferences); }
    void AddFuncStart(Link_Func _func) { Func_Start = _func; }
    void AddFuncSteady(Link_Func _func) { Func_Steady = _func; }
    void AddFuncEnd(Link_Func _func) { Func_End = _func; }
    void AddFuncExit(Link_Func _func) { Func_Exit = _func; }

    UINT GetPreference() { return iPreferences; }
    const wstring& GetKey() { return pAnimKey; }

public:
    tAnimNode()
        : iPreferences(0)
        , Func_Start(nullptr)
        , Func_Steady(nullptr)
        , Func_End(nullptr)
        , pAnim()
        , pAnimKey()
        , vecTranAnim{}
        
    {

    }

    ~tAnimNode()
    {
        Safe_Del_Vec(vecTranAnim);
    }

    friend class CLinkAnimScript;
};

