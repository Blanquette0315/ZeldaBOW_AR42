#ifndef _EFFECT
#define _EFFECT

#include "register.fx"

float4 PaperBurn(in float4 _vDiffuse, float2 _vUV)
{
    float fAccTime = g_float_1;
    float fMaxTime = g_float_2;
    
    float fCurTime = fAccTime / fMaxTime;
        
    float4 vNoise = g_Noise.SampleLevel(g_sam_0, _vUV, 0);
        
    float fCurAlpha = vNoise.r * -1.f + (fCurTime * 2.f);
    
    fCurAlpha = saturate(1.f - fCurAlpha);
        
    float fCurRampUV = saturate(1.f - fCurAlpha) * 1.5f;
        
    float4 vRamp = g_texarr_0.SampleLevel(g_sam_0, float3(fCurRampUV, 0.5f, g_float_3), 0);
    
    if (fCurRampUV > 0.f)
    {
        _vDiffuse.rgb = vRamp.rgb;
    }
    
    clip(1.f - fCurRampUV);
    
    if (_vDiffuse.a <= 0.f)
    {
        clip(-1.f);
    }
        
    return _vDiffuse;
}

#endif