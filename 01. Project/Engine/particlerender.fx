#ifndef _PARTICLERENDER
#define _PARTICLERENDER

#include "register.fx"
#include "func.fx"

// ========================
// Particle Render Shader
// RS_TYPE : CULL_NONE
// BS_TYPE : ALPHABLEND
// DS_TYPE : NO_WRITE
// DOMAIN  : Transparent
StructuredBuffer<tParticle> ParticleBuffer : register(t16);
#define ParticleIndex   g_int_0
#define IsWorldSpawn    g_int_1
#define Is3DParticle    g_int_2

#define StartScale      g_vec4_0
#define EndScale        g_vec4_1
#define StartColor      g_vec4_2
#define EndColor        g_vec4_3
// ========================

#define Particle ParticleBuffer[_in[0].iInstance]

struct VS_IN
{
    float3 vPos : POSITION;
    uint iInstance : SV_InstanceID;
};

struct VS_OUT
{
    float3 vLocalPos : POSITION; // ���� VertexShader���� ������ �����ϴ� ���� Rasterizer�� �ƴϱ� ������ SV�� ������ �ʴ´�.
    uint iInstance : SV_InstanceID;
};

VS_OUT VS_ParticleRender(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vLocalPos = _in.vPos;
    output.iInstance = _in.iInstance;
    
    return output;
}

// GeomtryShader�� �̿��� �� Mesh�� �簢������ ����Ƿ� �ش� VS���̴��ڵ�� ������� �ʴ´�.
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


// GeometryShader�� ���������� �߰��� ������ �������� �� �ִ�.
// GeometryShader�� DX10 ���� �߰��Ǿ�����, ����� ���� �ϴ� HS,DS�� Dx11 ���Ŀ� �߰��Ǿ���.
// GS�� GPU���� ������ ������ �߰��ϰ� �Ǹ� ȿ���� �ް��� �������Ƿ�, GS�δ� ������ ���� ���� �˰����� ����� �� ����.
// ������ HS, DS (ƽ�����̼� Tessellation)�� GPU�������� �ش� �˰����� ������ �ֱ� ������ GS���� ������ ������ ������ �� �ִ�.
// ���� GS�� ���� ������ Main���� ������� �ʴ´�. (������ ���� �������� ����Ѵ�.)
struct GS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    uint iInstance : SV_InstanceID;
};

