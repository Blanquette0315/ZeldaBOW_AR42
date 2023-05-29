#ifndef _TRAILEFFECT
#define _TRAILEFFECT

#include "register.fx"

StructuredBuffer<tTwoDot> TrailBuffer : register(t16);
#define SIZE g_int_0

//---------------
// Mesh : Point
// 
//---------------

struct VS_IN
{
    float3 iIdx : POSITION;
};

struct VS_OUT
{
    //float3 vPos : POSITION;
    float4 vPos : SV_POSITION;
};

struct GS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};


VS_OUT VS_Trail(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    uint iIsOdd = _in.iIdx % 2;
    if (iIsOdd) // odd
    {
        output.vPos = TrailBuffer[(uint)(_in.iIdx.x) / 2].vNear;
    }
    else
    {
        output.vPos = TrailBuffer[(uint)(_in.iIdx.x) / 2].vFar;
    }
    output.vPos = mul(mul(float4(output.vPos.xyz, 1.f), g_matView), g_matProj);
    
    return output;
}

//[maxvertexcount(170)]
//void GS_Trail(point VS_OUT _in[1], inout TriangleStream<GS_OUT> _OutStream)
//{
//    for (int i = 0; i < SIZE - 1; ++i)
//    {
//        GS_OUT Far0 = { TrailBuffer[i].vFar, float2(1 / SIZE * i, 0) };
//        GS_OUT Near0 = { TrailBuffer[i].vNear, float2(1 / SIZE * i, 1) };
//        GS_OUT Far1 = { TrailBuffer[i + 1].vFar, float2(1 / SIZE * (i + 1), 0) };
//        GS_OUT Near1 = { TrailBuffer[i + 1].vNear, float2(1 / SIZE * (i + 1), 1) };
        
//        _OutStream.Append(Far0);
//        _OutStream.Append(Near0);
//        _OutStream.Append(Near1);
//        _OutStream.RestartStrip();
    
//        _OutStream.Append(Far0);
//        _OutStream.Append(Near1);
//        _OutStream.Append(Near0);
//        _OutStream.RestartStrip();
//    }
//}

float4 PS_Trail(VS_OUT _in) : SV_Target
{
    float4 output = (float4) 1.f;
    output.a = 0.3f;
    
    return output;
}


#endif