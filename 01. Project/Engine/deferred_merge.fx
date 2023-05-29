#ifndef _MERGE
#define _MERGE

#include "register.fx"

// ===========================
// Merge
// �ش� ���̴��� ���� ������ ���� SwapchainMRT�� �����ϴ� �뵵�� ���̴��̴�.
// ���� : ��ü�� �������ϴ� ������ ���̴��� �ƴϴ�.
// mesh : RectMesh

// Parameter
// === Texture ===
// DeferredMRT���� ����Ÿ���� �� 4���� �־��µ�,
// �����Ҷ��� ��ü�� ���� ������ ��� �÷� ���� Ÿ�ٰ� ��ü�� ��ġ ������ ��� ������ ���� Ÿ�ٸ� �ʿ��ϴ�.
// ���� ����(�ܼ� ��ü ���� ����)������ �븻�� ������.r(x)�� ��� ������ �����Ҷ� �ʿ��� �����͵��̴�.
//
// g_tex_0 : ColorTargetTex     �÷� Ÿ�ٿ��� �ؽ�ó�� ���ø��� ��ü ������ ���� �����Ѵ�.
// g_tex_1 : PositionTargetTex  ������ Ÿ�ٿ��� ��ü�� ������ ������ ��� ������, �ȼ��� a�� 1�� ��쿡�� �ش� �ȼ��� ��ü�� �����Ѵٴ� ������ ���踦 �صξ���.
// g_tex_2 : DiffuseTargetTex   ������ �׸��� �� ��, ���� RenderTarget�� �ִ� ���ݻ籤 ���� �ݻ籤 ���� �����´�.
// g_tex_3 : SpecularTargetTex
// ===========================

struct VS_IN
{
    float3 vPos : POSITION;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
};

VS_OUT VS_Merge(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    // ������ �������� �̷��� ����ִ� ������ PostProcess ���� ������ �����̴�.
    // ȭ�� ��ü�� �ȼ��� ȣ�����־�� �ϱ� ������ ������ ȭ�� 4�����̷� ����־�� �Ѵ�.
    // ���� �츮�� Local ��ǥ�迡�� RectMesh�� �Ѻ��� 0.5�� ����־��� ������
    // 2��� Ȯ���ϰ� �� ��� 1�� �ִ� ũ�Ⱑ �ȴ�.
    // �̷��� ������ ����ְ� �ȴٸ�, ȭ�� ��ü ������ �ȼ��� ȣ���� �� �ְ� �ȴ�.
    
    // w���� 1.f�� �� ������ RS�ܰ迡�� �� x,y,z������ w�� �������ִµ�, �̶� �츮�� ������ x,y,z ��ǥ�� Ʋ������ �ʰ� �ϱ� ����
    // ��, ���� �״�� �����ϱ� ���� 1.f�� �־��� ���̴�.
    
    // �� �̷� ����� ����� ��� ���� ������ ������ǥ����� ��ȯ�� �ʿ䵵 ����.
    output.vPosition = float4(_in.vPos.xy * 2.f, 0.f, 1.f);
    
    return output;
}

