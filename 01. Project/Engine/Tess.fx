#ifndef _TESS
#define _TESS

#include "register.fx"

// ================
// TessShader
// Domain : Domain_Opaque
// ================

struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_OUT
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

VS_OUT VS_TESS(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPos = _in.vPos;
    output.vUV = _in.vUV;
    
    return output;
}

// Hull Shader

struct PatchTess
{
    float EdgeFactor[3] : SV_TessFactor;
    float InsideFactor : SV_InsideTessFactor;
};

// ��ġ ��� �Լ�
// �ش� ��ġ ��� �Լ��� ��ġ�� �ѹ� ȣ��ȴ�.
// ù ���ڷ� ���� 3���� ������ ��ġ�� �޴´�. VS_OUT, 3�� �� ��. InputPatch�� ��ġ�Է��� �ǹ�
// _patchID�� ��ġ�� ���̵�
PatchTess PatchConstFunc(InputPatch <VS_OUT, 3> _patch, uint _patchID : SV_PrimitiveID)
{
    PatchTess factor = (PatchTess) 0.f;
    
    factor.EdgeFactor[0] = g_int_0; // �ش� ��ġ�� ù��° ������ ���� ���
    factor.EdgeFactor[1] = g_int_1; // �ش� ��ġ�� �ι�° ���� ���� ���
    factor.EdgeFactor[2] = g_int_2; // �ش� ��ġ�� ����° ���� ���� ���
    factor.InsideFactor = g_int_3;  // ��ġ ���� ���� ���

    return factor;
}

struct HS_OUT
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

// �ش� HullShader�� ��ġ�� �����ϴ� ��Ʈ�� ����Ʈ ���� ���� ȣ��ȴ�.
// ��, HullShader�� Patch�� �����ϴ� �������� 1�� ȣ��, ���� ������ ��ġ��� �Լ��� ��ġ�� 1�� ȣ��ȴ�.
[domain("tri")]
[outputcontrolpoints(3)]                // ��ġ�� �����ϴ� ��Ʈ�� ����Ʈ �� �Է�
[partitioning("integer")]               // �����϶� integer, �Ǽ��� �����ϰ� ���� ��� partitioning("fractional_odd")]
[outputtopology("triangle_cw")]         // triangle�̱� ������ �ﰢ������ ���� ����, cw�̱� ������ ���� ������ �ð� �������� �ﰢ������ �����ϴ� ��.
[patchconstantfunc("PatchConstFunc")]   // ��ġ ��� �Լ� �Է�
[maxtessfactor(32)]                     // �ִ� ���� ���� ��� ����
HS_OUT HS_TESS(InputPatch <VS_OUT, 3> _patch
                , uint _patchID : SV_PrimitiveID
                , uint _vtxID : SV_OutputControlPointID) // �������� ��ġ ����� ����� ���� ������, vtxID�� �ش� ��ġ�� �����ϴ� ��Ʈ�� ����Ʈ ���� 3���� ���� ���̵� �ǹ�
{
    HS_OUT output = (HS_OUT) 0.f;
    
    output.vPos = _patch[_vtxID].vPos;
    output.vUV = _patch[_vtxID].vUV;
    
    return output;
}

// Domain Shader
struct DS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};

[domain("tri")] // DS���� �ش� ��ũ������ �־��־�� �Ѵ�.
DS_OUT DS_TESS(PatchTess _tessFactor
                , const OutputPatch<HS_OUT, 3> _patch
                , float3 _Ratio : SV_DomainLocation)
{
    DS_OUT output = (DS_OUT) 0.f;
    
    // �׼������ͷ� ������ ������ ��ǥ�� ���� ���� ��ǥ�� �ƴ�, ���� ������� ������ �������� ���� �ȴ�.
    // ���� ���ҵ� ���� �� ��ǥ�� �츮�� ���ڷ� ���� ������� ����� ����ؾ� �Ѵ�.
    // ��, ��ġ�� �����ϴ� ���� �����ǿ� ���� ������ ���� ���� ������ �������� ����Ѵ�.
    float3 vLocalPos = (_patch[0].vPos * _Ratio[0]) + (_patch[1].vPos * _Ratio[1]) + (_patch[2].vPos * _Ratio[2]);
    float2 vUV = (_patch[0].vUV * _Ratio[0]) + (_patch[1].vUV * _Ratio[1]) + (_patch[2].vUV * _Ratio[2]);
    
    // HS�� ���� Tesselation�� �����ϱ� ������ VS�� ���� Local��ǥ�� �ޱ� ������ ���ݱ����� LocalSpace�󿡼��� ��ǥ�̴�.
    // ���� �ش� ���� ��ǥ�� Proj Space���� �����ؾ� �Ѵ�.
    output.vPosition = mul(float4(vLocalPos, 1.f), g_matWVP);
    output.vUV = vUV;
    
    return output;
}

float4 PS_TESS(DS_OUT _in) : SV_Target
{
    float4 vColor = float4(1.f, 0.f, 1.f, 1.f);
    
    return vColor;
}

#endif