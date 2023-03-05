#pragma once
#include <Engine/CCamera.h>

class CEditorCam :
    public CCamera
{
public:
    // 원래 카메라 컴포넌트 쪽에서 finaltick에 카메라를 등록하는게 있었는데, 우리는 에디터용 카메라에만 등록하면 되기 때문에
    // 오버라이딩 해주었다.
    virtual void finaltick() override;
};

