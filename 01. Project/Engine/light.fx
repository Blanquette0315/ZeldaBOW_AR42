#ifndef _LIGHT
#define _LIGHT

#include "register.fx"
#include "struct.fx"
#include "func.fx"

// ==============================
// Directional Light
// MRT : MRT_TYPE::LIGHT
// mesh : RectMesh
//
// g_int_0 : Light Index
// g_tex_0 : Position Target tex
// g_tex_1 : Normal Target Tex
// g_tex_2 : Data Target Tex
// g_tex_3 : Depth Tex
// g_tex_5 : Emissive Tex
// ==============================

#define DepthMap        g_tex_3
#define LightVP         g_mat_0
#define StaticDepthMap  g_tex_7

#define DepthMapResolution 8192
#define Bias               0.000025f

struct VS_IN
{
    float3 vPos : POSITION;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
};

VS_OUT VS_DirLightShader(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    // ȭ�� ��ü�� ������ؼ� Local��ǥ���� 2�踦 Ű�� ȭ�� ��ü �ȼ��� ȣ�� ���ش�.
    output.vPosition = float4(_in.vPos.xy * 2.f, 0.f, 1.f);
    
    return output;
}

struct PS_OUT
{
    float4 vDiffuse : SV_Target0;
    float4 vSpecular : SV_Target1;
};

PS_OUT PS_DirLightShader(VS_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
    
    float2 vUV = _in.vPosition.xy / g_vRenderResolution;
    
    float4 vViewPos = g_tex_0.Sample(g_sam_0, vUV);
    
    if (0.f == vViewPos.a)
    {
        discard;
    }
    
    float4 vViewNormal = g_tex_1.Sample(g_sam_0, vUV);
    
    tLightColor LightColor = (tLightColor) 0.f;
    CalcLight3D(vViewPos.xyz, vViewNormal.xyz, g_int_0, LightColor);
    
    // shadow
    // ViewPos -> WorldPos
    float3 vWorldPos = mul(float4(vViewPos.xyz, 1.f), g_matViewInv).xyz;
    
    // WorldPos -> Light
    float4 vLightProj = mul(float4(vWorldPos, 1.f), LightVP);
    vLightProj.xyz /= vLightProj.w;

    float fShadowPow = 0.f;
    float2 vDepthMapUV = float2((vLightProj.x / 2.f) + 0.5f, -(vLightProj.y / 2.f) + 0.5f);
    
    if (g_btex_7 && g_int_2 != 0)
    {
        //if (g_int_3 > -1)
        //{
        int2 iUV = vDepthMapUV.xy * int2(DepthMapResolution, DepthMapResolution);

        for (int j = 0; j < 5; j++)
        {
            for (int i = 0; i < 5; i++)
            {
                vDepthMapUV.x = (float) (iUV.x + (j - 2) * (g_int_3 + 1)) / DepthMapResolution;
                vDepthMapUV.y = (float) (iUV.y + (i - 2) * (g_int_3 + 1)) / DepthMapResolution;
                float fDepth = StaticDepthMap.Sample(g_sam_0, vDepthMapUV).r;
        
                if (0.f != fDepth
            && 0.f <= vDepthMapUV.x && vDepthMapUV.x <= 1.f
            && 0.f <= vDepthMapUV.y && vDepthMapUV.y <= 1.f
            && vLightProj.z >= fDepth + Bias)
                {
                    fShadowPow += 0.9f * GaussianFilter[j][i];
                }
            }
        }
        //}
        //else
        //{
        //    float fDepth = StaticDepthMap.Sample(g_sam_0, vDepthMapUV).r;
    
        //    if (0.f != fDepth
        //&& 0.f <= vDepthMapUV.x && vDepthMapUV.x <= 1.f
        //&& 0.f <= vDepthMapUV.y && vDepthMapUV.y <= 1.f
        //&& vLightProj.z >= fDepth + Bias)
        //    {
        //        fShadowPow = 0.9f;
        //    }
        //}
    }
    
    if (fShadowPow <= 0.8f)
    {
        //if (g_int_3 > -1)
        //{
        int2 iUV = vDepthMapUV.xy * int2(DepthMapResolution, DepthMapResolution);

        for (int j = 0; j < 5; j++)
        {
            for (int i = 0; i < 5; i++)
            {
                vDepthMapUV.x = (float) (iUV.x + (j - 2) * (g_int_3 + 1)) / DepthMapResolution;
                vDepthMapUV.y = (float) (iUV.y + (i - 2) * (g_int_3 + 1)) / DepthMapResolution;
                float fDepth = DepthMap.Sample(g_sam_1, vDepthMapUV).r;
        
                if (0.f != fDepth
            && 0.f < vDepthMapUV.x && vDepthMapUV.x < 0.99f
            && 0.f < vDepthMapUV.y && vDepthMapUV.y < 0.99f
            && vLightProj.z >= fDepth + Bias)
                {
                    fShadowPow += 0.9f * GaussianFilter[j][i];
                }
            }
        }
        //}
        //else
        //{
        //    float fDepth = DepthMap.Sample(g_sam_0, vDepthMapUV).r;
            
        //    if (0.f != fDepth
        //&& 0.f < vDepthMapUV.x && vDepthMapUV.x < 0.99f
        //&& 0.f < vDepthMapUV.y && vDepthMapUV.y < 0.99f
        //&& vLightProj.z >= fDepth + Bias)
        //    {
        //        fShadowPow = 0.9f;
        //    }
        //}
        
        if (fShadowPow > 0.9f)
            fShadowPow = 0.9f;
    }
        
    // Emissive Tex Sample
    float4 vEmissive = float4(0.f, 0.f, 0.f, 1.f);
    if (g_btex_5)
    {
        vEmissive = g_tex_5.Sample(g_sam_0, vUV);
    }
    
    if(g_int_1 != 0)
    {
        output.vDiffuse = (round(LightColor.vDiff * 5) / 5.0f) * (1.f - fShadowPow) + (round(LightColor.vEmb * 5) / 5.0f) + (round(vEmissive * 5) / 5.0f);
    }
    else
    {
        output.vDiffuse = LightColor.vDiff * (1.f - fShadowPow) + LightColor.vEmb+ vEmissive;
    }
    // deferred MRT�� Data RenderTarget�� a�ڸ��� ����ŧ�� ����� �־�ξ�����.
    float SpecCoef = g_tex_2.Sample(g_sam_1, vUV).x;
    float4 vSpec = decode(SpecCoef);

    output.vSpecular.xyz = LightColor.vSpec.xyz * vSpec.xyz;
    
    // ImGui�󿡼� �̹����� ���� ���� ���ĸ� 1�� �ξ���.
    output.vDiffuse.a = 1.f;
    output.vSpecular.a = 1.f;

    return output;
}

