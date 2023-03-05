#ifndef _PARTICLERENDER
#define _PARTICLERENDER

#include "register.fx"

// ========================
// Particle Render Shader
// RS_TYPE : CULL_NONE
// BS_TYPE : ALPHABLEND
// DS_TYPE : NO_WRITE
// DOMAIN  : Transparent
StructuredBuffer<tParticle> ParticleBuffer : register(t16);
#define ParticleIndex   g_int_0
#define IsWorldSpawn    g_int_1

#define StartScale      g_vec4_0
#define EndScale        g_vec4_1
#define StartColor      g_vec4_2
#define EndColor        g_vec4_3
// ========================

struct VS_IN
{
    float3 vPos : POSITION;
    uint iInstance : SV_InstanceID;
};

struct VS_OUT
{
    float3 vLocalPos : POSITION; // 이제 VertexShader에서 정보를 전달하는 곳이 Rasterizer가 아니기 때문에 SV를 붙이지 않는다.
    uint iInstance : SV_InstanceID;
};

VS_OUT VS_ParticleRender(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vLocalPos = _in.vPos;
    output.iInstance = _in.iInstance;
    
    return output;
}

// GeomtryShader를 이용해 점 Mesh를 사각형으로 만드므로 해당 VS셰이더코드는 사용하지 않는다.
//VS_OUT VS_ParticleRender(VS_IN _in)
//{
//    VS_OUT output = (VS_OUT) 0.f;
//    
//    //float3 vWorldPos = (_in.vPos * 50.f) + g_matWorld._41_42_43 + ParticleBuffer[ParticleIndex].vRelativePos.xyz;
//    float3 vWorldPos = (_in.vPos * 50.f) + g_matWorld._41_42_43 + ParticleBuffer[_in.iInstance].vRelativePos.xyz;
//    
//    float4 vViewPos = mul(float4(vWorldPos, 1.f), g_matView);
//    output.vPosition = mul(vViewPos, g_matProj);
//        
//    return output;
//}


// GeometryShader는 파이프라인 중간에 정점을 생성해줄 수 있다.
// GeometryShader는 DX10 이후 추가되었으며, 비슷한 일을 하는 HS,DS는 Dx11 이후에 추가되었다.
// GS는 GPU에서 복잡한 연산을 추가하게 되면 효율이 급격히 떨어지므로, GS로는 복잡한 정점 생성 알고리즘을 사용할 수 없다.
// 하지만 HS, DS (틱셀레이션 Tessellation)은 GPU차원에서 해당 알고리즘을 지원해 주기 때문에 GS보다 복잡한 정점을 생성할 수 있다.
// 따라서 GS는 정점 생성에 Main으로 사용하지 않는다. (간단한 정점 생성에만 사용한다.)
struct GS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    uint iInstance : SV_InstanceID;
};

