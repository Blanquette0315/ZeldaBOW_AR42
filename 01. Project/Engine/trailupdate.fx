#ifndef _TRAILUPDATE
#define _TRAILUPDATE

#include "struct.fx"
#include "register.fx"

#define DOT_MAX g_int_0

RWStructuredBuffer<tTwoDot> VecWorldPos : register(u0);
RWStructuredBuffer<uint> Output : register(u1);

[numthreads(128, 1, 1)]
void CS_TrailUpdate(uint3 _id : SV_DispatchThreadID)
{
    if ((uint) DOT_MAX <= _id.x)
        return;
    
    VecWorldPos[_id.x].vFar.w = 1.f;
    VecWorldPos[_id.x].vNear.w = 1.f;
    VecWorldPos[_id.x].vFar = mul(mul(float4(VecWorldPos[_id.x].vFar), g_matView), g_matProj);
    VecWorldPos[_id.x].vNear = mul(mul(float4(VecWorldPos[_id.x].vNear), g_matView), g_matProj);
    
    
}

#endif