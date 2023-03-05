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
    // 현재 애니메이션이 몇번째 애니메이션인지 가져와 보기
    int GetCurAnimIdx();

public:
    // 애니메이션 UI를 만들기 위해 추가한 함수
    const map<wstring, CAnimation2D*>& GetAllAnimation() { return m_mapAnim; }
    CAnimation2D* GetCurAnimation() { return m_pCurAnim; }

    // 애니메이션을 찾아서 삭제해주는 기능
    void EraseAnimation(const wstring& _strKey);

    // 애니메이션의 이름과 키값을 바꿔주는 함수
    void ChangeAnimName(const wstring& _strOrignKey, const wstring& _strNewKey);

    // 에니메이션 등록 함수 : 인자로 저장할 키값과 Animation2D*를 받는다.
    void RegisterAnimation(const wstring& _strKey, CAnimation2D* _TargetAnim);

    // 애니메이션 덮어 씌우기 기능
    void OverWriteAnimation(const wstring& _strKey, Ptr<CTexture> _AtlasTex, const vector<tAnim2DFrm>& _vecFrm);

    void CreateAnimation(const wstring& _strKey, Ptr<CTexture> _AtlasTex, const vector<tAnim2DFrm>& _vecFrm);

    // 빈 애니메이션 추가 기능 : 임시 Animation을 만들어주고, 해당 Animation을 현재 Animation으로 지정해 준다.
    void CreatebeginAnimation();

public:
    virtual void begin() override;
    virtual void finaltick() override;

public:
    // 인자 : 애니메이션 키값, 아틀라스 텍스처 이미지, 좌상단 좌표, 한 칸의 간격, 한 이미지 당 간격, 몇 장인지, 출력 초당 시간
    void CreateAnimation(const wstring& _strKey, Ptr<CTexture> _AtlasTex, Vec2 _vLeftTop, Vec2 _vSlice, float _fStep, int _iMaxFrm, float _FPS);
    // 애니메이션을 찾아오는 함수
    CAnimation2D* FindAnimation(const wstring& _strKey);

    void Play(const wstring& _strKey, bool _bRepeat);

    void Stop() { m_pCurAnim = nullptr; } // 임시

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

