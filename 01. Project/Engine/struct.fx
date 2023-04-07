#ifndef _STRUCT
#define _STRUCT

struct tLightColor
{
	float4 vDiff;	// ���ݻ� ��
	float4 vSpec;	// �ݻ籤 ���
	float4 vEmb;	// ȯ�汤
};

// LightInfo
struct tLightInfo
{
	tLightColor	color;
	float4		vWorldPos;	// ���� ���� ��ġ
	float4		vWorldDir;	// ������ ���� ����
	int			iLightType;	// ���� Ÿ��
	float		fRadius;	// ���� �ݰ�
	float		fInAngle;	// ���� �� ����
    float		fOutAngle;	// ���� �۰� ����
};

// Particle
struct tParticle
{
    float4	vRelativePos;
    float4	vDir;
	
    float	fMaxTime;
    float	fCurTime;
    float	fSpeed;
    uint	iActive;
};

struct tParticleShare
{
    uint iAliveCount;
    uint3 iPadding;
};

struct tRaycastOut
{
    float2 vUV;
    int iDist;
    int success;
};

struct tSkinningInfo
{
    float3 vPos;
    float3 vTangent;
    float3 vBinormal;
    float3 vNormal;
};

#endif