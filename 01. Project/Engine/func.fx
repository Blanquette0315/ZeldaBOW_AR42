#ifndef _FUNC
#define _FUNC

#include "struct.fx"
#include "register.fx"

// ù��° ������ inout�� �����ͼ� ������ ������ ���۷����� ������ �����̴�.
// out�� �������⸸ �� �� ������ ���� const ���۷����� ������ ����
// https://learn.microsoft.com/ko-kr/windows/win32/direct3dhlsl/dx-graphics-hlsl-function-parameters ����
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
		// distance()�� �� �� ������ �Ÿ�(��Į�� ��)�� �����ش�.
		// �츮�� �׸��� ������ ��� ���� z���� ����Ѵ�. ������ ������ z�� �������� ����� �Ϸ��� �ſ� �����ϱ� ������
		// 2D ȯ�濡���� �켱 x,y�θ� ����� ������ 2D�� �����ߴ�.
		float fDistance = distance(_vWorldPos.xy, g_Light2DBuffer[iLightIdx].vWorldPos.xy);

		if (fDistance < g_Light2DBuffer[iLightIdx].fRadius)
		{
			// �������� �Ÿ��� ���� ���� ������� ȿ���� �����Ѵ�.
			float fRatio = 1.f - fDistance / g_Light2DBuffer[iLightIdx].fRadius;
			_LightColor.vDiff += g_Light2DBuffer[iLightIdx].color.vDiff * fRatio;
		}
	}

	// Spot Light
	// ����� ���� ������ �ɾ��ָ� �ȴ�.
	else if (2 == g_Light2DBuffer[iLightIdx].iLightType)
    {
		// �ش� �ȼ��� ������ �������� �Ÿ� ���ϱ�
        float fDistance = distance(_vWorldPos.xy, g_Light2DBuffer[iLightIdx].vWorldPos.xy);
		// �Ÿ� üũ
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
			// ���� üũ
            if ((deg < rangedeg1 && deg > rangeDirdeg)
				|| (deg < rangeDirdeg && deg > rangedeg2))
            {
				// ������ ����� �� ���� ���� �������.
                float fRatio = 1.f - fDistance / g_Light2DBuffer[iLightIdx].fRadius;
                _LightColor.vDiff += g_Light2DBuffer[iLightIdx].color.vDiff * fRatio;
            }
        }
    }
}


void CalcLight3D(float3 _vViewPos, float3 _vViewNormal, int _iLigntIdx, inout tLightColor _LightColor)
{
    // ������ ������ ���� ��ǥ�迡�� �� ��ǥ��� ��ȯ
    tLightInfo Lightinfo = g_Light3DBuffer[_iLigntIdx];
    
    // ����� ������ �ʱ�ȭ
    float3 ViewLightDir = (float3) 0.f;
    float fDiffPow = (float) 0.f;
    float fSpecPow = (float) 0.f;
    
    // Directional Light
    if(Lightinfo.iLightType == 0)
    {
        float3 ViewLightDir = normalize(mul(float4(Lightinfo.vWorldDir.xyz, 0.f), g_matView)).xyz;
    
        // View �����̽� �󿡼� ǥ���� ���� ���⸦ ����
        fDiffPow = saturate(dot(-ViewLightDir, _vViewNormal));
    
        // �ݻ籤 ���⸦ ����
        // ǥ���� ���� �ݻ纤��
        float3 vViewReflect = normalize(ViewLightDir + 2.f * dot(-ViewLightDir, _vViewNormal) * _vViewNormal);
        
        // �������� ǥ���� ���ϴ� ����
        float3 vEye = normalize(_vViewPos);
        
        // �ü� ���Ϳ� �ݻ纤�͸� �����ؼ� �ݻ籤�� ���⸦ ����
        fSpecPow = saturate(dot(-vEye, vViewReflect));
        fSpecPow = pow(fSpecPow, 20);
    }
    
    // Point Light
    // �������� ������ ��ġ�� �߿��ϴ�.
    else if(Lightinfo.iLightType == 1)
    {
        // VeiwSpace �󿡼� ������ ��ġ�� ã�´�.
        float3 vLightViewPos = mul(float4(Lightinfo.vWorldPos.xyz, 1.f), g_matView).xyz;
        
        // ������ ��ġ���� ��ü�� ���ϴ� ���͸� ���Ѵ�.
        // ���ʹ� xyz �������� ��Ÿ���� ������ �������� ���� ���� ������ ���� �� �ִ�.
        ViewLightDir = _vViewPos - vLightViewPos;
        
        // �������� ��ü������ �Ÿ��� ���Ѵ�.
        float fDist = length(ViewLightDir);
        
        // �������� ��ü�� ���ϴ� ���� ���͸� ���Ѵ�.
        ViewLightDir = normalize(ViewLightDir);
        
        // �ݰ��� �Ÿ��� ���� ���� ���� ����
        // �������� ������ ������, �����ϴ°� �Ȱ���,
        //float fRatio = saturate(1.f - (fDist / Lightinfo.fRadius));
        // �ڻ������� ������ ������, �ܰ����� �� ���� �ް��� ���Ⱑ �����Ѵ�.
        float fRatio = cos(saturate(fDist / Lightinfo.fRadius) * 3.1415926535f * 0.5f);
        
        // View Space �󿡼��� ��ü�� ���� ���⸦ ���Ѵ�.
        fDiffPow = saturate(dot(-ViewLightDir, _vViewNormal)) * fRatio;
        
        // �ݻ籤 ���⸦ ����
        // ��ü ǥ���� ���� �ݻ� ����
        float3 vViewReflect = normalize(ViewLightDir + 2.f * dot(-ViewLightDir, _vViewNormal) * _vViewNormal);
        
        // �������� ��ü ǥ���� ���ϴ� ����
        float3 vEye = normalize(_vViewPos);
        
        // �ü� ���Ϳ� �ݻ纤�͸� �����ؼ� �ݻ籤�� ���⸦ ����
        fSpecPow = saturate(dot(-vEye, vViewReflect));
        fSpecPow = pow(fSpecPow, 20);
    }
    
    // Spot Light
    else if (Lightinfo.iLightType == 2)
    {
        // ����Ʈ����Ʈ ���� ���� ���ϱ�
        float3 ViewLightDir = normalize(mul(float4(Lightinfo.vWorldDir.xyz, 0.f), g_matView)).xyz;
        
        // VeiwSpace �󿡼� ������ ��ġ�� ã�´�.
        float3 vLightViewPos = mul(float4(Lightinfo.vWorldPos.xyz, 1.f), g_matView).xyz;
        
        // ���� �������� ���� ��ü ���������� ���� ���ϱ�
        float3 ViewVToLDir = _vViewPos - vLightViewPos;
        
        // �������� ��ü������ �Ÿ��� ���Ѵ�.
        float fDist = length(ViewVToLDir);
        
        // �������� ��ü�� ���ϴ� ���� ���͸� ���Ѵ�.
        ViewVToLDir = normalize(ViewVToLDir);
        
        // �ݰ��� �Ÿ��� ���� ���� ���� ����
        float fRatio = saturate(1.f - (fDist / Lightinfo.fRadius));
        
        
        // ����Ʈ����Ʈ �ڻ��ΰ�
        float fSpotCos = saturate(dot(ViewLightDir, ViewVToLDir));
        
        // ���� ���谪
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
        
        // View Space �󿡼��� ��ü�� ���� ���⸦ ���Ѵ�.
        fDiffPow = saturate(dot(-ViewVToLDir, _vViewNormal)) * fRatio * fConRatio;
        
        
        // �ݻ籤 ���⸦ ����
        // ��ü ǥ���� ���� �ݻ� ����
        float3 vViewReflect = normalize(ViewVToLDir + 2.f * dot(-ViewVToLDir, _vViewNormal) * _vViewNormal);
        
        // �������� ��ü ǥ���� ���ϴ� ����
        float3 vEye = normalize(_vViewPos);
        
        // �ü� ���Ϳ� �ݻ纤�͸� �����ؼ� �ݻ籤�� ���⸦ ����
        fSpecPow = saturate(dot(-vEye, vViewReflect));
        fSpecPow = pow(fSpecPow, 20);
    }
    
    // ���� ���ݻ籤
    _LightColor.vDiff += Lightinfo.color.vDiff * fDiffPow;
    
    // ���� �ݻ籤
    _LightColor.vSpec += Lightinfo.color.vSpec * fSpecPow;
    
    // ���� ȯ�汤
    _LightColor.vEmb += Lightinfo.color.vEmb;
}


