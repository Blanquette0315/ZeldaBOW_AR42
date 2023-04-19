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
    bool						m_bFinalMatUpdate; // 최종행렬 연산 수행여부

    int                         m_iFrmlimit;

    map<wstring, CAnimation3D*> m_mapAnim;
    CAnimation3D*               m_pCurAnim;

    bool                        m_bRepeat;

public:
    virtual void finaltick() override;
    void UpdateData();

public:
    void SetBones(const vector<tMTBone>* _vecBones) { m_pVecBones = _vecBones; }
    void SetAnimClip(const vector<tMTAnimClip>* _vecAnimClip);
    void SetFinalMatUpdate(bool _bool) { m_bFinalMatUpdate = _bool; }

    CStructuredBuffer* GetFinalBoneMat() { return m_pBoneFinalMatBuffer; }
    UINT GetBoneCount() { return (UINT)m_pVecBones->size(); }
    const tMTBone& GetBoneByName(const wstring& _strBoneName);

    void ClearData();

    void CreateAnimation(const wstring& _strKey, int _iStartFrame, int _iEndFrame, int _iFrameCount);
    void CreateAnimation(const wstring& _strKey, const tMTAnimClip* _pClip, int _iFrameCount = 30);
    
    CAnimation3D* FindAnimation(const wstring& _strKey);

    void Play(const wstring& _strKey, bool _bRepeat);
    void Stop() { m_pCurAnim = nullptr; }

public:
    const map<wstring, CAnimation3D*>& GetMapAnimation() { return m_mapAnim; }
    CAnimation3D* GetCurAnimation() { return m_pCurAnim; }
    void ChangeAnimName(const wstring& _strOrignKey, const wstring& _strNewKey);
    void EraseAnimation(const wstring& _strKey);
    void CreateDefaultAnimation();
    void CreateAnimation(CAnimation3D* _animation);

private:
    void check_mesh(Ptr<CMesh> _pMesh);

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;
 
    CLONE(CAnimator3D);

public:
    CAnimator3D();
    CAnimator3D(const CAnimator3D& _origin);
    ~CAnimator3D();
};

