#pragma once
#include "CComponent.h"

#include "CTexture.h"

class CAnimation2D;

class CAnimator2D :
    public CComponent
{
private:
    map<wstring, CAnimation2D*> m_mapAnim;
    CAnimation2D*               m_pCurAnim;

    bool                        m_bRepeat;

public:
    // ���� �ִϸ��̼��� ���° �ִϸ��̼����� ������ ����
    int GetCurAnimIdx();

public:
    // �ִϸ��̼� UI�� ����� ���� �߰��� �Լ�
    const map<wstring, CAnimation2D*>& GetAllAnimation() { return m_mapAnim; }
    CAnimation2D* GetCurAnimation() { return m_pCurAnim; }

    // �ִϸ��̼��� ã�Ƽ� �������ִ� ���
    void EraseAnimation(const wstring& _strKey);

    // �ִϸ��̼��� �̸��� Ű���� �ٲ��ִ� �Լ�
    void ChangeAnimName(const wstring& _strOrignKey, const wstring& _strNewKey);

    // ���ϸ��̼� ��� �Լ� : ���ڷ� ������ Ű���� Animation2D*�� �޴´�.
    void RegisterAnimation(const wstring& _strKey, CAnimation2D* _TargetAnim);

    // �ִϸ��̼� ���� ����� ���
    void OverWriteAnimation(const wstring& _strKey, Ptr<CTexture> _AtlasTex, const vector<tAnim2DFrm>& _vecFrm);

    void CreateAnimation(const wstring& _strKey, Ptr<CTexture> _AtlasTex, const vector<tAnim2DFrm>& _vecFrm);

    // �� �ִϸ��̼� �߰� ��� : �ӽ� Animation�� ������ְ�, �ش� Animation�� ���� Animation���� ������ �ش�.
    void CreatebeginAnimation();

public:
    virtual void begin() override;
    virtual void finaltick() override;

public:
    // ���� : �ִϸ��̼� Ű��, ��Ʋ�� �ؽ�ó �̹���, �»�� ��ǥ, �� ĭ�� ����, �� �̹��� �� ����, �� ������, ��� �ʴ� �ð�
    void CreateAnimation(const wstring& _strKey, Ptr<CTexture> _AtlasTex, Vec2 _vLeftTop, Vec2 _vSlice, float _fStep, int _iMaxFrm, float _FPS);
    // �ִϸ��̼��� ã�ƿ��� �Լ�
    CAnimation2D* FindAnimation(const wstring& _strKey);

    void Play(const wstring& _strKey, bool _bRepeat);

    void Stop() { m_pCurAnim = nullptr; } // �ӽ�

    void UpdateData();
    void Clear();

public:
    virtual void SaveToFile(FILE* _File);
    virtual void LoadFromFile(FILE* _File);

    CLONE(CAnimator2D);

public:
    CAnimator2D();
    CAnimator2D(const CAnimator2D& _origin);
    ~CAnimator2D();
};

