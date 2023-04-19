#pragma once

class CAnimation3D;

class tTransition
{
private:
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
private:
    CAnimation3D*           pAnim;
    wstring                 pAnimKey;

    UINT                    iPreferences;

    vector<tTransition*>    vecTranAnim;

    // tAnimNode* arrReserveAnim[2];

public:
    void AddPreferences(UINT _Preferences) { AddBit(iPreferences, _Preferences); }

public:
    tAnimNode()
        : iPreferences(0)
    {

    }

    ~tAnimNode()
    {
        Safe_Del_Vec(vecTranAnim);
    }

    friend class CLinkAnimScript;
};

