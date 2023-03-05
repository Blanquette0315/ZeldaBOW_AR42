#ifndef _STD2D
#define _STD2D

#include "register.fx"
#include "func.fx"

// HLSL로 VertexShader 작성하기
struct VTX_IN
{
	float3 vPos : POSITION;
	float2 vUV : TEXCOORD;
};

struct VTX_OUT
{
	float4 vPos : SV_POSITION;
	float3 vWorldPos : POSITION;
	float2 vUV : TEXCOORD;
};

VTX_OUT VS_Std2D(VTX_IN _in)
{
	VTX_OUT output = (VTX_OUT)0.f;
	
	output.vPos = mul(float4(_in.vPos, 1.f), g_matWVP);
	output.vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld).xyz;
	output.vUV = _in.vUV;

	return output;
}

float4 PS_Std2D(VTX_OUT _in) : SV_Target
{
	float4 vOutColor = float4(1.f, 0.f, 1.f, 0.f);

	if (g_iAnim2DUse)
	{
		float2 vDiff = (g_vFullSize - g_vSlice) / 2.f;
		float2 vUV = (g_vLeftTop - vDiff - g_vOffset) + (g_vFullSize * _in.vUV);
	
		if (vUV.x < g_vLeftTop.x || g_vLeftTop.x + g_vSlice.x < vUV.x
			|| vUV.y < g_vLeftTop.y || g_vLeftTop.y + g_vSlice.y < vUV.y)
		{
			discard;
		}
	
		vOutColor = g_Atlas.Sample(g_sam_1, vUV);
	}
	else
	{
        if (g_btex_0)        
            vOutColor = g_tex_0.Sample(g_sam_0, _in.vUV);
        else
            vOutColor = float4(1.f, 0.f, 1.f, 1.f);
	}

	// 노이즈 텍스쳐 Burn 효과
    //if ( 0.f < g_float_0)
    //{
    //    float PrevAlpha = vOutColor.a;
    //    vOutColor.a -= g_Noise.Sample(g_sam_0, _in.vUV).x * 0.8f + g_float_0 * 0.3f;
	//	
    //    float DeltaAlpha = PrevAlpha - vOutColor.a;
	//	
    //    vOutColor.rgb *= float3(2.f, 1.f, 1.f) * DeltaAlpha;
    //}    
	
	if (0.f >= vOutColor.a)
		discard;

	// 광원 처리
	tLightColor color = (tLightColor) 0.f;
	
	for (int i = 0; i < g_iLight2DCount; ++i)
	{
		CalcLight2D(color, _in.vWorldPos, i);
	}

	vOutColor *= color.vDiff;
	
	//if(g_float_2 == 0.5f)
    //    discard;

	return vOutColor;
}



float4 PS_Std2D_AlphaBlend(VTX_OUT _in) : SV_Target
{
	float4 vOutColor = float4(1.f, 0.f, 1.f, 1.f);
	
	// 애니메이션 추가할 곳.

	vOutColor = g_tex_0.Sample(g_sam_0, _in.vUV);
	
	// 광원 처리
    tLightColor color = (tLightColor) 0.f;
    color.vDiff.w = 1.f;
    color.vEmb.w = 1.f;
    color.vSpec.w = 1.f;
	
    for (int i = 0; i < g_iLight2DCount; ++i)
    {
        CalcLight2D(color, _in.vWorldPos, i);
    }
	
    vOutColor *= color.vDiff;

	return vOutColor;
}

#endif