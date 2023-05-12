#ifndef _STD3D
#define _STD3D

#include "register.fx"
#include "func.fx"

// �ش� Shader�ڵ�� ������ �������� �Ҷ� ����Ѵ�.
// ������ �������� MRT�� ����Ÿ�� ����� ������ ���常 ����ϴ� ������ ������ ����̴�.

// ===========================
// Std3DShader
// g_float_0 : Spec Coefficent
// g_tex_0   : ����� �ؽ�ó
// g_tex_1   : �븻��
// ===========================

#define fSpecCoefficent g_float_0

struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
    
    float3 vTangent : TANGENT;
    float3 vNormal : NORMAL;
    float3 vBinormal : BINORMAL;
};


struct VTX_IN_INST
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
    
    float3 vTangent : TANGENT;
    float3 vNormal : NORMAL;
    float3 vBinormal : BINORMAL;
    
    // Per Instance Data
    row_major matrix matWorld : WORLD;
    row_major matrix matWV : WV;
    row_major matrix matWVP : WVP;
    uint iRowIndex : ROWINDEX;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    
    float3 vViewPos : POSITION;
    
    float3 vViewTangent : TANGENT;
    float3 vViewNormal : NORMAL;
    float3 vViewBinormal : BINORMAL;
};

VS_OUT VS_Std3D(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    
    // Phong Shading
    output.vViewPos = mul(float4(_in.vPos, 1.f), g_matWV).xyz;
    output.vViewTangent = normalize(mul(float4(_in.vTangent, 0.f), g_matWV).xyz);
    output.vViewNormal = normalize(mul(float4(_in.vNormal, 0.f), g_matWV).xyz);
    output.vViewBinormal = normalize(mul(float4(_in.vBinormal, 0.f), g_matWV).xyz);
        
    // Gouraud Shading
    //float3 vWorldNormal = normalize(mul(float4(_in.vNormal, 0.f), g_matWorld).xyz);
    //float3 vWorldPos = normalize(mul(float4(_in.vPos, 1.f), g_matWorld).xyz);
    
    //float3 LightDir = normalize(g_Light3DBuffer[0].vWorldDir);
    //output.fPow = saturate(dot(-LightDir, vWorldNormal));
    
    return output;
}

VS_OUT VS_Std3D_Inst(VTX_IN_INST _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), _in.matWVP);
    output.vUV = _in.vUV;
    
    // Phong Shading
    output.vViewPos = mul(float4(_in.vPos, 1.f), _in.matWV).xyz;
    output.vViewTangent = normalize(mul(float4(_in.vTangent, 0.f), _in.matWV).xyz);
    output.vViewNormal = normalize(mul(float4(_in.vNormal, 0.f), _in.matWV).xyz);
    output.vViewBinormal = normalize(mul(float4(_in.vBinormal, 0.f), _in.matWV).xyz);
    
    return output;
}


float4 PS_Std3D(VS_OUT _in) : SV_Target
{
    float4 vOutColor = float4(1.f, 0.f, 1.f, 1.f);
    
    // ��ü ���� �ؽ�ó
    if(g_btex_0)
    {
        vOutColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    }
    
    // �븻�� (�⺻�����δ� ������ ���� �븻���� ������ ��ġ�� ����Ѵ�.)
    // ������ tex_1�� �븻���� ���ε� �Ǹ�, �븻�� ���ؼ� �ش� �븻���� ����� ���̴�.
    float3 vNormal = _in.vViewNormal;
    
    if(g_btex_1)
    {
        // ��ü�� ǥ�鿡 ����� ź��Ʈ �����̽� ���� ���⺤�͸� �����´�.
        vNormal = g_tex_1.Sample(g_sam_0, _in.vUV).xyz;
        vNormal = (vNormal * 2.f) - 1.f;
        
        // (1, 0, 0) -> _in.vViewTangent
        // (0, 1, 0) -> _in.vViewBinormal
        // (0, 0, 1) -> _in.vViewNormal
        
        float3x3 matTBN =
        {
            _in.vViewTangent,
            _in.vViewBinormal,
            _in.vViewNormal,
        };
        
        vNormal = normalize(mul(vNormal, matTBN));
    }
    
    // ǥ��(�ȼ�)�� �޴� ���� �ѷ��� ������ ����
    tLightColor LightColor = (tLightColor) 0.f;
    
    for (int i = 0; i < g_iLight3DCount; ++i)
    {
        CalcLight3D(_in.vViewPos, vNormal, i, LightColor);
    }
        
    // ��ü�� ���� ���� ����
    vOutColor.rgb = (vOutColor.rgb * LightColor.vDiff.rgb)
                    + LightColor.vSpec.rgb * fSpecCoefficent
                    + (vOutColor.rgb * LightColor.vEmb.rgb);
    
    return vOutColor;
}





#endif