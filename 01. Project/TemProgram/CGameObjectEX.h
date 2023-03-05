#pragma once
#include <Engine\CGameObject.h>

class CGameObjectEX :
    public CGameObject
{
public:
    void finaltick();

public:
    CGameObjectEX();
    ~CGameObjectEX();
};