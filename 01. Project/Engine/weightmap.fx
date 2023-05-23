#ifndef _WEIGHTMAP
#define _WEIGHTMAP

#include "register.fx"
#include "struct.fx"

struct tWeight_4
{
    float arrWeight0[4];
    float arrWeight1[4];
    float arrWeight2[4];
};

RWStructuredBuffer<tWeight_4> WEIGHT_MAP : register(u0); // unordered acess
StructuredBuffer<tRaycastOut> LOCATION : register(t16); // �귯�� ��ġ(�»�� ����)

#define WIDTH       g_int_0
#define HEIGHT      g_int_1

//#define BRUSH_TEX   g_texarr_0
#define BRUSH_TEX   g_tex_0
#define SCALE       g_vec2_0   // �귯�� ũ��
#define BRUSH_IDX   g_int_2    // �귯�� �ε���

#define WEIGHT_IDX  g_int_3    // �ø� ����ġ ��ġ

[numthreads(32, 32, 1)]
void CS_WeightMap(int3 _iThreadID : SV_DispatchThreadID)
{
    if (WIDTH <= _iThreadID.x || HEIGHT <= _iThreadID.y || !LOCATION[0].success)
    {
        return;
    }

    int2 vCenterPos = float2(WIDTH, HEIGHT) * LOCATION[0].vUV;
    int2 vScale = float2(WIDTH, HEIGHT) * SCALE;

    if (_iThreadID.x < vCenterPos.x - (vScale.x / 2) || vCenterPos.x + (vScale.x / 2) < _iThreadID.x
        || _iThreadID.y < vCenterPos.y - (vScale.y / 2) || vCenterPos.y + (vScale.y / 2) < _iThreadID.y)
    {
        return;
    }

    // brush texture �� ���� ����
    int2 vLTPos = vCenterPos - (vScale / 2);
    float2 vUV = float2(_iThreadID.xy - vLTPos) / float2(vScale);

    //float4 vBrushColor = BRUSH_TEX.SampleLevel(g_sam_0, float3(vUV, BRUSH_IDX), 0);
    float4 vBrushColor = BRUSH_TEX.SampleLevel(g_sam_0, vUV, 0);

    // 2���� �ε��� ��ǥ�� 1���� �ε����� ���
    uint iIdx = (_iThreadID.y * WIDTH) + _iThreadID.x;
   

    // ���� ����ġ
    float arrWeight0[4] = WEIGHT_MAP[iIdx].arrWeight0;
    float arrWeight1[4] = WEIGHT_MAP[iIdx].arrWeight1;
    float arrWeight2[4] = WEIGHT_MAP[iIdx].arrWeight2;

    // ���� ��
    float fAdd = g_fDT * vBrushColor.a * 2.f;

    // ����ġ�� �ø���, ������ ������ ������ ���߾� ������.
    if (WEIGHT_IDX >= 0 && WEIGHT_IDX < 4)
    {
        arrWeight0[WEIGHT_IDX] = saturate(arrWeight0[WEIGHT_IDX] + fAdd);
    }
    else if (WEIGHT_IDX >= 4 && WEIGHT_IDX < 8)
    {
        arrWeight1[WEIGHT_IDX - 4] = saturate(arrWeight1[WEIGHT_IDX - 4] + fAdd);
    }
    else if (WEIGHT_IDX >= 8 && WEIGHT_IDX < 12)
    {
        arrWeight1[WEIGHT_IDX - 8] = saturate(arrWeight1[WEIGHT_IDX - 8] + fAdd);
    }

    // �ʰ���
    float fTotal = 0.f;
    for (int i = 0; i < 4; ++i)
    {
        fTotal += arrWeight0[i];
        fTotal += arrWeight1[i];
        fTotal += arrWeight2[i];
    }

    // �� ���� 1�� �Ѿ�� ���
    if (1.f < fTotal)
    {
        float fOver = fTotal - 1.f;

        // ������ ����ġ�� ����
        if (WEIGHT_IDX >= 0 && WEIGHT_IDX < 4)
        {
            fTotal = fTotal - arrWeight0[WEIGHT_IDX];
        }
        else if (WEIGHT_IDX >= 4 && WEIGHT_IDX < 8)
        {
            fTotal = fTotal - arrWeight1[WEIGHT_IDX - 4];
        }
        else if (WEIGHT_IDX >= 8 && WEIGHT_IDX < 12)
        {
            fTotal = fTotal - arrWeight2[WEIGHT_IDX - 8];
        }

        for (int j = 0; j < 12; ++j)
        {
            if (j == WEIGHT_IDX)
                continue;

            // �ʰ����� ������ ���߾� �����, �ٸ� ����ġ �ʿ��� ����.
            if (j >= 0 && j < 4)
            {
                arrWeight0[j] = saturate(arrWeight0[j] - fOver * (arrWeight0[j] / fTotal));
            }
            else if (j >= 4 && j < 8)
            {
                arrWeight1[j - 4] = saturate(arrWeight1[j - 4] - fOver * (arrWeight1[j - 4] / fTotal));
            }
            else if (j >= 8 && j < 12)
            {
                arrWeight2[j - 8] = saturate(arrWeight2[j - 8] - fOver * (arrWeight2[j - 8] / fTotal));
            }
        }
    }

    // ���ۿ� ���
    WEIGHT_MAP[iIdx].arrWeight0 = arrWeight0;
    WEIGHT_MAP[iIdx].arrWeight1 = arrWeight1;
    WEIGHT_MAP[iIdx].arrWeight2 = arrWeight2;
}





#endif