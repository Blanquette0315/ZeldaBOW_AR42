#ifndef _POST_PROCESS
#define _POST_PROCESS

#include "register.fx"

// ====================
// PostProcess Shader
// mesh    : RectMesh
// BS_TYPE : Default
// DS_TYPE : NoTest NoWrite
// ====================

struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_OUT
{
    float4 vPosition : SV_Position; // SV_Position 시멘틱은 PS로 전달될 떄, 픽셀 좌표로 전달되는 것이 특징이다.
    float2 vUV : TEXCOORD;
};

//VS_OUT VS_PostProcess(VS_IN _in)
//{
//    VS_OUT output = (VS_OUT) 0.f;
//    
//    // 전체 화면을 대상으로 하기 위해서 x는 2배로 곱해 NDC좌표 범위의 최대로 설정해주었다.
//    // z축은 깊이 테스트는 하지 않지만 0이면 출력이 안될 수 있기 때문에 무난한 0.5를 주었다.
//    // 동차좌표는 1을 주어 월드 이동이 가능하도록 만들었다.
//    output.vPosition = float4(_in.vPos.xy * 2.f, 0.5f, 1.f);
//    output.vUV = _in.vUV;
//    
//    return output;
//}

//float4 PS_PostProcess(VS_OUT _in) : SV_Target
//{
//    float4 vColor = (float4) 0.f;
//    
//    vColor = g_RTCopyTex.Sample(g_sam_0, _in.vUV);
//    
//    // 흑백 화면 필터를 적용하는 것과 같은 것.
//    vColor.rgb = (vColor.r + vColor.g + vColor.b) / 3.f;
//    
//    return vColor;
//}

VS_OUT VS_PostProcess(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_PostProcess(VS_OUT _in) : SV_Target
{
    float4 vColor = (float4) 0.f;
    
    float2 vUV = _in.vPosition.xy / g_vRenderResolution.xy;
    
    // 물결처럼 흔들리는 후처리
    //vUV.y += cos((_in.vUV.x - g_fAccTime * 0.5f) * 10.f * 3.141592f) * 0.01f;
    
    // 노이즈 텍스처를 이용한 랜덤 UV
    // 수치가 계속 바뀌도록 안만들면, 불투명한 유리와 같이 만들 수 도 있을 것 같다. (그럴빠에는 텍스처로 만드는게 나을듯하다)
    float2 fAdd = float2(g_Noise.Sample(g_sam_0, _in.vUV + g_fAccTime * 0.2f).x
                         , g_Noise.Sample(g_sam_0, _in.vUV + float2(0.1f, 0.f) + g_fAccTime * 0.2f).x);
    fAdd -= fAdd / 2.f;
    vUV += fAdd * 0.05f;
    
    vColor = g_RTCopyTex.Sample(g_sam_0, vUV);
    
    //vColor.r *= 1.5f;
    
    return vColor;
}


#endif