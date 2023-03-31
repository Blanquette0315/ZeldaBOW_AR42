#ifndef _FUNC
#define _FUNC

#include "struct.fx"
#include "register.fx"

// 첫번째 인자의 inout은 가져와서 수정이 가능한 레퍼런스와 유사한 느낌이다.
// out은 가져오기만 할 뿐 쓸수는 없는 const 레퍼런스와 유사한 느낌
// https://learn.microsoft.com/ko-kr/windows/win32/direct3dhlsl/dx-graphics-hlsl-function-parameters 참고
void CalcLight2D(inout tLightColor _LightColor, float3 _vWorldPos, int iLightIdx)
{
	// Directinal Light
	if (0 == g_Light2DBuffer[iLightIdx].iLightType)
	{
		_LightColor.vDiff += g_Light2DBuffer[iLightIdx].color.vDiff;
	}

	// Point Light
	else if (1 == g_Light2DBuffer[iLightIdx].iLightType)
	{
		// distance()는 두 점 사이의 거리(스칼라 값)을 구해준다.
		// 우리는 그리는 순서를 잡기 위해 z값을 사용한다. 하지만 광원의 z축 영역까지 계산을 하려면 매우 복잡하기 때문에
		// 2D 환경에서는 우선 x,y로만 계산해 광원을 2D로 적용했다.
		float fDistance = distance(_vWorldPos.xy, g_Light2DBuffer[iLightIdx].vWorldPos.xy);

		if (fDistance < g_Light2DBuffer[iLightIdx].fRadius)
		{
			// 광원과의 거리를 구해 점점 밝아지는 효과를 적용한다.
			float fRatio = 1.f - fDistance / g_Light2DBuffer[iLightIdx].fRadius;
			_LightColor.vDiff += g_Light2DBuffer[iLightIdx].color.vDiff * fRatio;
		}
	}

	// Spot Light
	// 방향과 각도 제한을 걸어주면 된다.
	else if (2 == g_Light2DBuffer[iLightIdx].iLightType)
    {
		// 해당 픽셀의 지점과 광원과의 거리 구하기
        float fDistance = distance(_vWorldPos.xy, g_Light2DBuffer[iLightIdx].vWorldPos.xy);
		// 거리 체크
        if (fDistance < g_Light2DBuffer[iLightIdx].fRadius)
        {
            float x = _vWorldPos.x - g_Light2DBuffer[iLightIdx].vWorldPos.x;
            float y = _vWorldPos.y - g_Light2DBuffer[iLightIdx].vWorldPos.y;
			
            float radian = atan2(y, x);
            float deg = degrees(radian);
			
            float rangeDirdeg = degrees(atan2(g_Light2DBuffer[iLightIdx].vWorldDir.y, g_Light2DBuffer[iLightIdx].vWorldDir.x));
			
            //float rangeDirdeg = degrees(acos(g_Light2DBuffer[iLightIdx].vWorldDir.x));
            //float rangeDirdeg = degrees(acos(1));
            float rangedeg1 = rangeDirdeg + g_Light2DBuffer[iLightIdx].fInAngle / 2;
            float rangedeg2 = rangeDirdeg - g_Light2DBuffer[iLightIdx].fInAngle / 2;
			// 각도 체크
            if ((deg < rangedeg1 && deg > rangeDirdeg)
				|| (deg < rangeDirdeg && deg > rangedeg2))
            {
				// 원점에 가까워 질 수록 점점 밝아진다.
                float fRatio = 1.f - fDistance / g_Light2DBuffer[iLightIdx].fRadius;
                _LightColor.vDiff += g_Light2DBuffer[iLightIdx].color.vDiff * fRatio;
            }
        }
    }
}