// ==============================
// Point Light
// MRT : MRT_TYPE::LIGHT
// mesh : SphereMesh
//
// g_int_0 : Light Index
// g_tex_0 : Position Target tex
// g_tex_1 : Normal Target Tex
// g_tex_2 : Data Target Tex
// ==============================

VS_OUT VS_PointLightShader(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    
    return output;
}

PS_OUT PS_PointLightShader(VS_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
  
    float2 vUV = _in.vPosition.xy / g_vRenderResolution;
    float4 vViewPos = g_tex_0.Sample(g_sam_0, vUV);
  
    // �������� ���ٴ� ���� ���� ���� ��ü�� ���ٴ� �ǹ��̹Ƿ� discard
    if (0.f == vViewPos.a)
    {
        discard;
    }
  
    // ���ο��� üũ
    // ���� ������ ���� PositionTarget�� ��ġ���� ������ ���÷� �ű��.
    // ���� �������� ���� �Ž�(����) ���� ���ٸ�, ������ ���� �������� �ش� �ȼ��� �ִ� ��ǥ��� ���̴�.
    float4 vLocalPos = mul(mul(vViewPos, g_matViewInv), g_matWorldInv);
    if( length(vLocalPos.xyz) > 0.5f )
    {
        discard;
    }
  
    float4 vViewNormal = g_tex_1.Sample(g_sam_0, vUV);
  
    tLightColor LightColor = (tLightColor) 0.f;
    CalcLight3D(vViewPos.xyz, vViewNormal.xyz, g_int_0, LightColor);
  
    float SpecCoef = g_tex_2.Sample(g_sam_0, vUV).x;
  
    output.vDiffuse = LightColor.vDiff + LightColor.vEmb;
    output.vSpecular = LightColor.vSpec * SpecCoef;
  
    output.vDiffuse.a = 1.f;
    output.vSpecular.a = 1.f;
  
    return output;
}


