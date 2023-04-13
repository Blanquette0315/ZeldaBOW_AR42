#ifndef _STD3D_DEFERRED
#define _STD3D_DEFERRED

#include "register.fx"
#include "struct.fx"
#include "func.fx"

#define fSpecCoefficent g_float_0

struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
    
    float3 vTangent : TANGENT;
    float3 vNormal : NORMAL;
    float3 vBinormal : BINORMAL;
    
    float4 vWeights : BLENDWEIGHT;
    float4 vIndices : BLENDINDICES;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    
    float3 vViewPos : POSITION;
    
    float3 vViewTangent : TANGENT;
    float3 vViewNormal : NORMAL;
    float3 vViewBinormal : BINORMAL;
};

VS_OUT VS_Std3D_Deferred(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    if (g_iAnim)
    {
        Skinning(_in.vPos, _in.vTangent, _in.vBinormal, _in.vNormal, _in.vWeights, _in.vIndices, 0);
    }
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    
    output.vViewPos = mul(float4(_in.vPos, 1.f), g_matWV).xyz;
    output.vViewTangent = normalize(mul(float4(_in.vTangent, 0.f), g_matWV).xyz);
    output.vViewNormal = normalize(mul(float4(_in.vNormal, 0.f), g_matWV).xyz);
    output.vViewBinormal = normalize(mul(float4(_in.vBinormal, 0.f), g_matWV).xyz);

    return output;
}

// SV_Target은 뒤에 숫자를 붙여서 몇번째 렌더 타겟에 데이터를 저장할지를 의미한다.
// 우리가 기존 PS에서 사용하던 SV_Target의 의미는 SV_Target0와 같다.
struct PS_OUT
{
    float4 vColor       : SV_Target0;
    float4 vNormal      : SV_Target1;
    float4 vPosition    : SV_Target2;
    float4 vData        : SV_Target3;
    float4 vEmissiv     : SV_Target4;
};

PS_OUT PS_Std3D_Deferred(VS_OUT _in) : SV_Target
{
    PS_OUT output = (PS_OUT) 0.f;
    
    if (g_btex_4)
    {
        float4 vMasking = float4(0.f, 0.f, 0.f, 1.f);
        vMasking = g_tex_4.Sample(g_sam_0, _in.vUV);
        if (vMasking.a == 0.f)
        {
            discard;
        }
    }
    
    float4 vObjColor = float4(1.f, 0.f, 1.f, 1.f);
    
    if (g_btex_0)
    {
        vObjColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    }

    float3 vNormal = _in.vViewNormal;
    
    if (g_btex_1)
    {
        vNormal = g_tex_1.Sample(g_sam_0, _in.vUV).xyz;
        vNormal = (vNormal * 2.f) - 1.f;
        
        float3x3 matTBN =
        {
            _in.vViewTangent,
            _in.vViewBinormal,
            _in.vViewNormal,
        };
        
        vNormal = normalize(mul(vNormal, matTBN));
    }
    
    // if Binding EmissiveTex
    float4 vEmissiveColor = float4(0.f, 0.f, 0.f, 1.f);
    if(g_btex_3)
    {
        vEmissiveColor = g_tex_3.Sample(g_sam_0, _in.vUV);
    }
    
    output.vColor = vObjColor * g_vDiff;
    output.vNormal = float4(vNormal, 1.f);
    output.vPosition = float4(_in.vViewPos, 1.f);
    output.vEmissiv = vEmissiveColor;
    float4 vSpecCoeff = float4(fSpecCoefficent, fSpecCoefficent, fSpecCoefficent, 1.f);
       
    // Spec 맵이 있으면
    if (g_btex_2)
    {
        vSpecCoeff *= g_tex_2.Sample(g_sam_0, _in.vUV);
    }
    else
    {
        vSpecCoeff *= g_vSpec;
    }
    
    output.vData.x = encode(vSpecCoeff);
    
    return output;
}


// =====================================
// AlphaBlend Defferd Shader
// =====================================

#define Is_MaskTex g_btex_4

struct PSALPHA_OUT
{
    float4 vColor : SV_Target0;
    float4 vNormal : SV_Target1;
    float4 vPosition : SV_Target2;
    float4 vData : SV_Target3;
    float4 vEmissiv : SV_Target4;
};

PSALPHA_OUT PS_Std3DAlpha_Deferred(VS_OUT _in) : SV_Target
{
    PSALPHA_OUT output = (PSALPHA_OUT) 0.f;
    
    float4 vObjColor = float4(1.f, 0.f, 1.f, 1.f);
    
    if (g_btex_0)
    {
        vObjColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    }

    float3 vNormal = _in.vViewNormal;
    
    if (g_btex_1)
    {
        vNormal = g_tex_1.Sample(g_sam_0, _in.vUV).xyz;
        vNormal = (vNormal * 2.f) - 1.f;
        
        float3x3 matTBN =
        {
            _in.vViewTangent,
            _in.vViewBinormal,
            _in.vViewNormal,
        };
        
        vNormal = normalize(mul(vNormal, matTBN));
    }
    
    // if Binding EmissiveTex
    float4 vEmissiveColor = float4(0.f, 0.f, 0.f, 1.f);
    if (g_btex_3)
    {
        vEmissiveColor = g_tex_3.Sample(g_sam_0, _in.vUV);
    }
    
    // Sampling Masking Texture
    if (Is_MaskTex)
    {
        float4 vMasking = float4(0.f, 0.f, 0.f, 1.f);
        vMasking = g_tex_4.Sample(g_sam_0, _in.vUV);
        
        if (vMasking.a == 0.f)
        {
            discard;
        }
        
        vObjColor.a = vMasking.a;
    }
    
    output.vColor = vObjColor * g_vDiff;
    output.vNormal = float4(vNormal, 1.f);
    output.vPosition = float4(_in.vViewPos, 1.f);
    output.vEmissiv = vEmissiveColor;
    float4 vSpecCoeff = float4(fSpecCoefficent, fSpecCoefficent, fSpecCoefficent, 1.f);
       
    // Spec 맵이 있으면
    if (g_btex_2)
    {
        vSpecCoeff *= g_tex_2.Sample(g_sam_0, _in.vUV);
    }
    else
    {
        vSpecCoeff *= g_vSpec;
    }
    
    output.vData.x = encode(vSpecCoeff);
    
    return output;
}

#endif