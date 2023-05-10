#pragma once
#include <Engine/CScript.h>

struct tLinkStatus
{
    float fHP;
};

struct tLinkDamaged
{
    float fDamage;
    LINK_DAMAGED_TYPE eType;
};


class CAnimator3D;
class CAnimation3D;
class tAnimNode;
class CGroundCheckScript;
class CLockOnScript;

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
    tAnimNode*      m_pCurAnimNodeLower;
    tAnimNode*      m_pPrevAnimNode;
    tAnimNode*      m_pAnyStateNode;

    // tAnimNode*      m_pNextAnimNode;
    UINT            m_iCond;

    CGameObject*            m_pLinkCamObj;
    CGroundCheckScript*     m_pGroundChecker;
    CLockOnScript*          m_pLockOnRadar;

    CGameObject*            m_pSwordObj;
    CGameObject*            m_pBowObj;
    CGameObject*            m_pShieldObj;


    Vec3                    m_vDir[(UINT)LINK_DIRECTION::END];

    UINT                    m_iMode;

    // use for function that should operate once
    bool                    m_bOnceAtAnimStart;

    bool                    m_bComboProgress;
    float                   m_fComboAccTime;
    float                   m_fComboMaxTime;

    bool                    m_bLockOn;
    bool                    m_bIsAnimChanged;

    // save
private:
    float           m_fAnglePerSec;

    float           m_fWalkSpeed;
    float           m_fRunSpeed;
    float           m_fDashSpeed;
    float           m_fJumpSpeed;
    float           m_fSelectedSpeed;
    tLinkStatus     m_tLinkStatus;
    tLinkDamaged    m_tLinkDamaged;

    // FSM function
private:
    void CreateAnimNode();
    void ReplaceNodeAnim();
    void MakeFSM();
    void SetAnimNode(tAnimNode*& _pAnimNode, LINK_ANIM_TYPE _eAnim, UINT _ePref = 0);
    void SetAnimTran(tAnimNode* _pAnimNode, LINK_ANIM_TYPE _eAnim, UINT _eIncludeAs1Bit, UINT _eIncludeAs0Bit = (LINK_ANIM_CONDITION)0);

    // tick structure
private:
    // set link condition
    void SetLinkCond();

    // This Func can use anim finished member 
    void OperateAnimFuncAfter();

    void PlayNextAnim();
    void OperateAnimFunc(); 

    // anim function
private:
    void MoveRotation(Vec3 _vDir);
    void SelectSpeed();
    void MoveToDir(DIR _eDir, bool _bReverse = 0);

    void Func_WalkRunDash();
    void Func_LockOnMove();
    void Func_TurnBack();
    void Func_Jump();
    void Func_LowerBodyBlend();
    void Func_SwordRun();
    void Func_BowRun();
    void Func_BowEquipOn();
    void Func_BowEquipOff();
    void Func_SwordAttackMove();
    void Func_SwordEquipOn();
    void Func_SwordEquipOff();

    // convenience function
private:
    // check current anim's member finish
    bool IsCurAnim(LINK_ANIM_TYPE _eLAT); 
    bool IsCurAnimLower(LINK_ANIM_TYPE _eLAT);
    
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

    // get ground from child ground checker
    bool IsGround();

public:
    tAnimNode* GetCurAnimNode() { return m_pCurAnimNode; }
    bool IsAnimChanged() { return m_bIsAnimChanged; }

    // Interaction with monster function
public:
    void SetDamage(tLinkDamaged _tDamage) { m_tLinkDamaged = _tDamage; }
    void ApplyDamage();

public:
    static void ClearAnimNode();

public:
    virtual void init() override;
    virtual void begin() override;
    virtual void tick() override;

    virtual void BeginOverlap(CGameObject* _pOther) override;
    virtual void Overlap(CGameObject* _pOther) override;
    virtual void EndOverlap(CGameObject* _pOther) override;

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CLinkAnimScript)

public:
    CLinkAnimScript();
    CLinkAnimScript(const CLinkAnimScript& _origin);
    ~CLinkAnimScript();
};

typedef void (CLinkAnimScript::* Link_Func)(void);