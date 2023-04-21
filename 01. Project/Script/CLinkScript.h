#pragma once
#include <Engine/CScript.h>



class CAnimation3D;
class CGameObject;

class CLinkScript :
    public CScript
{
private:
    CGameObject*    m_pLinkCamObj;
    Vec3            m_vDir[(UINT)LINK_DIRECTION::END];

private:
    float m_fSpeed;

public:
    const Vec3& GetMoveDir(LINK_DIRECTION _eDir) { return m_vDir[(UINT)_eDir]; }

private:
    LINK_FRONT_TOE GetFrontToe(CAnimation3D* _pCurAnim);
    int FindClosestIdx(const vector<tMTKeyFrame>& _vec, float _fTime, int _iIdx, int _iStart, int _iEnd);

    void CalcMoveDirection();

public:
    virtual void begin() override;
    virtual void tick() override;

    virtual void BeginOverlap(CCollider* _pOther) override;
    virtual void Overlap(CCollider* _pOther) override;
    virtual void EndOverlap(CCollider* _pOther) override;

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CLinkScript)

public:
    CLinkScript();
    ~CLinkScript();
};

