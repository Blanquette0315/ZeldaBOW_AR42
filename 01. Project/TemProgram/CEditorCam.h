#pragma once
#include <Engine/CCamera.h>

class CEditorCam :
    public CCamera
{
public:
    // ���� ī�޶� ������Ʈ �ʿ��� finaltick�� ī�޶� ����ϴ°� �־��µ�, �츮�� �����Ϳ� ī�޶󿡸� ����ϸ� �Ǳ� ������
    // �������̵� ���־���.
    virtual void finaltick() override;
};