// ==============================
// Spot Light
// MRT : MRT_TYPE::LIGHT
// mesh : ConeMesh
//
// g_int_0 : Light Index
// g_tex_0 : Position Target tex
// g_tex_1 : Normal Target Tex
// g_tex_2 : Data Target Tex
// ==============================

VS_OUT VS_SpotLightShader(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    
    return output;
}

PS_OUT PS_SpotLightShader(VS_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
    
    tLightInfo Lightinfo = g_Light3DBuffer[g_int_0];
    
    float2 vUV = _in.vPosition.xy / g_vRenderResolution;
    float4 vViewPos = g_tex_0.Sample(g_sam_0, vUV);
    
    // �������� ���ٴ� ���� ���� ���� ��ü�� ���ٴ� �ǹ��̹Ƿ� discard
    if (0.f == vViewPos.a)
    {
        discard;
    }
    
    // ���ο��� üũ
    // ���� ������ ���� PositionTarget�� ��ġ���� ������ ���÷� �ű��.
    // ���� �������� ���� �Ž�(Cone Mesh) ���� ���ٸ�, ������ ���� �������� �ش� �ȼ��� �ִ� ��ǥ��� ���̴�.
    float4 vLocalPos = mul(mul(vViewPos, g_matViewInv), g_matWorldInv);
    
    // ����Ʈ����Ʈ ���� ���� ���ϱ�
    float3 LocalLightDir = normalize(float3(0.f, 0.f, 1.f));
    
    // �������� ��ü�� ���ϴ� ���� ���͸� ���Ѵ�.
    float3 LocalLToPDir = normalize(vLocalPos.xyz);
    
    // �����ϸ� cosa
    float fCos = dot(LocalLightDir, LocalLToPDir);
    
    float seta = acos(fCos);
    
    if (abs(seta) > (Lightinfo.fOutAngle / 360 * 3.141592f))
    {
        discard;
    }
    
    float4 vViewNormal = g_tex_1.Sample(g_sam_0, vUV);
    
    tLightColor LightColor = (tLightColor) 0.f;
    CalcLight3D(vViewPos.xyz, vViewNormal.xyz, g_int_0, LightColor);
    
    float SpecCoef = g_tex_2.Sample(g_sam_0, vUV).x;
    
    output.vDiffuse = LightColor.vDiff + LightColor.vEmb;
    output.vSpecular = LightColor.vSpec * SpecCoef;
    
    output.vDiffuse.a = 1.f;
    output.vSpecular.a = 1.f;
    
    return output;
}

// ===============
// DepthMap Shader
// MRT : ShadowMap MRT
// RS : CULL_BACK
// BS : Default
// DS : Less
// ===============
struct VS_DEPTH_IN
{
    float3 vPos : POSITION;
    float4 vWeights : BLENDWEIGHT;
    float4 vIndices : BLENDINDICES;
};

struct VS_DEPTH_OUT
{
    float4 vPosition : SV_Position;
    float4 vProjPos : POSITION;
};

VS_DEPTH_OUT VS_DepthMap(VS_DEPTH_IN _in)
{
    VS_DEPTH_OUT output = (VS_DEPTH_OUT) 0.f;

    if (g_iAnim)
    {
        Skinning(_in.vPos, _in.vWeights, _in.vIndices, 0);
    }
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vProjPos = output.vPosition;
    
    return output;
}


float PS_DepthMap(VS_DEPTH_OUT _in) : SV_Target
{
    float fOut = 0.f;
    fOut = _in.vProjPos.z / _in.vProjPos.w;
    return fOut;
}

#endif