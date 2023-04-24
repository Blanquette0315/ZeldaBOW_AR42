#pragma once
#include "ComponentUI.h"

class CAnimation3D;

class Animator3DUI :
    public ComponentUI
{
private:
    CAnimation3D*       m_Animation;
    CAnimation3D*       m_AnimationLower;

    int                 m_iStartFrame;
    int                 m_iEndFrame;
    int                 m_iFrameCount;
    bool                m_bRepeat;

    int                 m_iStartFrameLower;
    int                 m_iEndFrameLower;
    int                 m_iFrameCountLower;
    bool                m_bRepeatLower;

public:
    virtual void update() override;
    virtual void render_update() override;
    virtual void Close() override { ComponentUI::Close(); m_Animation = nullptr; m_AnimationLower = nullptr; }

private:
    void SetAnimaion(DWORD_PTR _strAnimKey);
    void SaveAnimation();
    void LoadAnimation(const wstring& _FileName);

private:
    void SetAnimaionLower(DWORD_PTR _strAnimKey);
    void SaveAnimationLower();

public:
    Animator3DUI();
    ~Animator3DUI();
};

