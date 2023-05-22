#ifndef _STD2D_EFFECT
#define _STD2D_EFFECT

#include "register.fx"
#include "func.fx"

#define ALPHA g_vec4_0.a
#define Color g_vec4_0.rgb
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
};

VTX_OUT VS_Std2D_Effect(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;
	
    output.vPos = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld).xyz;
    output.vUV = _in.vUV;

    return output;
}

VTX_OUT VS_Std2D_Effect_Inst(VTX_IN_INST _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;
	
    output.vPos = mul(float4(_in.vPos, 1.f), _in.matWVP);
    output.vWorldPos = mul(float4(_in.vPos, 1.f), _in.matWorld).xyz;
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

#endif