// �簢���� ����� ���� ���� �� 6���� ������ �ϹǷ� vertexcount�� 6���� �������ش�.
[maxvertexcount(6)]
// �� �ϳ� ¥�� Mesh�� �޾ұ� ������ ���ڷ� point�� �ٿ��ش�. �޽ð� �ﰢ���� ���� ��� Triangle�� �ٿ��ش�.
// �� �츮�� �ش� ���� �ﰢ������ �����ϱ� ������ TriangleStream���� ���� �����ش�.
void GS_ParticleRender(point VS_OUT _in[1], inout TriangleStream<GS_OUT> _OutStream)
{
    GS_OUT output[4] = { (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f };
    
    // �ش� ��ƼŬ�� Ȱ��ȭ�� �Ǿ����� �ʴٸ� �׳� ��ȯ�Ѵ�.
    // �̷��� ���� ������, PS���� discard�ϴ� ����� �ִµ�, �̷� ��� �츮�� ������� �ʴ� ��ƼŬ�� �簢��Ȯ���� �̷������.
    // �׷��� �簢�� Ȯ���� �ϴ� ����� ���ʿ��� ��ƼŬ������ ��ٴ� ���̱� ������
    // GS���� Ȱ��ȭ ���� ���� ��ƼŬ���� PS���� ������ �ʰ�, �簢�� Ȯ�嵵 �̷������ �ʰ� �Ѵ�.
    // �� �ش� discard�� PS������ ����� ������ �Լ��̴�.
    if(0 == ParticleBuffer[_in[0].iInstance].iActive)
    {
        return;
    }
    
    float3 vWorldPos = _in[0].vLocalPos + ParticleBuffer[_in[0].iInstance].vRelativePos.xyz;
    
    if (Is3DParticle == 0)
    {
        if (0 == IsWorldSpawn)
        {
            vWorldPos += g_matWorld._41_42_43;
        }
    
        float3 vViewPos = mul(float4(vWorldPos, 1.f), g_matView).xyz;
    
    // ���� ����
        float fRatio = ParticleBuffer[_in[0].iInstance].fCurTime / ParticleBuffer[_in[0].iInstance].fMaxTime;
        float3 vScale = lerp(StartScale.xyz, EndScale.xyz, fRatio);
    
    // ViewSpace Ư¡ : ī�޶� �����̰�, z���� �ٶ󺸴� �����̴�.
    // ���� Local�� World�� ��ȯ�ϰ�, ViewSpace���� �簢������ Ȯ���ϸ�, �׻� ī�޶� ������ �������� Ȯ��Ǹ�,
    // z���� �ٶ󺸱� ������ ī�޶� ȸ���� �Ҷ����� �츮�� �簢���� ȸ������ �ʿ䰡 ��������.
    // ũ�⵵ �̰����� ������ ���ָ�, �� �޽ð� ����� ��ġ�ǰ�, ViewSpace�� ��ȯ�Ǿ� �װ��� �������� �簢�� Ȯ���̵ȴ�.
    // �̶�, ũ�Ⱑ Ŀ���� �ش� ũ�⸸ŭ�� �簢�� ������ �����ǰ� �ȴ�.
    // �̷� ���� ������ ViewSpace���� ���� �������ش�.
    
    
    
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
    }
    else
    {
        float3 vLocalPos = _in[0].vLocalPos;
        
        // scale
        float fRatio = Particle.fCurTime / Particle.fMaxTime;
        float3 vScale = lerp(StartScale.xyz, EndScale.xyz, fRatio);
     
        float3 NewPos[4] =
        {
            vLocalPos + float3(-0.5f, 0.5f, 0.f),
            vLocalPos + float3(0.5f, 0.5f, 0.f),
            vLocalPos + float3(0.5f, -0.5f, 0.f),
            vLocalPos + float3(-0.5f, -0.5f, 0.f)
        };  
        
        float4x4 matSRT = CreateMatrixScale(vScale);
        matSRT = mul(matSRT, CreateMatrixRot(Particle.vDir.xyz));
        matSRT = mul(matSRT, CreateMatrixTrans(Particle.vRelativePos.xyz));
        
        for (int i = 0; i < 4; i++)
        {
            NewPos[i] = mul(float4(NewPos[i], 1.f), matSRT).xyz;
        }
        
        float4x4 matWVP = (float4x4) 0.f;
        
        if(IsWorldSpawn)
        {
            matWVP = mul(mul(Particle.matWorld, g_matView), g_matProj);
        }
        
        for (int j = 0; j < 4; j++)
        {
            if(IsWorldSpawn)
            {
                output[j].vPosition = mul(float4(NewPos[j], 1.f), matWVP);
                output[j].iInstance = _in[0].iInstance;
            }
            else
            {
                output[j].vPosition = mul(float4(NewPos[j], 1.f), g_matWVP);
                output[j].iInstance = _in[0].iInstance;
            }
        }
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
     
    // �ᱹ GeometryShader���� �簢�� Mesh�� ���� �Ͱ� �ٸ��� ���� ������ �츮�� �簢�� �޽ø� ����� �Ͱ� �ٸ��� ����. 
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

float4 PS_ParticleRender_Alpha(GS_OUT _in) : SV_Target
{
    float4 vColor = (float4) 0.f;
    
    //vColor = float4(1.f, 0.f, 0.f, 1.f);
    vColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    
    float fRatio = ParticleBuffer[_in.iInstance].fCurTime / ParticleBuffer[_in.iInstance].fMaxTime;
    vColor *= lerp(StartColor, EndColor, fRatio);
    
    return vColor;
}


float4 PS_ShieldParticleRender(GS_OUT _in) : SV_Target
{
    float4 vColor = (float4) 0.f;
    
    vColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    
    // ���İ� 0�� �͵� �ڸ���
    if ( vColor.a == 0.f )
    {
        discard;
    }
    // �װ� �ƴ϶�� ���ĸ� 0.6f�� �ش�.
    else
    {
        vColor.a = 0.8f;
    }
    
    float fRatio = ParticleBuffer[_in.iInstance].fCurTime / ParticleBuffer[_in.iInstance].fMaxTime;
    vColor.rgb *= lerp(StartColor, EndColor, fRatio).rgb;
    
    return vColor;
}

#endif