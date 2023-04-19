#pragma once
#include <Engine/CScript.h>

class CAnimator3D;
class CAnimation3D;
class tAnimNode;

class CLinkAnimScript :
    public CScript
{
private:
    static map<wstring, CAnimation3D*>  m_mapAnim;
    static map<wstring, tAnimNode*>     m_mapAnimNode;
    static bool                         m_bIsAnimNodeLoaded;

private:
    CAnimator3D*    m_pAnimator;
    tAnimNode*      m_pCurAnimNode;
    tAnimNode*      m_pNextAnimNode;
    UINT            m_iCond;

private:
    float           m_fAnglePerSec;
    float           m_fSpeed;

private:
    void CreateAnimNode();
    void MakeFSM();
    void SetAnimNode(tAnimNode*& _pAnimNode, LINK_ANIM_TYPE _eAnim);
    void SetAnimNode(tAnimNode*& _pAnimNode, LINK_ANIM_TYPE _eAnim, UINT _ePref);
    void SetAnimTran(tAnimNode* _pAnimNode, LINK_ANIM_TYPE _eAnim, UINT _eIncludeAs1Bit, UINT _eIncludeAs0Bit = (LINK_ANIM_CONDITION)0);
    
private:
    void PlayNextAnim();
    void OperateAnimFunc();
    void SetLinkCond();
    void SetNextAnim();

private:
    void SetRotation();
    void Func_Walk();

public:
    static void ClearAnimNode();

public:
    virtual void init() override;
    virtual void begin() override;
    virtual void tick() override;

    virtual void BeginOverlap(CCollider* _pOther) override;
    virtual void Overlap(CCollider* _pOther) override;
    virtual void EndOverlap(CCollider* _pOther) override;

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CLinkAnimScript)

public:
    CLinkAnimScript();
    CLinkAnimScript(const CLinkAnimScript& _origin);
    ~CLinkAnimScript();
};

