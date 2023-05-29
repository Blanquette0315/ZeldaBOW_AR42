#ifndef _STD2D_EFFECT
#define _STD2D_EFFECT

#include "register.fx"
#include "func.fx"

#define ALPHA g_vec4_0.a
#define Color g_vec4_0.rgb
#define EmsvCoeff g_vec4_3
#define Option g_int_0

#define OPT_BLUR 1


// HLSL로 VertexShader 작성하기
struct VTX_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VTX_IN_INST
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
	
	// Per Instance Data
    row_major matrix matWorld : WORLD;
    row_major matrix matWV : WV;
    row_major matrix matWVP : WVP;
    uint iRowIndex : ROWINDEX;
};

struct VTX_OUT
{
    float4 vPos : SV_POSITION;
    float3 vWorldPos : POSITION;
    float2 vUV : TEXCOORD;
    float3 vViewPos : POSITION1;
};

VTX_OUT VS_Std2D_Effect(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;
	
    output.vPos = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld).xyz;
    output.vViewPos = mul(float4(_in.vPos, 1.f), g_matWV).xyz;
    output.vUV = _in.vUV;

    return output;
}

VTX_OUT VS_Std2D_Effect_Inst(VTX_IN_INST _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;
	
    output.vPos = mul(float4(_in.vPos, 1.f), _in.matWVP);
    output.vWorldPos = mul(float4(_in.vPos, 1.f), _in.matWorld).xyz;
    output.vViewPos = mul(float4(_in.vPos, 1.f), _in.matWV).xyz;
    output.vUV = _in.vUV;

    return output;
}

float4 PS_Std2D_Effect(VTX_OUT _in) : SV_Target
{
    float4 vOutColor = float4(1.f, 0.f, 1.f, 1.f);

    vOutColor = g_tex_0.Sample(g_sam_0, _in.vUV);
        	
    if(Option & OPT_BLUR)
        vOutColor = Blur(g_tex_0, _in.vUV);
    
    vOutColor.rgb += Color;
    vOutColor.a *= ALPHA;
    
    return vOutColor;
}

// ========================
// Effect2D_Emissive_Shader
// g_vec4 = Emissive Coefficient
// =======================

struct PS_OUT
{
    float4 vColor : SV_Target0;
    float4 vPosition : SV_Target2;
    float4 vEmissiv : SV_Target4;
};

PS_OUT PS_Std2D_Effect_Emsv(VTX_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
    float4 vOutColor = float4(1.f, 0.f, 1.f, 0.f);
    float4 vEmsvColor = float4(0.f, 0.f, 0.f, 0.f);

    vOutColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    
    if (vOutColor.a <= 0.4f)
        discard;
        	
    if (Option & OPT_BLUR)
        vOutColor = Blur(g_tex_0, _in.vUV);
    
    vOutColor.rgb += Color;
    vOutColor.a *= ALPHA;
    
    output.vColor = vOutColor;
    
    vEmsvColor.r = vOutColor.r * EmsvCoeff.r;
    vEmsvColor.g = vOutColor.g * EmsvCoeff.g;
    vEmsvColor.b = vOutColor.b * EmsvCoeff.b;
    vEmsvColor.a = vOutColor.a;
    
    output.vEmissiv = vEmsvColor;
    
    output.vPosition = float4(_in.vViewPos, 1.f);
    
    return output;
}

#endif