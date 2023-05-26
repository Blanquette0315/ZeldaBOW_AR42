#ifndef _CAMEFFECT
#define _CAMEFFECT

#include "register.fx"
#include "func.fx"

struct VS_IN
{
    float3 vPos : POSITION;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
};

// ===========
// FADE OUT
// ===========

VS_OUT VS_FADEOUT(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    output.vPosition = float4(_in.vPos.xy * 2.f, 0.f, 1.f);
    
    return output;
}

float4 PS_FADEOUT(VS_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;
    
    vOutColor.a = g_float_0;
    
    return vOutColor;
}

// ===========
// FADE IN
// ===========

VS_OUT VS_FADEIN(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    output.vPosition = float4(_in.vPos.xy * 2.f, 0.f, 1.f);
    
    return output;
}

float4 PS_FADEIN(VS_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;
    
    vOutColor.a = 1.f - g_float_0;
    
    return vOutColor;
}


#endif