// ComputeShader���� ����� Sampler ����
// CS������ PS���� ����ϴ� Texture.Sample(Sampler, UV)�� ����� �� ����.
// SampleLevel()�� ����� ���������� �츮�� ���ϴ� ����þ� ���͸� ����ϴ� ���÷��� ������ �� ���� ������ ���� �����Ѵ�.

// ����þ� ���� ����ü
// HLSL���� static Ű���带 ���̸�, �������� ����� ����������. (��, CS�ʿ����� ����� ����������.)
// ����þ� ���ʹ� ������ ������ ����ũ��, �־��� ���� ������ �۾����� �����̴�. (�ش� ���� ����� ������ 1�̴�.)
// �ش� ���͸� ����ϴ� ���÷��� ����� ���� ���� �Լ��� ���� �����ϴ� ���̴�.
static float GaussianFilter[5][5] =
{
    0.003f, 0.0133f, 0.0219f, 0.0133f, 0.003f,
    0.0133f, 0.0596f, 0.0983f, 0.0596f, 0.0133f,
    0.0219f, 0.0983f, 0.1621f, 0.0983f, 0.0219f,
    0.0133f, 0.0596f, 0.0983f, 0.0596f, 0.0133f,
    0.003f, 0.0133f, 0.0219f, 0.0133f, 0.003f,
};

// ����þ� ���ø� �Լ�
// ���� ���� ���÷��� WRAP�� ��� ���� ������ ���־�� �Ѵ�.
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
    
    // �ش� UV���� �´� �ؽ�ó �ȼ� ��ǥ ���
    int2 iUV = _UV * g_vNoiseResolution;
    // �츮�� ���� UV�� �߽����̱� ������ ����þ� ������ 0��° �ε����� �����ϰ� �ٲ��ش�.
    iUV -= int2(2, 2);
    // ���� ����þ� ������ ��Ŀ� �°� �ݺ����� ������ ���ø��� �����Ѵ�.
    for (int j = 0; j < 5; j++)
    {
        for (int i = 0; i < 5; i++)
        {
            int2 idx = int2(iUV.y + i, iUV.x + j);
            // ������ �ؽ�ó�� �����ؼ� �˸��� ����þ� ������ ���� ���� �������ش�.
            // HLSL���� �ؽ�ó�� �ȼ��� ���� �����Ϸ��� [int][int]�� �ƴ϶� [int2]�� ������ �ؾ��Ѵ�.
            vOutColor += g_Noise[idx] * GaussianFilter[j][i];
        }
    }
    
    // ���ø��� ���� ����� ��ȯ���ش�.
    return vOutColor;
}

// 0 ~ 1 ������ ���� ��ȯ
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