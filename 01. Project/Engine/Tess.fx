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

// 패치 상수 함수
// 해당 패치 상수 함수는 패치당 한번 호출된다.
// 첫 인자로 정점 3개로 구성된 패치를 받는다. VS_OUT, 3이 그 뜻. InputPatch가 패치입력을 의미
// _patchID가 패치당 아이디
PatchTess PatchConstFunc(InputPatch <VS_OUT, 3> _patch, uint _patchID : SV_PrimitiveID)
{
    PatchTess factor = (PatchTess) 0.f;
    
    factor.EdgeFactor[0] = g_int_0; // 해당 패치의 첫번째 정점의 분할 계수
    factor.EdgeFactor[1] = g_int_1; // 해당 패치의 두번째 정점 분할 계수
    factor.EdgeFactor[2] = g_int_2; // 해당 패치의 세번째 정점 분할 계수
    factor.InsideFactor = g_int_3;  // 패치 내부 분할 계수

    return factor;
}

struct HS_OUT
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

// 해당 HullShader는 패치를 구성하는 컨트롤 포인트 정점 마다 호출된다.
// 즉, HullShader는 Patch를 구성하는 정점마다 1번 호출, 포괄 적으로 패치상수 함수는 패치당 1번 호출된다.
[domain("tri")]
[outputcontrolpoints(3)]                // 패치를 구성하는 컨트롤 포인트 수 입력
[partitioning("integer")]               // 정수일때 integer, 실수로 분할하고 싶을 경우 partitioning("fractional_odd")]
[outputtopology("triangle_cw")]         // triangle이기 떄문에 삼각형으로 분할 구성, cw이기 때문에 분할 정점을 시계 방향으로 삼각형으로 연결하는 것.
[patchconstantfunc("PatchConstFunc")]   // 패치 상수 함수 입력
[maxtessfactor(32)]                     // 최대 정점 분할 계수 설정
HS_OUT HS_TESS(InputPatch <VS_OUT, 3> _patch
                , uint _patchID : SV_PrimitiveID
                , uint _vtxID : SV_OutputControlPointID) // 나머지는 패치 상수의 설명과 동일 하지만, vtxID는 해당 패치를 구성하는 컨트롤 포인트 정점 3개의 각각 아이디를 의미
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

[domain("tri")] // DS에도 해당 디스크립션을 넣어주어야 한다.
DS_OUT DS_TESS(PatchTess _tessFactor
                , const OutputPatch<HS_OUT, 3> _patch
                , float3 _Ratio : SV_DomainLocation)
{
    DS_OUT output = (DS_OUT) 0.f;
    
    // 테셀레이터로 생성된 정점의 좌표는 순수 정점 좌표가 아닌, 기준 정점들로 부터의 비율값이 전달 된다.
    // 따라서 분할된 정점 별 좌표를 우리가 인자로 들어온 정보들로 계산해 사용해야 한다.
    // 즉, 패치를 구성하는 정점 포지션에 각각 비율을 곱해 실제 정점의 포지션을 계산한다.
    float3 vLocalPos = (_patch[0].vPos * _Ratio[0]) + (_patch[1].vPos * _Ratio[1]) + (_patch[2].vPos * _Ratio[2]);
    float2 vUV = (_patch[0].vUV * _Ratio[0]) + (_patch[1].vUV * _Ratio[1]) + (_patch[2].vUV * _Ratio[2]);
    
    // HS를 거쳐 Tesselation을 진행하기 까지는 VS로 부터 Local좌표를 받기 때문에 지금까지는 LocalSpace상에서의 좌표이다.
    // 따라서 해당 정점 좌표를 Proj Space까지 변형해야 한다.
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