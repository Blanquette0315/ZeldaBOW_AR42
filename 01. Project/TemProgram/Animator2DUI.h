#pragma once
#include "ComponentUI.h"

#include <Engine/CAnimation2D.h>

class Animator2DUI :
    public ComponentUI
{
private:
    map<wstring, CAnimation2D*> m_mapAnim;
    CAnimation2D*               m_pCurAnim;
    int                         m_iCurComboIdx;

    bool                        m_bChanged; // 확인을 위해 임시로 만들어둔 것. 나중에 해당 에니매이션이 바뀌거나 할때 해당 컴포넌트에서 업데이트 된 것을 알아오도록 해야함.

    // 애니메이션 프리뷰을 위한 멤버
    vector<tAnim2DFrm>          m_vecFrm;
    float                       m_fAcctime;
    int                         m_iCurIdx;

private:
    void PlayPreview();

public:
    void ChangeInfo(bool _Change) { m_bChanged = _Change; }
    void SetCurAnim(CAnimation2D* _Anim) { m_pCurAnim = _Anim; }
    void Open() override;

public:
    virtual void update() override;
    virtual void render_update() override;

public:
    Animator2DUI();
    ~Animator2DUI();
};

