#ifndef _STRUCT
#define _STRUCT

struct tLightColor
{
	float4 vDiff;	// ³­¹Ý»ç ±¤
	float4 vSpec;	// ¹Ý»ç±¤ °è¼ö
	float4 vEmb;	// È¯°æ±¤
};

// LightInfo
struct tLightInfo
{
	tLightColor	color;
	float4		vWorldPos;	// ±¤¿ø ¿ùµå À§Ä¡
	float4		vWorldDir;	// ±¤¿øÀÇ ¿ùµå ¹æÇâ
	int			iLightType;	// ±¤¿ø Å¸ÀÔ
	float		fRadius;	// ±¤¿ø ¹Ý°æ
	float		fInAngle;	// ±¤¿ø ¾È °¢µµ
    float		fOutAngle;	// ±¤¿ø ¹Û°° °¢µµ
};

// Particle
struct tParticle
{
    float4	vRelativePos;
    float4	vDir;
    float4x4 matWorld;
    
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