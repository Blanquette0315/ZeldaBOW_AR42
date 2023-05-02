#ifndef _BLOOM
#define _BLOOM

#include "register.fx"
#include "func.fx"

// =========================
// Bloom
// mesh : RectMesh
//
// Parameter
// === Texture ===
// g_tex_0 : DiffuseTargetTex
#define DiffuseTex g_tex_0
// g_tex_1 : DiffuseBrightTargetTex
#define BrightTex g_tex_1
// g_tex_1 :BloomBlurTargetTex0
// g_tex_2 :BloomBlurTargetTex1
// g_tex_3 :BloomBlurTargetTex2
// g_tex_4 :BloomBlurTargetTex3
// g_tex_5 :BloomBlurTargetTex4
// g_tex_6 :BloomBlurTargetTex5
// g_tex_7 :BloomBlurTargetTex6

// g_int_3 : MaxMipLevels
#define MaxMipLevels g_int_3
// ==========================

struct VS_IN
{
    float3 vPos : POSITION;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
};

VS_OUT VS_Bloom(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    output.vPosition = float4(_in.vPos.xy * 2.f, 0.f, 1.f);
    
    return output;
}

float4 PS_Bloom(VS_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;
    
    float2 vUV = _in.vPosition.xy / g_vRenderResolution;
    vOutColor = g_tex_0.Sample(g_sam_0, vUV);
    
    float4 vSampleColor = g_tex_1.Sample(g_sam_0, vUV);
    vOutColor += vSampleColor;
    
    return vOutColor;
}

float4 PS_AlphaBloom(VS_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;
    
    float2 vUV = _in.vPosition.xy / g_vRenderResolution;
    vOutColor = g_tex_0.Sample(g_sam_0, vUV);
    
    return vOutColor;
}


// =========================
// Bright
// mesh : RectMesh
//
// Parameter
// === Texture ===
// g_tex_0 : DiffuseTargetTex
// #define DiffuseTex g_tex_0
// ==========================

float4 PS_Bright(VS_OUT _in) : SV_Target
{
    float4 BrightColor = float4(0.f, 0.f, 0.f, 0.f);
    float2 vUV = _in.vPosition.xy / g_vRenderResolution;
    float4 FragColor = DiffuseTex.Sample(g_sam_0, vUV);
    
    float brightness = dot(FragColor.rgb, float3(0.2125f, 0.7154f, 0.0721f));
    if (brightness > 0.99)
    {
        BrightColor = float4(FragColor.rgb, 1.0f);
    }
    return BrightColor;
}

// =========================
// BloomBlur
// mesh : RectMesh
//
// Parameter
// === Texture ===
// g_tex_0 : DiffuseBrightTargetTex
// ==========================

struct PS_BU_OUT
{
    float4 BBColor0 : SV_Target0;
    float4 BBColor1 : SV_Target1;
    float4 BBColor2 : SV_Target2;
    float4 BBColor3 : SV_Target3;
    float4 BBColor4 : SV_Target4;
    float4 BBColor5 : SV_Target5;
    float4 BBColor6 : SV_Target6;
};

PS_BU_OUT PS_BloomUpScale(VS_OUT _in)
{
    PS_BU_OUT _out;
    float4 vOutColor = float4(0.f, 0.f, 0.f, 0.f);
    
    for (int k = 0; k < 7; ++k)
    {
        float2 vUV = _in.vPosition.xy / g_vRenderResolution;
        
        float2 GaussianPos = _in.vPosition.xy - float2(2.f, 2.f);
        
        for (int j = 0; j < 5; j++)
        {
          
            for (int i = 0; i < 5; i++)
            {
                float2 NewPos = GaussianPos + float2(j, i);
                float2 GaussianUV = NewPos / g_vRenderResolution;
                
                if (GaussianUV.x > 1.f || GaussianUV.x < 0.f || GaussianUV.y > 1.f || GaussianUV.y < 0.f)
                {
                    continue;
                }
                
                float4 vBrightColor = g_tex_0.SampleLevel(g_sam_2, GaussianUV, k);
                vOutColor += vBrightColor * GaussianFilter[j][i];
            }
            
            switch (k)
            {
                case 0:
                    _out.BBColor0 = vOutColor;
                    break;
        
                case 1:
                    _out.BBColor1 = vOutColor;
                    break;
        
                case 2:
                    _out.BBColor2 = vOutColor;
                    break;
        
                case 3:
                    _out.BBColor3 = vOutColor;
                    break;
        
                case 4:
                    _out.BBColor4 = vOutColor;
                    break;
        
                case 5:
                    _out.BBColor5 = vOutColor;
                    break;
                
                case 6:
                    _out.BBColor6 = vOutColor;
                    break;
            }
        }
    }
    
    return _out;
}

#endif