#ifndef _UI
#define _UI

#include "register.fx"
#include "func.fx"

#define Color g_vec4_0
#define Color_Damage g_vec4_1
#define RemainHeart g_int_0


struct VTX_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VTX_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};


VTX_OUT VS_UI(VTX_IN _in) 
{
    VTX_OUT output = (VTX_OUT) 0.f;
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_UI(VTX_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;
    if(g_btex_0)
    {
        vOutColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    }
    
    if (Color.a > 0.f)
        vOutColor.rgb = Color.rgb;
    
    if (vOutColor.a <= 0.f)
        discard;
    
    if (RemainHeart == 3)
    {
        if (_in.vUV.x > 0.5f && _in.vUV.y < 0.5f)
        {
            vOutColor.rgb = Color_Damage.rgb;
            vOutColor.a *= Color_Damage.a;
        }

    }
    else if (RemainHeart == 2)
    {
        if (_in.vUV.x > 0.5f)
        {
            vOutColor.rgb = Color_Damage.rgb;
            vOutColor.a *= Color_Damage.a;
        }
    }
    else if (RemainHeart == 1)
    {
        if (_in.vUV.x > 0.5f || _in.vUV.y > 0.5f)
        {
            vOutColor.rgb = Color_Damage.rgb;
            vOutColor.a *= Color_Damage.a;
        }
    }
    else if (RemainHeart == 4)
    {
        vOutColor.rgb = Color_Damage.rgb;
        vOutColor.a *= Color_Damage.a;
    }
    
    return vOutColor;
}

#endif