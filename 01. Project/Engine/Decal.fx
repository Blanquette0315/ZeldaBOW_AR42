#ifndef _DECAL
#define _DECAL

#include "register.fx"

// ==============================
// DecalShader (Forawrd)
// MRT : MRT_TPYE::DECAL
// mesh : CubeMesh
// Blend : AlphaBlend
// DepthStencil : No_Test_No_Write
//
// g_tex_0 : Position Target Tex  �׷����ִ� ��ü �������� �̿��� ��Į ť��Ž� ���� ���� �ش� ������ ��ġ�ߴ��� ����ϱ� ���� �ʿ��ϴ�.
// g_tex_1 : Decal Tex
// ===============================

struct VS_IN
{
    float3 vPos : POSITION;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
};

VS_OUT VS_Decal(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    
    return output;
}

struct PS_OUT
{
    float4 vOutColor : SV_Target0;
};

PS_OUT PS_Decal(VS_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
    
    // ��Į�� �ؽ�ó�� ���õǾ� ���� �ʴ´ٸ�,
    if(g_btex_1 == 0)
    {
        discard;
    }
    
    float2 vUV = _in.vPosition.xy / g_vRenderResolution;
    
    float4 vViewPos = g_tex_0.Sample(g_sam_0, vUV);
    // ���� Position Target Tex�� ���İ� 0 ��, �׷����� ���ٸ� discard �Ѵ�.
    if (vViewPos.a == 0.f)
    {
        discard;
    }
    
    // ��Į�� ť�� ���� �Ž� ���ο��� ����
    float4 vLocalPos = mul(mul(vViewPos, g_matViewInv), g_matWorldInv);
    
    // �츮�� ť�긦 �����Ҷ� ���� ���� 0.5�� ��ġ�� ���� 1¥���� ��ұ� ������ �̰ɷ� �Ǻ��� �����ϴ�.
    // ��, x,y,z�� 0.5 ���� ���� �ִ����� üũ�� �ָ� �ȴ�. ������ �츮�� -0.5~0.5�� ��ұ� ������ ���밪�� �̿����־�� �Ѵ�.
    if (abs(vLocalPos.x) > 0.5f || abs(vLocalPos.y) > 0.5f || abs(vLocalPos.z) > 0.5f)
    {
        discard;
    }
    
    // ��Į�� UV�� ���Ѵ�.
    // ���ο��� üũ�� ���� discard���� ���� �������� Cube�� LocalSpace�󿡼� x,y ������ -0.5 ~ 0.5 , 0.5 ~ -0.5�� ���̴�.
    // ������ -0.5 ~ 0.5, 0.5 ~ -0.5 ������ 0 ~ 1�� ����� �ָ� �ȴ�.
    // x�� -0.5 ~ 0.5 ���� �̹Ƿ� 0.5�� �����ָ� 0~1�� ������ ���´�.
    // y�� 0.5 ~ -0.5 ���� �̹Ƿ� 0.5�� ������ ������ 1���� ���ָ� �ȴ�.
    float2 vDecalUV = float2(vLocalPos.x + 0.5f, 1 - (vLocalPos.y + 0.5f));
    float4 vDecalColor = g_tex_1.Sample(g_sam_0, vDecalUV);
    output.vOutColor = vDecalColor;
    
    return output;
}

// ==============================
// Deferred_DecalShader (Deferred)
// MRT : MRT_TPYE::Decal
// mesh : CubeMesh
// Blend : AlphaBlend
// DepthStencil : No_Test_No_Write
//
// g_tex_0 : Position Target Tex
// g_tex_1 : Decal Tex
// ===============================

VS_OUT VS_DeferredDecal(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    
    return output;
}

struct DeferredPS_OUT
{
    float4 vOutColor : SV_Target0;
};

DeferredPS_OUT PS_DeferredDecal(VS_OUT _in)
{
    DeferredPS_OUT output = (DeferredPS_OUT) 0.f;
    
    // ��Į�� �ؽ�ó�� ���õǾ� ���� �ʴ´ٸ�,
    if (g_btex_1 == 0)
    {
        discard;
    }
    
    float2 vUV = _in.vPosition.xy / g_vRenderResolution;
    
    float4 vViewPos = g_PTCopyTex.Sample(g_sam_0, vUV);
    // ���� Position Target Tex�� ���İ� 0 ��, �׷����� ���ٸ� discard �Ѵ�.
    if (vViewPos.a == 0.f)
    {
        discard;
    }
    
    // ��Į�� ť�� ���� �Ž� ���ο��� ����
    float4 vLocalPos = mul(mul(vViewPos, g_matViewInv), g_matWorldInv);
    
    // �츮�� ť�긦 �����Ҷ� ���� ���� 0.5�� ��ġ�� ���� 1¥���� ��ұ� ������ �̰ɷ� �Ǻ��� �����ϴ�.
    // ��, x,y,z�� 0.5 ���� ���� �ִ����� üũ�� �ָ� �ȴ�. ������ �츮�� -0.5~0.5�� ��ұ� ������ ���밪�� �̿����־�� �Ѵ�.
    if (abs(vLocalPos.x) > 0.5f || abs(vLocalPos.y) > 0.5f || abs(vLocalPos.z) > 0.5f)
    {
        discard;
    }
    
    // ��Į�� UV�� ���Ѵ�.
    float2 vDecalUV = float2(vLocalPos.x + 0.5f, 1 - (vLocalPos.y + 0.5f));
    float4 vDecalColor = g_tex_1.Sample(g_sam_0, vDecalUV);
    output.vOutColor = vDecalColor;
    
    return output;
}
#endif