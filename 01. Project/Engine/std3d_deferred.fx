#ifndef _STD3D_DEFERRED
#define _STD3D_DEFERRED

#include "register.fx"
#include "struct.fx"
#include "func.fx"

#define fSpecCoefficent g_float_0
#define DepthMap        g_tex_2
#define LightVP         g_mat_0

#define DepthMapResolution 4096
#define Bias               0.01f

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

// SV_Target은 뒤에 숫자를 붙여서 몇번째 렌더 타겟에 데이터를 저장할지를 의미한다.
// 우리가 기존 PS에서 사용하던 SV_Target의 의미는 SV_Target0와 같다.
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
    
    output.vColor = vObjColor;
    output.vNormal = float4(vNormal, 1.f);
    output.vPosition = float4(_in.vViewPos, 1.f);
    output.vData.x = fSpecCoefficent;
    
    // shadow
    // ViewPos -> WorldPos
    float3 vWorldPos = mul(float4(_in.vViewPos.xyz, 1.f), g_matViewInv).xyz;
    
    // WorldPos -> Light
    float4 vLightProj = mul(float4(vWorldPos, 1.f), LightVP);
    vLightProj.xyz /= vLightProj.w;

    float fShadowPow = 0.f;
    float2 vDepthMapUV = float2((vLightProj.x / 2.f) + 0.5f, -(vLightProj.y / 2.f) + 0.5f);
    
    if(g_btex_3)
    {
        if (g_int_3 > 0)
        {
            int2 iUV = vDepthMapUV.xy * int2(DepthMapResolution, DepthMapResolution);

            for (int j = 0; j < 5; j++)
            {
                for (int i = 0; i < 5; i++)
                {
                    vDepthMapUV.x = (float) (iUV.x + (j - 2) * g_int_3) / DepthMapResolution;
                    vDepthMapUV.y = (float) (iUV.y + (i - 2) * g_int_3) / DepthMapResolution;
                    float fDepth = encode(g_tex_3.Sample(g_sam_0, vDepthMapUV));
            
                    if (0.f != fDepth
                && 0.f <= vDepthMapUV.x && vDepthMapUV.x <= 1.f
                && 0.f <= vDepthMapUV.y && vDepthMapUV.y <= 1.f
                && vLightProj.z >= fDepth + Bias)
                    {
                        fShadowPow += 0.9f * GaussianFilter[j][i];
                    }
                }
            }
        }
        else
        {
            float fDepth = encode(g_tex_3.Sample(g_sam_0, vDepthMapUV));
    
            if (0.f != fDepth
        && 0.f <= vDepthMapUV.x && vDepthMapUV.x <= 1.f
        && 0.f <= vDepthMapUV.y && vDepthMapUV.y <= 1.f
        && vLightProj.z >= fDepth + Bias)
            {
                fShadowPow = 0.9f;
            }
        }
    }
    else
    {
        if (g_int_3 > 0)
        {
            int2 iUV = vDepthMapUV.xy * int2(DepthMapResolution, DepthMapResolution);

            for (int j = 0; j < 5; j++)
            {
                for (int i = 0; i < 5; i++)
                {
                    vDepthMapUV.x = (float) (iUV.x + (j - 2) * g_int_3) / DepthMapResolution;
                    vDepthMapUV.y = (float) (iUV.y + (i - 2) * g_int_3) / DepthMapResolution;
                    float fDepth = encode(DepthMap.Sample(g_sam_0, vDepthMapUV));
            
                    if (0.f != fDepth
                && 0.f <= vDepthMapUV.x && vDepthMapUV.x <= 1.f
                && 0.f <= vDepthMapUV.y && vDepthMapUV.y <= 1.f
                && vLightProj.z >= fDepth + Bias)
                    {
                        fShadowPow += 0.9f * GaussianFilter[j][i];
                    }
                }
            }
        }
        else
        {
            float fDepth = encode(DepthMap.Sample(g_sam_0, vDepthMapUV));
    
            if (0.f != fDepth
        && 0.f <= vDepthMapUV.x && vDepthMapUV.x <= 1.f
        && 0.f <= vDepthMapUV.y && vDepthMapUV.y <= 1.f
        && vLightProj.z >= fDepth + Bias)
            {
                fShadowPow = 0.9f;
            }
        }
    }
    
    output.vData.y = fShadowPow;
    
    return output;
}

#endif