// 사각형을 만들기 위해 점을 총 6개를 만들어야 하므로 vertexcount를 6으로 전달해준다.
[maxvertexcount(6)]
// 점 하나 짜리 Mesh를 받았기 때문에 인자로 point를 붙여준다. 메시가 삼각형이 었을 경우 Triangle을 붙여준다.
// 또 우리는 해당 점을 삼각형으로 분할하기 때문에 TriangleStream으로 만들어서 전해준다.
void GS_ParticleRender(point VS_OUT _in[1], inout TriangleStream<GS_OUT> _OutStream)
{
    GS_OUT output[4] = { (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f };
    
    // 해당 파티클이 활성화가 되어있지 않다면 그냥 반환한다.
    // 이렇게 하지 않으면, PS에서 discard하는 방법이 있는데, 이럴 경우 우리가 사용하지 않는 파티클도 사각형확장이 이루어진다.
    // 그러면 사각형 확장을 하는 비용이 불필요한 파티클에서도 든다는 것이기 때문에
    // GS에서 활성화 되지 않은 파티클들은 PS까지 가지도 않고, 사각형 확장도 이루어지지 않게 한다.
    // 또 해당 discard는 PS에서만 사용이 가능한 함수이다.
    if(0 == ParticleBuffer[_in[0].iInstance].iActive)
    {
        return;
    }
    
    float3 vWorldPos = _in[0].vLocalPos + ParticleBuffer[_in[0].iInstance].vRelativePos.xyz;
    
    if (0 == IsWorldSpawn)
    {
        vWorldPos += g_matWorld._41_42_43;
    }
    
    float3 vViewPos = mul(float4(vWorldPos, 1.f), g_matView).xyz;
    
    // 수명 비율
    float fRatio = ParticleBuffer[_in[0].iInstance].fCurTime / ParticleBuffer[_in[0].iInstance].fMaxTime;
    float3 vScale = lerp(StartScale.xyz, EndScale.xyz, fRatio);
    
    // ViewSpace 특징 : 카메라가 원점이고, z축을 바라보는 공간이다.
    // 따라서 Local을 World로 변환하고, ViewSpace에서 사각형으로 확장하면, 항상 카메라 원점을 기준으로 확장되며,
    // z축을 바라보기 때문에 카메라가 회전을 할때마다 우리가 사각형을 회전해줄 필요가 없어진다.
    // 크기도 이곳에서 결정을 해주면, 점 메시가 월드로 배치되고, ViewSpace로 전환되어 그곳을 기준으로 사각형 확장이된다.
    // 이때, 크기가 커지면 해당 크기만큼의 사각형 정점이 생성되게 된다.
    // 이런 이유 때문에 ViewSpace에서 점을 분할해준다.
    float3 NewPos[4] =
    {
        vViewPos - float3(-0.5f, 0.5f, 0.f) * vScale,
        vViewPos - float3(0.5f, 0.5f, 0.f) * vScale,
        vViewPos - float3(0.5f, -0.5f, 0.f) * vScale,
        vViewPos - float3(-0.5f, -0.5f, 0.f) * vScale
    };

    for (int i = 0; i < 4; i++)
    {
        output[i].vPosition = mul(float4(NewPos[i], 1.f), g_matProj);
        output[i].iInstance = _in[0].iInstance;

    }
    
    output[0].vUV = float2(0.f, 0.f);
    output[1].vUV = float2(1.f, 0.f);
    output[2].vUV = float2(1.f, 1.f);
    output[3].vUV = float2(0.f, 1.f);
    
    // 0 -- 1
    // | \  |
    // 3 -- 2
    
    _OutStream.Append(output[0]);
    _OutStream.Append(output[1]);
    _OutStream.Append(output[2]);
    _OutStream.RestartStrip();
    
    _OutStream.Append(output[0]);
    _OutStream.Append(output[2]);
    _OutStream.Append(output[3]);
    _OutStream.RestartStrip();
     
    // 결국 GeometryShader에서 사각형 Mesh를 만든 것과 다른게 없기 때문에 우리가 사각형 메시를 사용한 것과 다른게 없다. 
}

float4 PS_ParticleRender(GS_OUT _in) : SV_Target
{
    float4 vColor = (float4) 0.f;
    
    //vColor = float4(1.f, 0.f, 0.f, 1.f);
    vColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    
    float fRatio = ParticleBuffer[_in.iInstance].fCurTime / ParticleBuffer[_in.iInstance].fMaxTime;
    vColor.rgb *= lerp(StartColor, EndColor, fRatio).rgb;
    
    return vColor;
}


float4 PS_ShieldParticleRender(GS_OUT _in) : SV_Target
{
    float4 vColor = (float4) 0.f;
    
    vColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    
    // 알파가 0인 것들 자르기
    if ( vColor.a == 0.f )
    {
        discard;
    }
    // 그게 아니라면 알파를 0.6f을 준다.
    else
    {
        vColor.a = 0.8f;
    }
    
    float fRatio = ParticleBuffer[_in.iInstance].fCurTime / ParticleBuffer[_in.iInstance].fMaxTime;
    vColor.rgb *= lerp(StartColor, EndColor, fRatio).rgb;
    
    return vColor;
}

#endif