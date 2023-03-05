#pragma once
#include "UI.h"

#include <Engine/ptr.h>
#include <Engine/CRes.h>

class ResUI :
    public UI
{
private:
    Ptr<CRes>   m_TargetRes;    // 해당 UI가 어떤 Res를 참조하고있는지 주소를 지님
    RES_TYPE    m_eResType;     // 해당 UI가 어떤 타입의 Res를 참조하는지 타입을 지님

public:
    void SetTarget(Ptr<CRes> _pTarget) { m_TargetRes = _pTarget; }
    Ptr<CRes> GetTarget() { return m_TargetRes; }
    RES_TYPE GetComponentType() { return m_eResType; }

    virtual void render_update() override;

public:
    ResUI(const string& _UIName, RES_TYPE _eType);
    ~ResUI();
};

