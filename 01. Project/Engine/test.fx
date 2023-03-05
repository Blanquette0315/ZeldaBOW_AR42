#ifndef _TEST
#define _TEST

#include "register.fx"

// HLSL로 VertexShader 작성하기
struct VTX_IN
{
	float3 vPos : POSITION;
	float4 vColor : COLOR;
	float2 vUV : TEXCOORD;
};

struct VTX_OUT
{
	float4 vPos : SV_Position;
	float4 vColor : COLOR;
	float2 vUV : TEXCOORD;
};

VTX_OUT VS_TEST(VTX_IN _in)
{
	VTX_OUT output = (VTX_OUT)0.f;

	//output.vPos = float4(_in.vPos * 0.5f + g_vPos.xyz, 1.f);

    float4 vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld);
    float4 vViewPos = mul(vWorldPos, g_matView);
    float4 vProjPos = mul(vViewPos, g_matProj);

    output.vPos = vProjPos;

	output.vColor = _in.vColor;
	output.vUV = _in.vUV;

	return output;
}

float4 PS_TEST(VTX_OUT _in) : SV_Target
{
    float4 vOutColor = float4(1.f, 0.f, 1.f, 1.f);
    //float4 vOutColor = _in.vColor;

    /*if (g_int_0)
    {
        vOutColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    }
    else
    {
        vOutColor = g_tex_0.Sample(g_sam_1, _in.vUV);
    }*/

	return vOutColor;
}

#endif
