#ifndef _REGISTER
#define _REGISTER

#include "struct.fx"

// 상수 레지스터
cbuffer TRANSFORM : register(b0)
{
    row_major matrix g_matWorld;
    row_major matrix g_matView;
    row_major matrix g_matProj;
    
    row_major matrix g_matWorldInv;
    row_major matrix g_matViewInv;

    row_major matrix g_matWV;
    row_major matrix g_matWVP;
};

cbuffer MATERIAL : register(b1)
{
    float4 g_vDiff;
    float4 g_vSpec;
    float4 g_vAmb;
    float4 g_vEmv;
    
    int g_int_0;
    int g_int_1;
    int g_int_2;
    int g_int_3;

    float g_float_0;
    float g_float_1;
    float g_float_2;
    float g_float_3;

    float2 g_vec2_0;
    float2 g_vec2_1;
    float2 g_vec2_2;
    float2 g_vec2_3;

    float4 g_vec4_0;
    float4 g_vec4_1;
    float4 g_vec4_2;
    float4 g_vec4_3;

    row_major matrix g_mat_0;
    row_major matrix g_mat_1;
    row_major matrix g_mat_2;
    row_major matrix g_mat_3;
    
    int g_btex_0;
    int g_btex_1;
    int g_btex_2;
    int g_btex_3;
    int g_btex_4;
    int g_btex_5;
    int g_btex_6;
    int g_btex_7;
    
    int g_btexcube_0;
    int g_btexcube_1;
    
    int g_btexarr_0;
    int g_btexarr_1;
    
    // 3D Animation 정보
    int g_iAnim;
    int g_iBoneCount;
    int2 padding;
};

cbuffer GLOBAL : register(b2)
{
    float2  g_vRenderResolution;
    float2  g_vNoiseResolution;
    int     g_iLight2DCount;
    int     g_iLight3DCount;

    float   g_fAccTime;
    float   g_fDT;
}

cbuffer ANIM2DINFO : register(b3)
{
    float2 g_vLeftTop;		// 샘플링할 위치 : 우리는 아틀라스 이미지를 바인딩하기 때문에 어떤 위치를 잘라야하는지 알아야 한다.
    float2 g_vSlice;		// 받은 좌상단을 기점으로 얼마나 자를지 Slice 정보
    float2 g_vOffset;		// 중심이 안맞을 경우 offset 해줄 값
    float2 g_vFullSize;		// 전체 크기 : 

    int	   g_iAnim2DUse;	// 지금 에니메이션2D가 사용중인지 확인하기 위한 값. 0일 경우 사용하지 않는 중이다.
    int3   ipadding;	// 16바이트 단위로 맞추기 위한 공간할당 (패딩)
}


// 테스쳐 레지스터
Texture2D g_tex_0 : register(t0);
Texture2D g_tex_1 : register(t1);
Texture2D g_tex_2 : register(t2);
Texture2D g_tex_3 : register(t3);
Texture2D g_tex_4 : register(t4);
Texture2D g_tex_5 : register(t5);
Texture2D g_tex_6 : register(t6);
Texture2D g_tex_7 : register(t7);

TextureCube g_texcube_0 : register(t8);
TextureCube g_texcube_1 : register(t9);

Texture2DArray g_texarr_0 : register(t10);
Texture2DArray g_texarr_1 : register(t11);

// Animation2D Atlas
Texture2D g_Atlas : register(t12);

// LightBuffer
// 구조화 버퍼용 레지스터 선언은 StructuredBuffer<>로 선언을 해주어야 한다.
// 일반적인 텍스처 레지스터 선언은 Texture2D를 사용해 선언한다.
StructuredBuffer<tLightInfo> g_Light2DBuffer : register(t13);
StructuredBuffer<tLightInfo> g_Light3DBuffer : register(t14);

// Noise Texture
Texture2D g_Noise : register(t15);

// Animation3D Bone Matrix Buffer
StructuredBuffer<Matrix> g_arrBoneMat : register(t30);

// RenderTarget CopyTex
Texture2D g_RTCopyTex : register(t60);
Texture2D g_PTCopyTex : register(t61);

// 샘플러 레지스터
SamplerState g_sam_0 : register(s0); // Anisotropic Sampler
SamplerState g_sam_1 : register(s1); // Point Sampler

#endif