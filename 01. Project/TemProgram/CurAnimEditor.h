#pragma once
#include "UI.h"

#include <Engine/ptr.h>
#include <Engine/CTexture.h>
#include <Engine/CAnimator2D.h>

class Animator2DUI;
class TextureUI;

class CurAnimEditor :
    public UI
{
private:
    CGameObject*                m_pTarget;
    Animator2DUI*               m_pOwnerUI;
    CAnimation2D*               m_pCurAnim;
    string                      m_CurAnimName;
    vector<tAnim2DFrm>          m_vecFrm;
    Ptr<CTexture>               m_AtlasTex;
    ImVec2                      m_vSceneSize;
    Vec2                        m_vAtlasWH;
    Vec2                        m_vFullSize;
    TextureUI*                  m_pTextureUI;

    float                       m_fGridPixelSize;
    int                         m_AnimPreviewIdx;   // 만든 애니메이션 미리 재생을 위한 인덱스
    float                       m_fAcctime;         // 프리뷰 애니메이션 재생시 시간 측정
    bool                        m_bPlayPreview;     // 프리뷰 실행 버튼을 눌렀는지 체크
    bool                        m_bGrid;            // 프리뷰에 그리드 생성
    bool                        m_bGrid_Front;
    bool                        m_bGrid_Center;     // Center 표시
    bool                        m_bPlayOncePreview; // 애니메이션 한번만 실행

    // 지금 선택한 프레임 정보
    int                         m_iSelectedIdx;

private:
    void SetAnimation(CAnimation2D* _Anim);
    void SetCurFrm(const vector<tAnim2DFrm>& _Frm) { m_vecFrm = _Frm; }
    void SetAtlasTex(Ptr<CTexture> _Atlas) { m_AtlasTex = _Atlas; } // 아직 사용중인 곳없음.
    void SetTarget(CGameObject* _pObj) { m_pTarget = _pObj; }
    void SetOwnerUI(Animator2DUI* _UI) { m_pOwnerUI = _UI; }

    // 실행 함수
    void PlayWorkingAnim();
    void ResetWorkingAnim();

public:
    void SetAtlasTex(DWORD_PTR _AtlasKey);
    void SetSliceImage(DWORD_PTR _LeftTop, DWORD_PTR _Slice);

public:
    virtual void update() override;
    virtual void render_update() override;

    void clear() { m_iSelectedIdx = 0; m_AnimPreviewIdx = 0; }

public:
    CurAnimEditor();
    ~CurAnimEditor();

    friend Animator2DUI;
};

