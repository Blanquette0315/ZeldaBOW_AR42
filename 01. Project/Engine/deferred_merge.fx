#ifndef _MERGE
#define _MERGE

#include "register.fx"

// ===========================
// Merge
// 해당 셰이더는 지연 렌더링 사용시 SwapchainMRT와 병합하는 용도의 셰이더이다.
// 주의 : 물체를 랜더링하는 목적의 셰이더는 아니다.
// mesh : RectMesh

// Parameter
// === Texture ===
// DeferredMRT에는 랜더타겟이 총 4장이 있었는데,
// 병합할때는 물체의 색상 정보가 담긴 컬러 랜더 타겟과 물체의 위치 정보가 담긴 포지션 랜더 타겟만 필요하다.
// 현재 시점(단순 물체 색상만 병합)에서는 노말과 데이터.r(x)의 경우 광원을 적용할때 필요한 데이터들이다.
//
// g_tex_0 : ColorTargetTex     컬러 타겟에는 텍스처를 샘플링한 물체 본연의 색이 존재한다.
// g_tex_1 : PositionTargetTex  포지션 타겟에는 물체의 포지션 정보가 들어 있으며, 픽셀의 a가 1일 경우에만 해당 픽셀에 물체가 존재한다는 것으로 설계를 해두었다.
// g_tex_2 : DiffuseTargetTex   광원을 그리고 난 뒤, 광원 RenderTarget에 있는 난반사광 값과 반사광 값을 가져온다.
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
    
    // 정점의 포지션을 이렇게 잡아주는 이유는 PostProcess 때와 동일한 이유이다.
    // 화면 전체의 픽셀을 호출해주어야 하기 때문에 정점은 화면 4모퉁이로 잡아주어야 한다.
    // 따라서 우리가 Local 좌표계에서 RectMesh를 한변을 0.5로 잡아주었기 때문에
    // 2배로 확장하게 될 경우 1로 최대 크기가 된다.
    // 이렇게 정점을 잡아주게 된다면, 화면 전체 범위의 픽셀을 호출할 수 있게 된다.
    
    // w값을 1.f로 준 이유는 RS단계에서 각 x,y,z성분을 w로 나누어주는데, 이때 우리가 설정한 x,y,z 좌표가 틀어지지 않게 하기 위해
    // 즉, 원본 그대로 유지하기 위해 1.f를 넣어준 것이다.
    
    // 또 이런 방법을 사용할 경우 굳이 정점을 투영좌표계까지 변환할 필요도 없다.
    output.vPosition = float4(_in.vPos.xy * 2.f, 0.f, 1.f);
    
    return output;
}

float4 PS_Merge(VS_OUT _in) : SV_Target0
{
    float4 vOutColor = (float4) 0.f;
    
    // 우리가 VS에 전달해준 정보는 포지션 뿐인데, SwapChainMRT에 DeferredMRT의 랜더 타겟들을 병합하기 위해서는
    // UV를 알아야 한다.
    // 정점만으로 UV를 계산할 수 있는 이유는 정점을 화면 최대범위로 잡았기 때문에
    // x축에는 우리가 설정한 화면 해상도인 1600개의 픽셀이 들어오고,
    // y축에는 우리가 설정한 화면 해상도인 900개의 픽셀이 들어온다.
    
    // 이를 이용하면, 현재 픽셀.x 포지션 / 1600, 현재 픽셀.y 포지션 / 900
    // 을 하게 되면, 0~1사이로 정규화된 UV를 알아낼 수 있다.
    
    // 사실 정점 정보를 넣어줄 때, UV까지 셋팅해서 넘겨주었다면 RS를 거쳐 동일한 결과를 얻을 수 있는데,
    // 이런 방식을 사용한 이유는 PostProcess때 배운 내용을 복습할겸, 현재 넣어준 정보 만으로도 계산이 가능한 점 때문에 하신거 같다.
    // 비용적인 측면은 강사님께서도 어떤게 더 좋을지는 모르겠다고 하신다.
    float2 vUV = _in.vPosition.xy / g_vRenderResolution;
    
    float4 vViewPos = g_tex_1.Sample(g_sam_0, vUV);
    
    // tex_1은 DeferredMRT의 랜더타겟 중 포지션 정보가 담긴 랜더타겟을 바인딩 걸어주었다.
    // 이때, 포지션 정보의 a값이 0일 경우 아무런 물체가 존재하지 않는다는 것을 의미하는 것으로 설계했다.
    // 때문에 포지션을 먼저 샘플링한 뒤 a를 확인에 1.f가 아니라면,
    // 즉, 물체가 없다면 컬러를 샘플링할 필요가 없다는 뜻이므로 discard를 해주었다.
    //if(1.f != vViewPos.a)
    //    discard;
    
    vOutColor = g_tex_0.Sample(g_sam_0, vUV);
    
  
    
    // 광원 적용
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