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
    float3 vCubeUV : TEXCOORD1; // 큐브의 경우 UV가 3개들어감. 애당초 큐브의 Texture를 줄때 Texture2D가 아니라 Texture Cube를 이용하기 떄문.
};

VS_OUT VS_SkyBox(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    // ViewSpace에서 카메라가 원점이기 떄문에 스카이 박스가 View로 바로 이동하면,
    // 항상 카메라 원점을 중심으로 스카이박스의 중심이 잡힌다.
    
    float3 vViewPos = mul(mul(float4(_in.vPos, 0.f), g_mat_0), g_matView).xyz;
    // 투영 행렬을 곱했을 때, w에 있는 z값이 중요하다.
    // 즉, 투영 행렬을 곱했을 때, x,y,z의 값을 그대로 사용하면 안된다.
    // w 자리의 Vz가 들어있기 때무에 투영좌표는 다음과 같이 나온다.
    // (Vx * Vz, Vy * Vz, Vz * Vz, Vz)
    // 따라서 우리가 의도한 좌표로 계산되기 위해서는 투영계산을 마친 좌표의 z를 나누어 주어야 한다.
    // RS에서 x,y가 z로 나뉘기 떄문에 우리는 w에 있는 z를 z에 그대로 넣어주면 RS를 지나며 우리가 원하는 좌표가 나온다.
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
    
    // SphereType SkyBox인 경우
    if(g_int_0 == 0)
    {
        if(g_btex_0)
        {
            vOutColor = g_tex_0.Sample(g_sam_0, _in.vUV);
        }
    }
    
    // CubeType SkyBox인 경우
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