float4 PS_Merge(VS_OUT _in) : SV_Target0
{
    float4 vOutColor = (float4) 0.f;
    
    // �츮�� VS�� �������� ������ ������ ���ε�, SwapChainMRT�� DeferredMRT�� ���� Ÿ�ٵ��� �����ϱ� ���ؼ���
    // UV�� �˾ƾ� �Ѵ�.
    // ���������� UV�� ����� �� �ִ� ������ ������ ȭ�� �ִ������ ��ұ� ������
    // x�࿡�� �츮�� ������ ȭ�� �ػ��� 1600���� �ȼ��� ������,
    // y�࿡�� �츮�� ������ ȭ�� �ػ��� 900���� �ȼ��� ���´�.
    
    // �̸� �̿��ϸ�, ���� �ȼ�.x ������ / 1600, ���� �ȼ�.y ������ / 900
    // �� �ϰ� �Ǹ�, 0~1���̷� ����ȭ�� UV�� �˾Ƴ� �� �ִ�.
    
    // ��� ���� ������ �־��� ��, UV���� �����ؼ� �Ѱ��־��ٸ� RS�� ���� ������ ����� ���� �� �ִµ�,
    // �̷� ����� ����� ������ PostProcess�� ��� ������ �����Ұ�, ���� �־��� ���� �����ε� ����� ������ �� ������ �ϽŰ� ����.
    // ������� ������ ����Բ����� ��� �� �������� �𸣰ڴٰ� �ϽŴ�.
    float2 vUV = _in.vPosition.xy / g_vRenderResolution;
    
    float4 vViewPos = g_tex_1.Sample(g_sam_0, vUV);
    
    // tex_1�� DeferredMRT�� ����Ÿ�� �� ������ ������ ��� ����Ÿ���� ���ε� �ɾ��־���.
    // �̶�, ������ ������ a���� 0�� ��� �ƹ��� ��ü�� �������� �ʴ´ٴ� ���� �ǹ��ϴ� ������ �����ߴ�.
    // ������ �������� ���� ���ø��� �� a�� Ȯ�ο� 1.f�� �ƴ϶��,
    // ��, ��ü�� ���ٸ� �÷��� ���ø��� �ʿ䰡 ���ٴ� ���̹Ƿ� discard�� ���־���.
    //if(1.f != vViewPos.a)
    //    discard;
    
    vOutColor = g_tex_0.Sample(g_sam_0, vUV);
    
  
    
    // ���� ����
    float4 vDiffuse = g_tex_2.SampleLevel(g_sam_0, vUV, g_int_2);
    float4 vSpecular = g_tex_3.Sample(g_sam_0, vUV);
    
    if (1.f != vViewPos.a)
    {
        vOutColor.rgb += vDiffuse.rgb;
    }
    else
    {
        if (g_vec2_0.y != 0.f)
        {
            vOutColor.rgb = pow(vOutColor.rgb, 2.2);
        }
        
        vOutColor.rgb = (vOutColor.rgb * vDiffuse.rgb) + vSpecular.rgb;
        
        if (g_vec2_0.x != 0.f)
        {
            const float A = 2.51, B = 0.03, C = 2.43, D = 0.59, E = 0.14;
            vOutColor.rgb = saturate((vOutColor.rgb * (A * vOutColor.rgb + B)) / (vOutColor.rgb * (C * vOutColor.rgb + D) + E));
        }
        if (g_vec2_0.y != 0.f)
        {
            vOutColor.rgb = pow(vOutColor.rgb, 1 / 2.2);
        }
    
        if (g_float_0 != 0.f)
        {
            vOutColor.rgb = pow(vOutColor.rgb, 1 / 1.85);
        }
    }
    
    //if (g_int_0 == 1 || g_int_0 == 3)
    //{
    //    const float A = 2.51, B = 0.03, C = 2.43, D = 0.59, E = 0.14;
    //    vOutColor.rgb = saturate((vOutColor.rgb * (A * vOutColor.rgb + B)) / (vOutColor.rgb * (C * vOutColor.rgb + D) + E));
    //}
    //
    //if (g_int_0 >= 4)
    //{
    //    const float A = 6.2, B = 0.5, C = 6.2, D = 1.7, E = 0.06;
    //    vOutColor.rgb = saturate((vOutColor.rgb * (A * vOutColor.rgb + B)) / (vOutColor.rgb * (C * vOutColor.rgb + D) + E));
    //}
    //
    //if (g_int_0 == 2 || g_int_0 == 3)
    //{
    //    vOutColor.rgb = pow(vOutColor.rgb, 1 / 2.2);
    //}
    
    //const float A = 2.51, B = 0.03, C = 2.43, D = 0.59, E = 0.14;
    //vOutColor.rgb = saturate((vOutColor.rgb * (A * vOutColor.rgb + B)) / (vOutColor.rgb * (C * vOutColor.rgb + D) + E));
    //vOutColor.rgb = pow(vOutColor.rgb, 1 / 2.2);
    
    return vOutColor;
}

#endif