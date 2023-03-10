#ifndef _SKYBOX
#define _SKYBOX

#include "register.fx"

// ==============================================
// SkyBoxShader
// Mesh : Sphere, Cube
//
// g_int_0 : SkyBox Type( 0 : Sphere, 1: Cube )
// g_tex_0 : OutPut Texture(Sphere)
// g_texCube_0 : OutPut Texture(Cube)
// ==============================================

struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    float3 vCubeUV : TEXCOORD1; // ť���� ��� UV�� 3����. �ִ��� ť���� Texture�� �ٶ� Texture2D�� �ƴ϶� Texture Cube�� �̿��ϱ� ����.
};

VS_OUT VS_SkyBox(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    // ViewSpace���� ī�޶� �����̱� ������ ��ī�� �ڽ��� View�� �ٷ� �̵��ϸ�,
    // �׻� ī�޶� ������ �߽����� ��ī�̹ڽ��� �߽��� ������.
    
    float3 vViewPos = mul(mul(float4(_in.vPos, 0.f), g_mat_0), g_matView).xyz;
    // ���� ����� ������ ��, w�� �ִ� z���� �߿��ϴ�.
    // ��, ���� ����� ������ ��, x,y,z�� ���� �״�� ����ϸ� �ȵȴ�.
    // w �ڸ��� Vz�� ����ֱ� ������ ������ǥ�� ������ ���� ���´�.
    // (Vx * Vz, Vy * Vz, Vz * Vz, Vz)
    // ���� �츮�� �ǵ��� ��ǥ�� ���Ǳ� ���ؼ��� ��������� ��ģ ��ǥ�� z�� ������ �־�� �Ѵ�.
    // RS���� x,y�� z�� ������ ������ �츮�� w�� �ִ� z�� z�� �״�� �־��ָ� RS�� ������ �츮�� ���ϴ� ��ǥ�� ���´�.
    float4 vProjPos = mul(float4(vViewPos, 1.f), g_matProj);
    vProjPos.z = vProjPos.w;
    
    output.vPosition = vProjPos;
    output.vUV = _in.vUV;
    output.vCubeUV = normalize(_in.vPos);
    
    return output;
}

float4 PS_SkyBox(VS_OUT _in) : SV_Target
{
    float4 vOutColor = float4(0.f, 0.f, 0.f, 1.f);
    
    // SphereType SkyBox�� ���
    if(g_int_0 == 0)
    {
        if(g_btex_0)
        {
            vOutColor = g_tex_0.Sample(g_sam_0, _in.vUV);
        }
    }
    
    // CubeType SkyBox�� ���
    else if(g_int_0 == 1)
    {
        if(g_btexcube_0)
        {
            vOutColor = g_texcube_0.Sample(g_sam_0, _in.vCubeUV);
        }
    }
    
    return vOutColor;
}

#endif