void CalcLight3D(float3 _vViewPos, float3 _vViewNormal, int _iLigntIdx, inout tLightColor _LightColor)
{
    // 광원의 방향을 월드 좌표계에서 뷰 좌표계로 변환
    tLightInfo Lightinfo = g_Light3DBuffer[_iLigntIdx];
    
    // 사용할 변수들 초기화
    float3 ViewLightDir = (float3) 0.f;
    float fDiffPow = (float) 0.f;
    float fSpecPow = (float) 0.f;
    
    // Directional Light
    if(Lightinfo.iLightType == 0)
    {
        float3 ViewLightDir = normalize(mul(float4(Lightinfo.vWorldDir.xyz, 0.f), g_matView)).xyz;
    
        // View 스페이스 상에서 표면의 빛의 세기를 구함
        fDiffPow = saturate(dot(-ViewLightDir, _vViewNormal));
    
        // 반사광 세기를 구함
        // 표면의 빛의 반사벡터
        float3 vViewReflect = normalize(ViewLightDir + 2.f * dot(-ViewLightDir, _vViewNormal) * _vViewNormal);
        
        // 시점에서 표면을 향하는 벡터
        float3 vEye = normalize(_vViewPos);
        
        // 시선 벡터와 반사벡터를 내적해서 반사광의 세기를 구함
        fSpecPow = saturate(dot(-vEye, vViewReflect));
        fSpecPow = pow(fSpecPow, 20);
    }
    
    // Point Light
    // 점광원은 광원의 위치가 중요하다.
    else if(Lightinfo.iLightType == 1)
    {
        // VeiwSpace 상에서 광원의 위치를 찾는다.
        float3 vLightViewPos = mul(float4(Lightinfo.vWorldPos.xyz, 1.f), g_matView).xyz;
        
        // 광원의 위치에서 물체로 향하는 벡터를 구한다.
        // 벡터는 xyz 성분으로 나타내기 때문에 포지션을 빼면 방향 정보를 얻을 수 있다.
        ViewLightDir = _vViewPos - vLightViewPos;
        
        // 광원에서 물체까지의 거리를 구한다.
        float fDist = length(ViewLightDir);
        
        // 광원에서 물체를 향하는 단위 벡터를 구한다.
        ViewLightDir = normalize(ViewLightDir);
        
        // 반경대비 거리에 따른 빛의 세기 비율
        // 선형으로 비율을 잡으면, 감소하는게 똑같고,
        //float fRatio = saturate(1.f - (fDist / Lightinfo.fRadius));
        // 코사인으로 비율을 잡으면, 외곽으로 갈 수록 급격히 세기가 감소한다.
        float fRatio = cos(saturate(fDist / Lightinfo.fRadius) * 3.1415926535f * 0.5f);
        
        // View Space 상에서의 물체의 빛의 세기를 구한다.
        fDiffPow = saturate(dot(-ViewLightDir, _vViewNormal)) * fRatio;
        
        // 반사광 세기를 구함
        // 물체 표면의 빛의 반사 벡터
        float3 vViewReflect = normalize(ViewLightDir + 2.f * dot(-ViewLightDir, _vViewNormal) * _vViewNormal);
        
        // 시점에서 물체 표면을 향하는 벡터
        float3 vEye = normalize(_vViewPos);
        
        // 시선 벡터와 반사벡터를 내적해서 반사광의 세기를 구함
        fSpecPow = saturate(dot(-vEye, vViewReflect));
        fSpecPow = pow(fSpecPow, 20);
    }
    
    // Spot Light
    else if (Lightinfo.iLightType == 2)
    {
        // 스포트라이트 진행 방향 구하기
        float3 ViewLightDir = normalize(mul(float4(Lightinfo.vWorldDir.xyz, 0.f), g_matView)).xyz;
        
        // VeiwSpace 상에서 광원의 위치를 찾는다.
        float3 vLightViewPos = mul(float4(Lightinfo.vWorldPos.xyz, 1.f), g_matView).xyz;
        
        // 광원 지점에서 부터 물체 지점까지의 방향 구하기
        float3 ViewVToLDir = _vViewPos - vLightViewPos;
        
        // 광원에서 물체까지의 거리를 구한다.
        float fDist = length(ViewVToLDir);
        
        // 광원에서 물체를 향하는 단위 벡터를 구한다.
        ViewVToLDir = normalize(ViewVToLDir);
        
        // 반경대비 거리에 따른 빛의 세기 비율
        float fRatio = saturate(1.f - (fDist / Lightinfo.fRadius));
        
        
        // 스포트라이트 코사인값
        float fSpotCos = saturate(dot(ViewLightDir, ViewVToLDir));
        
        // 원뿔 감쇠값
        float fConRatio = 0.f;
        
        //if (fSpotCos > cos(radians(Lightinfo.fInAngle)))
        //{
        //    fConRatio = 1.f;
        //}
        //else
        //{
        //    fConRatio = saturate((fSpotCos - cos(radians(Lightinfo.fOutAngle * 0.5f))) / (cos(radians(Lightinfo.fInAngle * 0.5f)) - cos(radians(Lightinfo.fOutAngle * 0.5f))));
        //    //fConRatio = saturate(fSpotCos - cos(radians(Lightinfo.fOutAngle))) * cos(radians(Lightinfo.fInAngle));
        //}
        fConRatio = saturate((fSpotCos - cos(radians(Lightinfo.fOutAngle * 0.5f))) / (cos(radians(Lightinfo.fInAngle * 0.5f)) - cos(radians(Lightinfo.fOutAngle * 0.5f))));
        
        // View Space 상에서의 물체의 빛의 세기를 구한다.
        fDiffPow = saturate(dot(-ViewVToLDir, _vViewNormal)) * fRatio * fConRatio;
        
        
        // 반사광 세기를 구함
        // 물체 표면의 빛의 반사 벡터
        float3 vViewReflect = normalize(ViewVToLDir + 2.f * dot(-ViewVToLDir, _vViewNormal) * _vViewNormal);
        
        // 시점에서 물체 표면을 향하는 벡터
        float3 vEye = normalize(_vViewPos);
        
        // 시선 벡터와 반사벡터를 내적해서 반사광의 세기를 구함
        fSpecPow = saturate(dot(-vEye, vViewReflect));
        fSpecPow = pow(fSpecPow, 20);
    }
    
    // 최종 난반사광
    _LightColor.vDiff += Lightinfo.color.vDiff * fDiffPow;
    
    // 최종 반사광
    _LightColor.vSpec += Lightinfo.color.vSpec * fSpecPow;
    
    // 최종 환경광
    _LightColor.vEmb += Lightinfo.color.vEmb;
}


