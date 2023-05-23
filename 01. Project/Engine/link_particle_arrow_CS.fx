#ifndef _LINK_PARTICLE_ARROW
#define _LINK_PARTICLE_ARROW

#include "register.fx"
#include "func.fx"
#include "struct.fx"

// u레지스터에 구조화 버퍼를 바인딩 걸 경우 선언을 다음과 같이 RWStructuredBuffer로 한다.
RWStructuredBuffer<tParticle> ParticleBuffer : register(u0);
RWStructuredBuffer<tParticleShare> ParticleShare : register(u1);
#define PARTICLE_MAX g_int_0
#define Particle ParticleBuffer[_id.x]
// 우리가 만든 ParticleShare는 파티클 시스템 전체당 하나이기 때문에 0번 인덱스만 존재한다.
#define ShareData           ParticleShare[0]

#define FirstEntry          g_float_1
#define SpawnRange          g_float_0
#define IsWorldSpawn        g_int_1
#define Is3DParticle        g_int_2
#define ObjectWorldPos      g_vec4_0
#define MinSpeed            g_vec2_0.x
#define MaxSpeed            g_vec2_0.y
#define MinLife             g_vec2_1.x
#define MaxLife             g_vec2_1.y
#define WorldMatrix         g_mat_0
#define Option              g_int_3

#define PI                  3.141592f

// OPTION
#define GravityForce        100
#define DEFAULT             0
#define GRAVITY_ON          1
#define SPREAD_XY           2


// 1024개가 아닌 128개만 지정한 이유는 우리가 파티클을 1024개나 만들 경우가 없다고 판단했기 때문이다.
// 만약 더 많은 갯수가 필요하다면, 그룹을 추가로 할당 받으면 된다.
// 이렇게 단위를 적게 만든 이유는 1024로 배정해두었다가 1개가 부족해서 그룹을 하나 더 할당 받을 떄,
// 사용하지 않는 1023개가 비기 떄문에 이 수를 줄이기 위해 단위를 낮게 설정한 것이다.
[numthreads(128, 1, 1)]
void CS_ParticleArrow(uint3 _id : SV_DispatchThreadID)
{
    // 지금 스레드를 1차원으로 사용중이기 때문에 x만 증가한다.
    if ((uint) PARTICLE_MAX <= _id.x)
        return;
    
    if (FirstEntry)
    {
        Particle.iActive = 0;
    }
    
    if (0 == Particle.iActive)
    {
        while (0 < ShareData.iAliveCount)
        {
            int iOriginValue = ShareData.iAliveCount;
            int iExchange = iOriginValue - 1;

            InterlockedCompareExchange(ShareData.iAliveCount, iOriginValue, iExchange, iExchange);
            
            if (iOriginValue == iExchange)
            {
                Particle.iActive = 1;
                break;
            }
        }
        
        if (Particle.iActive)
        {
            float4 vRandom = (float4) 0.f;
            

            float2 vUV = float2((float) _id.x / PARTICLE_MAX, 0.5f);
            vUV.x += g_fAccTime;

            vUV.y += sin((vUV.x + g_fAccTime) * 3.141592f * 2.f * 10.f) * 0.5;

            vRandom = float4(GaussianSample(vUV + float2(0.f, 0.f)).x, GaussianSample(vUV + float2(0.1f, 0.f)).x, GaussianSample(vUV + float2(0.2f, 0.f)).x, GaussianSample(vUV + float2(0.3f, 0.f)).x);
            
            vRandom.x = 87.l * vRandom.x;
            vRandom.x = frac(vRandom.x);
            
            vRandom.y = 87.l * vRandom.y;
            vRandom.y = frac(vRandom.y);
            
            vRandom.z = 87.l * vRandom.z;
            vRandom.z = frac(vRandom.z);

            // 사각형 범위로 생성
            //Particle.vRelativePos.xyz = vRandom.xyz * 1000.f - 500.f;
            //Particle.vRelativePos.z = 100.f;
            
            float fTheta = vRandom.x * 3.141592f * 2.f;

            // Particle.vRelativePos.xyz = float3(float2(cos(fTheta), sin(fTheta)) * vRandom.y, vRandom.z * 2.f - 1.f) * SpawnRange;
            //Particle.vRelativePos.z = abs(vRandom.z - 0.5) / (vRandom.z - 0.5) * length(Particle.vRelativePos.xy) * tan(PI / 12.f);
            
            Particle.vRelativePos.xyz = (float3) 0.f;

            if (IsWorldSpawn)
            {
                if (Is3DParticle == 0)
                {
                    Particle.vRelativePos.xyz += ObjectWorldPos.xyz;
                }
                else
                {
                    Particle.matWorld = WorldMatrix;
                }

            }
            
            Particle.fCurTime = 0.f;
            Particle.fMaxTime = MaxLife;
        }
    }
    else if (Particle.iActive == 1)
    {
        Particle.fCurTime += g_fDT;
        if (Particle.fMaxTime < Particle.fCurTime)
        {
            tParticle reset = (tParticle) 0.f;
            Particle = reset;
            Particle.iActive = 2;
        }
        else
        {

        }
    }
}


#endif