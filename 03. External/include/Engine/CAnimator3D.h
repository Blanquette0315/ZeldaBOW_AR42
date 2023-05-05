#pragma once
#include "CComponent.h"

#include "Ptr.h"
#include "CTexture.h"
#include "CMaterial.h"
#include "CMesh.h"

class CStructuredBuffer;
class CAnimation3D;

class CAnimator3D :
    public CComponent
{
private:
    const vector<tMTBone>*      m_pVecBones;
    CStructuredBuffer*          m_pBoneFinalMatBuffer;  // 특정 프레임의 최종 행렬
    bool						m_bFinalMatUpdate;      // 최종행렬 연산 수행여부

    int                         m_iFrmlimit;

    map<wstring, CAnimation3D*> m_mapAnim;

    CAnimation3D*               m_pCurAnim;
    CAnimation3D*               m_pCurAnimLower;

    bool                        m_bRepeat;
    bool                        m_bRepeatLower;

    // if anim divided to upper - lower, upper anim's end frame.
    int                         m_iBoneDivPoint;

    // for animation blending ( added to member because of bonesocket which use this too)
    Matrix                      m_matUpperInv;
    Matrix                      m_matUpperNextInv;
    Matrix                      m_matLower;
    Matrix                      m_matLowerNext;

public:
    virtual void finaltick() override;
    void UpdateData();

public:
    void SetBones(const vector<tMTBone>* _vecBones) { m_pVecBones = _vecBones; }
    void SetAnimClip(const vector<tMTAnimClip>* _vecAnimClip);
    void SetFinalMatUpdate(bool _bool) { m_bFinalMatUpdate = _bool; }
    void SetFrameLimit(UINT _iFrameLimit) { m_iFrmlimit = _iFrameLimit; }
    void SetBoneDivPoint(int _iDivPoint) { m_iBoneDivPoint = _iDivPoint; }

    CStructuredBuffer* GetFinalBoneMat() { return m_pBoneFinalMatBuffer; }
    UINT GetBoneCount() { return (UINT)m_pVecBones->size(); }
    const tMTBone& GetBoneByName(const wstring& _strBoneName);
    const tMTBone& GetBoneByIdx(UINT _iIdx) { return m_pVecBones->at(_iIdx); }

    const Matrix& GetMatUpperInv() { return m_matUpperInv; }
    const Matrix& GetMatUpperNextInv() { return m_matUpperNextInv; }
    const Matrix& GetMatLower() { return m_matLower; }
    const Matrix& GetMatLowerNext() { return m_matLowerNext; }

    void ClearData();

    void CreateAnimation(const wstring& _strKey, int _iStartFrame, int _iEndFrame, int _iFrameCount);
    void CreateAnimation(const wstring& _strKey, const tMTAnimClip* _pClip, int _iFrameCount = 30);
    
    CAnimation3D* FindAnimation(const wstring& _strKey);

    void Play(const wstring& _strKey, bool _bRepeat);
    void Stop() { m_pCurAnim = nullptr; }

    // LowerAnim
    void PlayLowerAnim(const wstring& _strKey, bool _bRepeat);
    void StopLowerAnim() { m_pCurAnimLower = nullptr; }

public:
    const map<wstring, CAnimation3D*>& GetMapAnimation() { return m_mapAnim; }
    CAnimation3D* GetCurAnimation() { return m_pCurAnim; }
    CAnimation3D* GetCurAnimationLower() { return m_pCurAnimLower; }
    void ChangeAnimName(const wstring& _strOrignKey, const wstring& _strNewKey);
    void EraseAnimation(const wstring& _strKey);
    void CreateDefaultAnimation();
    void CreateAnimation(CAnimation3D* _animation);

private:
    void check_mesh(Ptr<CMesh> _pMesh);
    Matrix MakeMatrixFromKeyFrame(int _iBoneIdx ,int _iKeyFrameIdx);

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;
 
    CLONE(CAnimator3D);

public:
    CAnimator3D();
    CAnimator3D(const CAnimator3D& _origin);
    ~CAnimator3D();
};

