#ifndef _MAXLANDSCAPE
#define _MAXLANDSCAPE

#include "register.fx"
#include "func.fx"

// =========================
// TessShader
// Domain : DOMAIN_DEFERRED
// =========================
#define                         FACE_X                          g_int_0
#define                         FACE_Z                          g_int_1  

#define                         HeightMap                       g_tex_0     // ���̸�
#define                         HeightMapResolution             g_vec2_0    // ���̸� �ػ�

struct VSMAX_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VSMAX_OUT
{
    float3 vLocalPos : POSITION;
    float2 vUV : TEXCOORD;
    
    float3 vViewPos : POSITION1;
};


VSMAX_OUT VS_MaxLandScape(VSMAX_IN _in)
{
    VSMAX_OUT output = (VSMAX_OUT) 0.f;
    
    output.vLocalPos = _in.vPos;
    output.vUV = _in.vUV;
    
    output.vViewPos = mul(float4(_in.vPos, 1.f), g_matWV).xyz;
    
    return output;
}

// Hull Shader
struct PatchMaxTess
{
    float EdgeFactor[3] : SV_TessFactor;
    float InsideFactor : SV_InsideTessFactor;
};


// ��ġ ��� �Լ�
PatchMaxTess PatchMaxConstFunc(InputPatch<VSMAX_OUT, 3> _patch, uint _patchId : SV_PrimitiveID)
{
    PatchMaxTess factor = (PatchMaxTess) 0.f;
    
    factor.EdgeFactor[0] = 16;
    factor.EdgeFactor[1] = 16;
    factor.EdgeFactor[2] = 16;
    factor.InsideFactor = 16;
    
    return factor;
}


struct HSMAX_OUT
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};


[domain("tri")]
[outputcontrolpoints(3)]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[patchconstantfunc("PatchMaxConstFunc")]
[maxtessfactor(32)]
HSMAX_OUT HS_MaxLandScape(InputPatch<VSMAX_OUT, 3> _patch
                , uint _patchId : SV_PrimitiveID
                , uint _vtxId : SV_OutputControlPointID)
{
    HSMAX_OUT output = (HSMAX_OUT) 0.f;
    
    output.vPos = _patch[_vtxId].vLocalPos;
    output.vUV = _patch[_vtxId].vUV;
        
    return output;
}



// Domain Shader
struct DSMAX_OUT
{
    float3 vPosition : POSITION;
};

[domain("tri")]
DSMAX_OUT DS_MaxLandScape(PatchMaxTess _tessFactor
                , const OutputPatch<HSMAX_OUT, 3> _patch
                , float3 _Ratio : SV_DomainLocation)
{
    DSMAX_OUT output = (DSMAX_OUT) 0.f;
    
    float3 vLocalPos = (_patch[0].vPos * _Ratio[0]) + (_patch[1].vPos * _Ratio[1]) + (_patch[2].vPos * _Ratio[2]);
    float2 vUV = (_patch[0].vUV * _Ratio[0]) + (_patch[1].vUV * _Ratio[1]) + (_patch[2].vUV * _Ratio[2]);
    float2 FullUV = vUV / float2(FACE_X, FACE_Z);
    
    // ���̸� �ȼ��� UV ����
    float2 vLandscapeUVStep = float2(1.f / HeightMapResolution.x, 1.f / HeightMapResolution.y);

    // ���� �ֺ�(��, �Ʒ�, ��, ��) ���̸� ���ø� ��ġ
    float2 vLandScapeUpUV = float2(FullUV.x, FullUV.y - vLandscapeUVStep.y);
    float2 vLandScapeDownUV = float2(FullUV.x, FullUV.y + vLandscapeUVStep.y);
    float2 vLandScapeLeftUV = float2(FullUV.x - vLandscapeUVStep.x, FullUV.y);
    float2 vLandScapeRightUV = float2(FullUV.x + vLandscapeUVStep.x, FullUV.y);

    // �ȼ� �� �������� ��ȭ��
    float2 vLandscapeLocalposStep = float2(FACE_X / HeightMapResolution.x, FACE_Z / HeightMapResolution.y);

    // �ֺ� �ȼ��� �����ϴ� �ֺ� ��������
    float3 vLocalUpPos = float3(vLocalPos.x, HeightMap.SampleLevel(g_sam_0, vLandScapeUpUV, 0).x, vLocalPos.z + vLandscapeLocalposStep.y);
    float3 vLocalDownPos = float3(vLocalPos.x, HeightMap.SampleLevel(g_sam_0, vLandScapeDownUV, 0).x, vLocalPos.z - vLandscapeLocalposStep.y);
    float3 vLocalLeftPos = float3(vLocalPos.x - vLandscapeLocalposStep.x, HeightMap.SampleLevel(g_sam_0, vLandScapeLeftUV, 0).x, vLocalPos.z);
    float3 vLocalRightPos = float3(vLocalPos.x + vLandscapeLocalposStep.x, HeightMap.SampleLevel(g_sam_0, vLandScapeRightUV, 0).x, vLocalPos.z);

    vLocalUpPos = mul(float4(vLocalUpPos, 1.f), g_matWV).xyz;
    vLocalDownPos = mul(float4(vLocalDownPos, 1.f), g_matWV).xyz;
    vLocalLeftPos = mul(float4(vLocalLeftPos, 1.f), g_matWV).xyz;
    vLocalRightPos = mul(float4(vLocalRightPos, 1.f), g_matWV).xyz;

    // Tangent, Binormal, Normal �� ���        
    //output.vViewTangent = normalize(vLocalRightPos - vLocalLeftPos).xyz;
    //output.vViewBinormal = normalize(vLocalDownPos - vLocalUpPos).xyz;
    //output.vViewNormal = normalize(cross(output.vViewTangent, output.vViewBinormal).xyz).xyz;


    // ������ǥ ����
    vLocalPos.y = HeightMap.SampleLevel(g_sam_0, FullUV, 0).x;
    output.vPosition = vLocalPos;
    // mul(float4(vLocalPos, 1.f), g_matWVP);
            
    return output;
};


struct GS_OUT
{
    float3 vPosition : POSITION;
};

[maxvertexcount(3)]
void GS_StreamOut(triangle DSMAX_OUT _in[3], inout TriangleStream<GS_OUT> _outputStream)
{
    // �Է� ������ ������ŭ �ݺ��ϸ鼭 ��� ��Ʈ���� �߰�
    for (int i = 0; i < 3; ++i)
    {
        GS_OUT output;
        output.vPosition = _in[i].vPosition;
        _outputStream.Append(output);
    }
}

#endif