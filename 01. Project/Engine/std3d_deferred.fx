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
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    
    output.vViewPos = mul(float4(_in.vPos, 1.f), g_matWV).xyz;
    output.vViewTangent = normalize(mul(float4(_in.vTangent, 0.f), g_matWV).xyz);
    output.vViewNormal = normalize(mul(float4(_in.vNormal, 0.f), g_matWV).xyz);
    output.vViewBinormal = normalize(mul(float4(_in.vBinormal, 0.f), g_matWV).xyz);

    return output;
}

// SV_Target�� �ڿ� ���ڸ� �ٿ��� ���° ���� Ÿ�ٿ� �����͸� ���������� �ǹ��Ѵ�.
// �츮�� ���� PS���� ����ϴ� SV_Target�� �ǹ̴� SV_Target0�� ����.
struct PS_OUT
{
    float4 vColor       : SV_Target0;
    float4 vNormal      : SV_Target1;
    float4 vPosition    : SV_Target2;
    float4 vData        : SV_Target3;
};

PS_OUT PS_Std3D_Deferred(VS_OUT _in) : SV_Target
{
    PS_OUT output = (PS_OUT) 0.f;
    
    float4 vObjColor = float4(1.f, 0.f, 1.f, 1.f);
    
    if (g_btex_0)
    {
        vObjColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    }

    float3 vNormal = _in.vViewNormal;
    
    if (g_btex_1)
    {
        vNormal = g_tex_1.Sample(g_sam_0, _in.vUV);
        vNormal = (vNormal * 2.f) - 1.f;
        
        float3x3 matTBN =
        {
            _in.vViewTangent,
            _in.vViewBinormal,
            _in.vViewNormal,
        };
        
        vNormal = normalize(mul(vNormal, matTBN));
    }
    
    output.vColor = vObjColor;
    output.vNormal = float4(vNormal, 1.f);
    output.vPosition = float4(_in.vViewPos, 1.f);
    output.vData.x = fSpecCoefficent;
    
    return output;
}

#endif