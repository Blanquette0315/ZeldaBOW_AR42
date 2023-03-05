#pragma once
#include "CRenderComponent.h"

class CLandScape :
    public CRenderComponent
{
private:
    UINT m_iXFaceCount;
    UINT m_iZFaceCount;

public:
    virtual void finaltick() override;
    virtual void render() override;

public:
    void SetFaceCount(UINT _X, UINT _Z);

private:
    void CreateMesh();

public:

    CLONE(CLandScape);
public:
    CLandScape();
    ~CLandScape();
};