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
    int                         m_AnimPreviewIdx;   // ���� �ִϸ��̼� �̸� ����� ���� �ε���
    float                       m_fAcctime;         // ������ �ִϸ��̼� ����� �ð� ����
    bool                        m_bPlayPreview;     // ������ ���� ��ư�� �������� üũ
    bool                        m_bGrid;            // �����信 �׸��� ����
    bool                        m_bGrid_Front;
    bool                        m_bGrid_Center;     // Center ǥ��
    bool                        m_bPlayOncePreview; // �ִϸ��̼� �ѹ��� ����

    // ���� ������ ������ ����
    int                         m_iSelectedIdx;

private:
    void SetAnimation(CAnimation2D* _Anim);
    void SetCurFrm(const vector<tAnim2DFrm>& _Frm) { m_vecFrm = _Frm; }
    void SetAtlasTex(Ptr<CTexture> _Atlas) { m_AtlasTex = _Atlas; } // ���� ������� ������.
    void SetTarget(CGameObject* _pObj) { m_pTarget = _pObj; }
    void SetOwnerUI(Animator2DUI* _UI) { m_pOwnerUI = _UI; }

    // ���� �Լ�
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

