#ifndef _RAYMAP
#define _RAYMAP

#include "register.fx"

RWTexture2D<float> RAY_MAP : register(u0); // unordered acess
StructuredBuffer<tRaycastOut> LOCATION : register(t16); // 브러쉬 위치(좌상단 기준)

#define WIDTH       g_int_0
#define HEIGHT      g_int_1

//#define BRUSH_TEX   g_texarr_0
#define BRUSH_TEX   g_tex_0
#define SCALE       g_vec2_0   // 브러쉬 크기
#define BRUSH_IDX   g_int_2    // 브러쉬 인덱스
#define BRUSH_SHOW  g_int_3    // 브러시 영역 표시 체크

[numthreads(32, 32, 1)]
void CS_RayMap(int3 _iThreadID : SV_DispatchThreadID)
{
    if (BRUSH_SHOW != 0)
    {
        RAY_MAP[_iThreadID.xy] = 0.f;
        return;
    }
    
    if (WIDTH <= _iThreadID.x || HEIGHT <= _iThreadID.y || !LOCATION[0].success)
    {
        return;
    }

    int2 vCenterPos = float2(WIDTH, HEIGHT) * LOCATION[0].vUV;
    int2 vScale = float2(WIDTH, HEIGHT) * SCALE;
    
    if (_iThreadID.x < vCenterPos.x - (vScale.x / 2) || vCenterPos.x + (vScale.x / 2) < _iThreadID.x
        || _iThreadID.y < vCenterPos.y - (vScale.y / 2) || vCenterPos.y + (vScale.y / 2) < _iThreadID.y)
    {
        RAY_MAP[_iThreadID.xy] = 0.f;
        return;
    }
    
    // brush texture 에서 샘플링 할 UV 계산
    int2 vLTPos = vCenterPos - (vScale / 2);
    float2 vUV = float2(_iThreadID.xy - vLTPos) / float2(vScale);

    // 브러시 영역 색 지정
    float4 vBrushColor = BRUSH_TEX.SampleLevel(g_sam_0, vUV, 0);
    RAY_MAP[_iThreadID.xy] = vBrushColor.a;
}


#endif