#pragma once
#include "ComponentUI.h"
#include "ImGuizmo.h"

class CGameObject;

class TransformUI :
    public ComponentUI
{
private:
    Vec3                m_vPos;
    Vec3                m_vScale;
    Vec3                m_vRot;

    bool                m_bIgnorScale;
    bool                m_bTurnY180;

    ImGuizmo::OPERATION m_eOperation;
    ImGuizmo::MODE      m_eMode;             
    
    bool                m_bUseGizmo;

private:
    virtual void update() override;
    virtual void render_update() override;

private:
    void OperateGizmo3D();

private:
    void IgnoreMinuteDiff(Vec3 _vPrev, Vec3& _vOut, float _fIgnoreDiff = 0.01f);

public:
    TransformUI();
    ~TransformUI();
};

