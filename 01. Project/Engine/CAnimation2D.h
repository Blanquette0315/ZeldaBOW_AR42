#pragma once
#include "CEntity.h"

#include "CTexture.h"
class CAnimator2D;

class CAnimation2D :
    public CEntity
{
private:
    vector<tAnim2DFrm>  m_vecFrm;
    int                 m_iCurIdx;

    CAnimator2D*        m_pOwner;
    Ptr<CTexture>       m_AtlasTex;

    float               m_fAcctime;
    bool                m_bFinish;

public:
    // Animator2D UI�� �����ϸ鼭 ����� �ӽ� �Լ���
    Ptr<CTexture> GetAtlas() { return m_AtlasTex; }
    const vector<tAnim2DFrm>& GetvecFrm() { return m_vecFrm; }

    void Create(const wstring& _strKey, Ptr<CTexture> _AtlasTex, const vector<tAnim2DFrm>& _vecFrm);

    bool Is_End() { return m_bFinish; }

    // ���� �ε��� ��������
    int GetCurIdx() { return m_iCurIdx; }
public:
    void finaltick();

public:
    void Create(const wstring& _Key, Ptr<CTexture> _AtlasTex, Vec2 _vLeftTop, Vec2 _vSlice, float _fStep, int _iMaxFrm, float _FPS);
    void Reset()
    {
        m_fAcctime = 0;
        m_iCurIdx = 0;
        m_bFinish = false;
    }

    void UpdateData();
    void Clear();

    virtual void SaveToYAML(YAML::Emitter& _emitter) override {};
    virtual void LoadFromYAML(YAML::Node& _node) override {};
    CLONE(CAnimation2D);

public:
    CAnimation2D();
    ~CAnimation2D();

    friend class CAnimator2D;
};

