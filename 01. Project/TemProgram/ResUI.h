#pragma once
#include "UI.h"

#include <Engine/ptr.h>
#include <Engine/CRes.h>

class ResUI :
    public UI
{
private:
    Ptr<CRes>   m_TargetRes;    // �ش� UI�� � Res�� �����ϰ��ִ��� �ּҸ� ����
    RES_TYPE    m_eResType;     // �ش� UI�� � Ÿ���� Res�� �����ϴ��� Ÿ���� ����

public:
    void SetTarget(Ptr<CRes> _pTarget) { m_TargetRes = _pTarget; }
    Ptr<CRes> GetTarget() { return m_TargetRes; }
    RES_TYPE GetComponentType() { return m_eResType; }

    virtual void render_update() override;

public:
    ResUI(const string& _UIName, RES_TYPE _eType);
    ~ResUI();
};

