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

    bool                        m_bChanged; // Ȯ���� ���� �ӽ÷� ������ ��. ���߿� �ش� ���ϸ��̼��� �ٲ�ų� �Ҷ� �ش� ������Ʈ���� ������Ʈ �� ���� �˾ƿ����� �ؾ���.

    // �ִϸ��̼� �������� ���� ���
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

