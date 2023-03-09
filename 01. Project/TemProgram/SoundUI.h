#pragma once
#include "ResUI.h"

class SoundUI :
    public ResUI
{
private:
    CSound* m_pTargetSound;
    string  m_strName;
    string  m_strPath;

    int     m_iChenal;
    float   m_fVolume;
    bool    m_bPlaySound;

    bool    m_bUpdateOnce;

private:
    void PrevSoundStop();

public:
    virtual void update() override;
    virtual void render_update() override;

    virtual void Close() override;

public:
    SoundUI();
    ~SoundUI();
};