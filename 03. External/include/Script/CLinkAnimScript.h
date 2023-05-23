#pragma once
#include <Engine/CScript.h>



struct tLinkStatus
{
    int iHP;
};

struct tLinkDamaged
{
    int iDamage;
    LINK_DAMAGED_TYPE eType;
};

enum class EQUIPMENT_STATE
{
    SWORD,
    BOW,
    SHIELD,
    END
};

class CPrefab;
class CAnimator3D;
class CAnimation3D;
class tAnimNode;
class CGroundCheckScript;
class CLockOnScript;
class CUIHeartScript;
class CUIScript;

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
    CUIHeartScript*         m_pHPUI;
    CUIScript*              m_pCrosshairUI;

    CGameObject*            m_pSwordObj;
    CGameObject*            m_pBowObj;
    CGameObject*            m_pShieldObj;
    

    Ptr<CPrefab>            m_pBombPref;
    CGameObject*            m_pBombObj;

    CGameObject*            m_pInJustRigidObj;

    Vec3                    m_vDir[(UINT)LINK_DIRECTION::END];

    UINT                    m_iMode;

    // use for function that should operate once
    bool                    m_bOnceAtAnimStart;

    bool                    m_bComboProgress;
    float                   m_fComboAccTime;
    float                   m_fComboMaxTime;

    bool                    m_bLockOn;
    bool                    m_bLockOnRotFinish;
    bool                    m_bIsAnimChanged;
    bool                    m_bShieldGuard;
    bool                    m_bInJustRigid;
    bool                    m_bEvasionJust;
    bool                    m_bInvincible;

    float                   m_fParryingAccTime;
    bool                    m_bParryingOnce;

    float                   m_fEvasionAccTime;
    bool                    m_bEvasionOnce;

    float                   m_fJustAtkAccTime;

    bool                    m_bArrEquip[(UINT)EQUIPMENT_STATE::END];

    bool                    m_bCanJustAttackStart;
    bool                    m_bCanJustAttack;

    bool                    m_bJustAtkEndOnce;
    bool                    m_bOFA;

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

    float           m_fParryingMaxTime;
    float           m_fEvasionMaxTime;
    float           m_fJustAtkMaxTime;
    
    float           m_fJustMoveForce;

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
    void Timer();

    // This Func can use anim finished member 
    void OperateAnimFuncAfter();
    void PlayNextAnim();
    void OperateAnimFunc();
    void ControlUI();

    void ClearData();

    // anim function
private:
    bool MoveRotation(Vec3 _vDir);
    void SelectSpeed();
    void MoveToDir(DIR _eDir, bool _bReverse = 0);
    void MoveToDirAdd(DIR _eDir, bool _bReverse = 0);

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
    void Func_BowChargeMove();
    void Func_ShieldGuard();
    void Func_ShieldJustStart();
    void Func_ShieldJust();
    void Func_ShieldJustEnd();
    void Func_SwordLockOnWait();
    void Func_JustEvasionStart();
    void Func_JustEvasion();
    void Func_JustEvasionEnd();
    void Func_JustAtkStart();
    void Func_JustAtkDash();
    void Func_DisableCanJust();
    void Func_JustAtkEnd();
    void Func_CreateBomb();
    void Func_ThrowBombStart();

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

    void MoveByForce(Vec3 _vDirXZ, float _fForce);

    // getter setter
public:
    tAnimNode* GetCurAnimNode() { return m_pCurAnimNode; }
    bool IsAnimChanged() { return m_bIsAnimChanged; }

    UINT GetLinkMode() { return m_iMode; }
    UINT& GetLinkModeRef() { return m_iMode; }
    CLockOnScript* GetLockOnRadar() { return m_pLockOnRadar; }
    CGameObject* GetLinkCam() { return m_pLinkCamObj; }
    CGameObject* GetLinkBow() { return m_pBowObj; }
    const tLinkStatus& GetLinkStatus() { return m_tLinkStatus; }

    // Interaction with monster function
public:
    void SetDamage(tLinkDamaged _tDamage) { m_tLinkDamaged = _tDamage; }
    void ApplyDamage();

    bool IsGuardAnim() { return IsCurAnim(LAT_SWORD_GUARD_WAIT); }
    bool IsBowAtkAnim() { return IsCurAnim(LAT_BOW_ATTACK_CHARGE); }
    void SetGuardSuccess(bool _bGuardSuccess) { m_bShieldGuard = _bGuardSuccess; }
    
    // bool IsGuardJustAnim() { return IsCurAnim(LAT_SWORD_GUARD_JUST); }
    void SetGuardJustSuccess(bool _bJustSuccess, CGameObject* _pParryingObj) { m_bInJustRigid = _bJustSuccess; m_pInJustRigidObj = _pParryingObj; }

    // Timer Function
public:
    void JustAttackTimer();

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