// ComputeShader에서 사용할 Sampler 구현
// CS에서는 PS에서 사용하던 Texture.Sample(Sampler, UV)를 사용할 수 없다.
// SampleLevel()은 사용이 가능하지만 우리가 원하는 가우시안 필터를 사용하는 샘플러를 지정할 수 없기 때문에 직접 구현한다.

// 가우시안 필터 구조체
// HLSL에서 static 키워드를 붙이면, 전역에서 사용이 가능해진다. (즉, CS쪽에서도 사용이 가능해진다.)
// 가우시안 필터는 중점의 비중이 제일크고, 멀어질 수록 비중이 작아지는 필터이다. (해당 필터 행렬의 총합은 1이다.)
// 해당 필터를 사용하는 샘플러를 만들기 위해 샘플 함수를 직접 구현하는 것이다.
static float GaussianFilter[5][5] =
{
    0.003f, 0.0133f, 0.0219f, 0.0133f, 0.003f,
    0.0133f, 0.0596f, 0.0983f, 0.0596f, 0.0133f,
    0.0219f, 0.0983f, 0.1621f, 0.0983f, 0.0219f,
    0.0133f, 0.0596f, 0.0983f, 0.0596f, 0.0133f,
    0.003f, 0.0133f, 0.0219f, 0.0133f, 0.003f,
};

// 가우시안 샘플링 함수
// 직접 만든 샘플러는 WRAP이 없어서 직접 구현을 해주어야 한다.
float4 GaussianSample(float2 _UV)
{
    float4 vOutColor = (float4) 0.f;
	
    if (1.f < _UV.x)
        _UV.x = frac(_UV.x);
    else if (_UV.x < 0.f)
        _UV.x = 1 + frac(_UV.x);

    if (1.f < _UV.y)
        _UV.y = frac(_UV.y);
    else if (_UV.y < 0.f)
        _UV.y = 1 + frac(_UV.y);
    
    // 해당 UV값에 맞는 텍스처 픽셀 좌표 계산
    int2 iUV = _UV * g_vNoiseResolution;
    // 우리가 얻은 UV는 중심점이기 때문에 가우시안 필터의 0번째 인덱스와 동일하게 바꿔준다.
    iUV -= int2(2, 2);
    // 이후 가우시안 필터의 행렬에 맞게 반복문을 수행해 샘플링을 수행한다.
    for (int j = 0; j < 5; j++)
    {
        for (int i = 0; i < 5; i++)
        {
            int2 idx = int2(iUV.y + i, iUV.x + j);
            // 노이즈 텍스처에 접근해서 알맞은 가우시안 필터의 값을 곱해 누적해준다.
            // HLSL에서 텍스처의 픽셀에 직접 접근하려면 [int][int]가 아니라 [int2]로 접근을 해야한다.
            vOutColor += g_Noise[idx] * GaussianFilter[j][i];
        }
    }
    
    // 샘플링이 끝난 결과를 반환해준다.
    return vOutColor;
}

