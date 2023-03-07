#ifndef _FRUSTUM
#define _FRUSTUM

#include "register.fx"

// ===================================
// Frustum Render Shader
//
// Mesh : Point Mesh
//
// RS_TYPE : CULL_NONE
// BS_TYPE : ALPHABLEND
// DS_TYPE : NO_WRITE
// DOMAIN : TRANSPARENT
#define matInv g_mat_0
#define COLOR g_vec4_0
// ===================================

struct VS_IN
{
    float3 vPos : POSITION;
};

struct VS_OUT
{
    float3 vLocalPos : POSITION;
};

VS_OUT VS_Frustum(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vLocalPos = float3(0.f, 0.f, 0.f);
    
    return output;
}

struct GS_OUT
{
    float4 vPosition : SV_Position;
};

// 원래 포인트 매쉬가 0,0,0 기준에서 확장
[maxvertexcount(16)]
void GS_Frustum(point VS_OUT _in[1], inout LineStream<GS_OUT> _OutStream)
{
    GS_OUT output[8] = { (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f };
    
    // 투영공간 좌표
	//     4 ------ 5
	//     |        |  Far
	//   / |        |
	//	/  7 ------ 6	
	// /      /
	// 0 -- 1     /
	// |    |    / Near
	// 3 -- 2
    
    float3 vProjPos[8] =
    {
        float3(-1.f, 1.f, 0.f),
        float3(1.f, 1.f, 0.f),
        float3(1.f, -1.f, 0.f),
        float3(-1.f, -1.f, 0.f),

        float3(-1.f, 1.f, 1.f),
        float3(1.f, 1.f, 1.f),
        float3(1.f, -1.f, 1.f),
        float3(-1.f, -1.f, 1.f),
    };
    
    // 잡아둔 정점을 역행렬을 곱해, Frustum으로 보내주고, 카메라의 월드 행렬을 곱해 월드 좌표로 잡아준다.
    for (int i = 0; i < 8; ++i)
    {
        float4 vPos = mul(float4(vProjPos[i], 1.f), matInv);
        vPos /= vPos.w;
        output[i].vPosition = mul(mul(vPos, g_matView), g_matProj);
    }

    _OutStream.Append(output[0]);
    _OutStream.Append(output[4]);
    _OutStream.Append(output[5]);
    _OutStream.Append(output[1]);
    _OutStream.Append(output[2]);
    _OutStream.Append(output[6]);
    _OutStream.Append(output[7]);
    _OutStream.Append(output[3]);
    _OutStream.Append(output[0]);
    _OutStream.Append(output[1]);
    _OutStream.RestartStrip();
    
    _OutStream.Append(output[5]);
    _OutStream.Append(output[6]);
    _OutStream.RestartStrip();
    
    _OutStream.Append(output[4]);
    _OutStream.Append(output[7]);
    _OutStream.RestartStrip();
    
    _OutStream.Append(output[2]);
    _OutStream.Append(output[3]);
    _OutStream.RestartStrip();
}

float4 PS_Frustum(GS_OUT _in) : SV_Target
{
    float4 vColor = (float4) 0.f;
    
    vColor = COLOR;
    
    return vColor;
}

#endif