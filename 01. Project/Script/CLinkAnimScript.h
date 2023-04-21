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

    // not save
private:
    CAnimator3D*    m_pAnimator;
    tAnimNode*      m_pCurAnimNode;
    // tAnimNode*      m_pNextAnimNode;
    UINT            m_iCond;

    CGameObject*    m_pLinkCamObj;
    Vec3            m_vDir[(UINT)LINK_DIRECTION::END];

    // save
private:
    float           m_fAnglePerSec;

    float           m_fWalkSpeed;
    float           m_fRunSpeed;
    float           m_fDashSpeed;
    
    UINT            m_iMode;

    // FSM function
private:
    void CreateAnimNode();
    void MakeFSM();
    void SetAnimNode(tAnimNode*& _pAnimNode, LINK_ANIM_TYPE _eAnim);
    void SetAnimNode(tAnimNode*& _pAnimNode, LINK_ANIM_TYPE _eAnim, UINT _ePref);
    void SetAnimTran(tAnimNode* _pAnimNode, LINK_ANIM_TYPE _eAnim, UINT _eIncludeAs1Bit, UINT _eIncludeAs0Bit = (LINK_ANIM_CONDITION)0);
    
    // tick structure
private:
    /*void PlayNextAnim();*/
    void SetLinkCond();

    // This can use anim finished member 
    void OperateAnimFuncAfter();

    void PlayNextAnim();
    void OperateAnimFunc();

    // anim function
private:
    void SetRotation();

    void Func_WalkRunDash();
    void Func_TurnBack();

    // convenience function
private:
    // check current anim's member finish
    bool IsCurAnim(LINK_ANIM_TYPE _eLAT); 
    
    // Find which Link's toe is front 
    LINK_FRONT_TOE GetFrontToe(CAnimation3D* _pCurAnim);
    int FindClosestIdx(const vector<tMTKeyFrame>& _vec, float _fTime, int _iIdx, int _iStart, int _iEnd);

    // calc Link direction from camera
    void CalcMoveDirection();
    const Vec3& GetMoveDir(LINK_DIRECTION _eDir) { return m_vDir[(UINT)_eDir]; }

    // check angle between current rot and next rot (input by key)
    // if over 135degree return true
    bool ShouldTurnBack();

    // Get eight kind of dir from key combination ( camera is standard)
    Vec3 GetCombinedDir();

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

