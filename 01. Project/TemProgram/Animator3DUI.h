#pragma once
#include "ComponentUI.h"

class CAnimation3D;

class Animator3DUI :
    public ComponentUI
{
private:
    CAnimation3D*       m_Animation;

    int                 m_iStartFrame;
    int                 m_iEndFrame;
    int                 m_iFrameCount;
    bool                m_bRepeat;
public:
    virtual void update() override;
    virtual void render_update() override;
    virtual void Close() override { ComponentUI::Close(); m_Animation = nullptr; }

private:
    void SetAnimaion(DWORD_PTR _strAnimKey);
    void SaveAnimation();
    void LoadAnimation(const wstring& _FileName);

public:
    Animator3DUI();
    ~Animator3DUI();
};

