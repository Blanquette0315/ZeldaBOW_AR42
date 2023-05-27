#ifndef _MONSTER_PARTICLE_DUST
#define _MONSTER_PARTICLE_DUST

#include "register.fx"
#include "func.fx"
#include "struct.fx"

// u�������Ϳ� ����ȭ ���۸� ���ε� �� ��� ������ ������ ���� RWStructuredBuffer�� �Ѵ�.
RWStructuredBuffer<tParticle> ParticleBuffer : register(u0);
RWStructuredBuffer<tParticleShare> ParticleShare : register(u1);
#define PARTICLE_MAX g_int_0
#define Particle ParticleBuffer[_id.x]
// �츮�� ���� ParticleShare�� ��ƼŬ �ý��� ��ü�� �ϳ��̱� ������ 0�� �ε����� �����Ѵ�.
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


// 1024���� �ƴ� 128���� ������ ������ �츮�� ��ƼŬ�� 1024���� ���� ��찡 ���ٰ� �Ǵ��߱� �����̴�.
// ���� �� ���� ������ �ʿ��ϴٸ�, �׷��� �߰��� �Ҵ� ������ �ȴ�.
// �̷��� ������ ���� ���� ������ 1024�� �����صξ��ٰ� 1���� �����ؼ� �׷��� �ϳ� �� �Ҵ� ���� ��,
// ������� �ʴ� 1023���� ��� ������ �� ���� ���̱� ���� ������ ���� ������ ���̴�.
[numthreads(128, 1, 1)]
void CS_ParticleSpark(uint3 _id : SV_DispatchThreadID)
{
    // ���� �����带 1�������� ������̱� ������ x�� �����Ѵ�.
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
            
            float fTheta = vRandom.x * 3.141592f * 2.f;


            float fSpawnRange = SpawnRange * (1 - vRandom.y * 0.5);
            
            Particle.vRelativePos.x = cos(fTheta) * vRandom.y * SpawnRange;
            Particle.vRelativePos.z = sin(fTheta) * vRandom.y * SpawnRange;
            
            Particle.vDir.xyz = normalize(float3(0.f, 1.f, 0.f));

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
            Particle.fSpeed = vRandom.z * (MaxSpeed - MinSpeed) + MinSpeed;
            Particle.vSpeed = Particle.vDir * Particle.fSpeed;
            
            Particle.fCurTime = 0.f;
            Particle.fMaxTime = vRandom.w * (MaxLife - MinLife) + MinLife;
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
            // �ڽ��� �ӵ��� DT�� ���� �̹� DT��ŭ �̵����ϴµ�, ������ ���ؼ� �ش� �������� �̵��ϰ� ����� �־���.
            float4 vPrevPos = Particle.vRelativePos;
            
            if (Option & GRAVITY_ON)
            {
                Particle.vSpeed.y -= GravityForce * g_fDT;
                Particle.vDir = normalize(Particle.vSpeed);
            }
            
            Particle.vRelativePos += Particle.vSpeed * g_fDT;
        }
    }
}
#endif