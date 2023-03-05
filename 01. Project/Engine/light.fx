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
// ==============================

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
    
    // 화면 전체를 잡기위해서 Local좌표에서 2배를 키워 화면 전체 픽셀을 호출 해준다.
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
    
    // deferred MRT의 Data RenderTarget에 a자리에 스페큘러 계수를 넣어두었었다.
    float SpecCoef = g_tex_2.Sample(g_sam_0, vUV).x;
    
    output.vDiffuse = LightColor.vDiff + LightColor.vEmb;
    output.vSpecular = LightColor.vSpec * SpecCoef;
    
    // ImGui상에서 이미지로 보기 위해 알파를 1로 두었다.
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
  
    // 포지션이 없다는 것은 빛이 닿을 물체가 없다는 의미이므로 discard
    if (0.f == vViewPos.a)
    {
        discard;
    }
  
    // 내부영역 체크
    // 광원 영역에 잡힌 PositionTarget의 위치값을 광원의 로컬로 옮긴다.
    // 광원 영역에서 광원 매쉬(구형) 내부 였다면, 실제로 광원 영역내에 해당 픽셀이 있는 좌표라는 뜻이다.
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
    
    // 포지션이 없다는 것은 빛이 닿을 물체가 없다는 의미이므로 discard
    if (0.f == vViewPos.a)
    {
        discard;
    }
    
    // 내부영역 체크
    // 광원 영역에 잡힌 PositionTarget의 위치값을 광원의 로컬로 옮긴다.
    // 광원 영역에서 광원 매쉬(Cone Mesh) 내부 였다면, 실제로 광원 영역내에 해당 픽셀이 있는 좌표라는 뜻이다.
    float4 vLocalPos = mul(mul(vViewPos, g_matViewInv), g_matWorldInv);
    
    // 스포트라이트 진행 방향 구하기
    float3 LocalLightDir = normalize(float3(0.f, 0.f, 1.f));
    
    // 광원에서 물체를 향하는 단위 벡터를 구한다.
    float3 LocalLToPDir = normalize(vLocalPos.xyz);
    
    // 내적하면 cosa
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

#endif