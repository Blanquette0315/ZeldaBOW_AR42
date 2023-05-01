#pragma once
#include "CEntity.h"

class CAnimator3D;

class CAnimation3D :
    public CEntity
{
private:
    CAnimator3D*                m_pOwner;

    tMTAnimClip                 m_sClip;
    float				        m_fClipUpdateTime;
    int							m_iFrameCount; // 30
    double						m_dCurTime;

    int                         m_iFrmlimit;
    int							m_iFrameIdx; // 클립의 현재 프레임
    int							m_iNextFrameIdx; // 클립의 다음 프레임
    float						m_fRatio;	// 프레임 사이 비율
    bool                        m_bFinish;

public:
    void Create(const wstring& _Key, int _iStartFrame, int _iEndFrame, int _iFrameCount);
    void Create(const wstring& _Key, const tMTAnimClip* _pClip, int _iFrameCount = 30);

    int GetFrameCount() { return m_iFrameCount; }
    void SetFrameCount(int _iFrameCount) { m_iFrameCount = _iFrameCount; }
    int GetStartFrame() { return m_sClip.iStartFrame; }
    void SetStartFrame(int _iStartFrame) { m_sClip.iStartFrame = _iStartFrame; ResetClip(); }
    int GetEndFrame() { return m_sClip.iEndFrame; }
    void SetEndFrame(int _iEndFrame) { m_sClip.iEndFrame = _iEndFrame; ResetClip(); }
    const wstring& GetAnimName() { return m_sClip.strAnimName; }
    
    bool IsFinished() { return m_bFinish; }

    int GetCurFrame() { return m_iFrameIdx; }
    int GetNextFrame() { return m_iNextFrameIdx; }
    float GetRatio() { return m_fRatio; }

private:
    void ResetClip();

public:
    void finaltick();
    void Reset() { m_fClipUpdateTime = 0.f; m_bFinish = false; }

    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;

    CLONE(CAnimation3D);

public:
    CAnimation3D();
    CAnimation3D(const CAnimation3D& _origin);
    ~CAnimation3D();

    friend class CAnimator3D;
};