// 0 ~ 1 사이의 값을 반환
float Random(int key)
{
    float fValue = 0.f;
	
    return fValue;
}


bool IsBinding(in Texture2D _tex)
{
    uint width, height;
    _tex.GetDimensions(width, height);
    
    if (width == 0 || height == 0)
        return false;
    
    return true;
}

float encode(float4 _value)
{
    uint rgba = (uint(_value.x * 255.f) << 24) + (uint(_value.y * 255.f) << 16) + (uint(_value.z * 255.f) << 8) + uint(_value.w * 255.f);
    return asfloat(rgba);
}

float4 decode(float _value)
{
    uint rgba = asint(_value);

    float r = (float) (rgba >> 24) / 255.f;
    float g = (float) ((rgba & 0x00ff0000) >> 16) / 255.f;
    float b = (float) ((rgba & 0x0000ff00) >> 8) / 255.f;
    float a = (float) (rgba & 0x000000ff) / 255.f;

    return float4(r, g, b, a);
}

float GetTessFactor(float3 _vPos, int _iMinLevel, int _iMaxLevel, float _MinDistance, float _MaxDistance)
{
    float fDistance = length(_vPos);
    
    if (_MaxDistance < fDistance)
        return _iMinLevel;
    if (fDistance < _MinDistance)
        return _iMaxLevel;
    
    float fLevel = _iMaxLevel - (_iMaxLevel - _iMinLevel) * ((fDistance - _MinDistance) / (_MaxDistance - _MinDistance));
    
    return fLevel;
}

int IntersectsLay(float3 _vertices[3], float3 _vStart, float3 _vDir, out float3 _vCrossPoint, out float _fResult)
{
    float3 edge[2] = { (float3) 0.f, (float3) 0.f };
    edge[0] = _vertices[1].xyz - _vertices[0].xyz;
    edge[1] = _vertices[2].xyz - _vertices[0].xyz;

    float3 normal = normalize(cross(edge[0], edge[1]));
    float b = dot(normal, _vDir);

    float3 w0 = _vStart - _vertices[0].xyz;
    float a = -dot(normal, w0);
    float t = a / b;

    _fResult = t;

    float3 p = _vStart + t * _vDir;

    _vCrossPoint = p;

    float uu, uv, vv, wu, wv, inverseD;
    uu = dot(edge[0], edge[0]);
    uv = dot(edge[0], edge[1]);
    vv = dot(edge[1], edge[1]);

    float3 w = p - _vertices[0].xyz;
    wu = dot(w, edge[0]);
    wv = dot(w, edge[1]);
    inverseD = uv * uv - uu * vv;
    inverseD = 1.0f / inverseD;

    float u = (uv * wv - vv * wu) * inverseD;
    if (u < 0.0f || u > 1.0f)
    {
        return 0;
    }

    float v = (uv * wu - uu * wv) * inverseD;
    if (v < 0.0f || (u + v) > 1.0f)
    {
        return 0;
    }

    return 1;
}

float ShadowGaussianSample(float2 _UV, int _int)
{
    if (1.f < _UV.x)
        _UV.x = frac(_UV.x);
    else if (_UV.x < 0.f)
        _UV.x = 1 + frac(_UV.x);

    if (1.f < _UV.y)
        _UV.y = frac(_UV.y);
    else if (_UV.y < 0.f)
        _UV.y = 1 + frac(_UV.y);
    
    int2 iUV = _UV * int2((int) g_vRenderResolution.x, (int) g_vRenderResolution.y);
    float fShadowPow = 0.f;
    
    for (int j = 0; j < 5; j++)
    {
        for (int i = 0; i < 5; i++)
        {
            _UV.x = (float) (iUV.x + (j - 2) * _int) / g_vRenderResolution.x;
            _UV.y = (float) (iUV.y + (i - 2) * _int) / g_vRenderResolution.y;
            
            fShadowPow += g_tex_2.Sample(g_sam_0, _UV).y * GaussianFilter[j][i];
        }
    }
    
    return fShadowPow;
}
#endif