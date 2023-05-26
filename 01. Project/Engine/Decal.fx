#ifndef _DECAL
#define _DECAL

#define LayerIdx g_iLayerIdx
#define IsInstance g_bInstancing

#include "register.fx"

// ==============================
// DecalShader (Forawrd)
// MRT : MRT_TPYE::DECAL
// mesh : CubeMesh
// Blend : AlphaBlend
// DepthStencil : No_Test_No_Write
//
// g_tex_0 : Position Target Tex  그려져있는 물체 포지션을 이용해 데칼 큐브매시 영역 내에 해당 지점이 위치했는지 계산하기 위해 필요하다.
// g_tex_1 : Decal Tex
// ===============================

struct VS_IN
{
    float3 vPos : POSITION;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    row_major matrix matWInv : WInv;
    
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
    
    // 데칼용 텍스처가 셋팅되어 있지 않는다면,
    if(g_btex_1 == 0)
    {
        discard;
    }
    
    float2 vUV = _in.vPosition.xy / g_vRenderResolution;
    
    float4 vViewPos = g_tex_0.Sample(g_sam_0, vUV);
    // 만약 Position Target Tex에 알파가 0 즉, 그려진게 없다면 discard 한다.
    if (vViewPos.a == 0.f)
    {
        discard;
    }
    
    // 데칼의 큐브 볼륨 매시 내부영역 판정
    float4 vLocalPos = mul(mul(vViewPos, g_matViewInv), g_matWorldInv);
    
    // 우리가 큐브를 설계할때 가로 세로 0.5로 배치해 길이 1짜리로 잡았기 때문에 이걸로 판별이 가능하다.
    // 즉, x,y,z가 0.5 범위 내에 있는지만 체크해 주면 된다. 하지만 우리가 -0.5~0.5로 잡았기 때문에 절대값을 이용해주어야 한다.
    if (abs(vLocalPos.x) > 0.5f || abs(vLocalPos.y) > 0.5f || abs(vLocalPos.z) > 0.5f)
    {
        discard;
    }
    
    // 데칼의 UV를 구한다.
    // 내부영역 체크를 통해 discard되지 않은 지점들은 Cube의 LocalSpace상에서 x,y 성분이 -0.5 ~ 0.5 , 0.5 ~ -0.5일 것이다.
    // 때문에 -0.5 ~ 0.5, 0.5 ~ -0.5 범위를 0 ~ 1로 만들어 주면 된다.
    // x는 -0.5 ~ 0.5 범위 이므로 0.5를 더해주면 0~1의 범위가 나온다.
    // y는 0.5 ~ -0.5 범위 이므로 0.5를 더해준 다음에 1에서 빼주면 된다.
    float2 vDecalUV = float2(vLocalPos.x + 0.5f, 1 - (vLocalPos.z + 0.5f));
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
// g_tex_2 : Data Tex
// ===============================

struct VTX_IN_INST
{
    float3 vPos : POSITION;
    
     // Per Instance Data
    row_major matrix matWorld : WORLD;
    row_major matrix matWV : WV;
    row_major matrix matWVP : WVP;
    row_major matrix matWInv : WInv;
    uint iRowIndex : ROWINDEX;
};

VS_OUT VS_DeferredDecal(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    
    return output;
}

VS_OUT VS_DeferredDecal_Inst(VTX_IN_INST _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), _in.matWVP);
    output.matWInv = _in.matWInv;
    
    return output;
}

struct DeferredPS_OUT
{
    float4 vOutColor : SV_Target0;
};

DeferredPS_OUT PS_DeferredDecal(VS_OUT _in)
{
    DeferredPS_OUT output = (DeferredPS_OUT) 0.f;
    
    // 데칼용 텍스처가 셋팅되어 있지 않는다면,
    if (g_btex_1 == 0)
    {
        discard;
    }
    
    float2 vUV = _in.vPosition.xy / g_vRenderResolution;
    float iLayerIdx = g_tex_2.Sample(g_sam_1, vUV).y;
    
    if(iLayerIdx != Layer_Map)
        discard;

    
    float4 vViewPos = g_PTCopyTex.Sample(g_sam_0, vUV);
    // 만약 Position Target Tex에 알파가 0 즉, 그려진게 없다면 discard 한다.
    if (vViewPos.a == 0.f)
    {
        discard;
    }
    
    
    // 데칼의 큐브 볼륨 매시 내부영역 판정
    
    float4 vLocalPos = (float4) 0.f;
    if(g_bInstancing)
    {
        vLocalPos = mul(mul(vViewPos, g_matViewInv), _in.matWInv);
    }
    else
    {
        vLocalPos = mul(mul(vViewPos, g_matViewInv), g_matWorldInv);
    }
     
    // 우리가 큐브를 설계할때 가로 세로 0.5로 배치해 길이 1짜리로 잡았기 때문에 이걸로 판별이 가능하다.
    // 즉, x,y,z가 0.5 범위 내에 있는지만 체크해 주면 된다. 하지만 우리가 -0.5~0.5로 잡았기 때문에 절대값을 이용해주어야 한다.
    if (abs(vLocalPos.x) > 0.5f || abs(vLocalPos.y) > 0.5f || abs(vLocalPos.z) > 0.5f)
    {
        discard;
    }
    
    // 데칼의 UV를 구한다.
    float2 vDecalUV = float2(vLocalPos.x + 0.5f, 1 - (vLocalPos.z + 0.5f));
    
    float4 vDecalColor = g_tex_1.Sample(g_sam_0, vDecalUV);
    vDecalColor.rgb *= 0.1f;
    vDecalColor.a *= 0.4f;
    output.vOutColor = vDecalColor;
    
    return output;
}
#endif