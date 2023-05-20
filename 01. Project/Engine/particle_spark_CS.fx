#ifndef _PARTICLE_SPARK
#define _PARTICLE_SPARK

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


#define SpawnRange          g_float_0
#define IsWorldSpawn        g_int_1
#define Is3DParticle        g_int_2
#define ObjectWorldPos      g_vec4_0
#define MinSpeed            g_vec2_0.x
#define MaxSpeed            g_vec2_0.y
#define MinLife             g_vec2_1.x
#define MaxLife             g_vec2_1.y
#define WorldMatrix         g_mat_0

// 1024���� �ƴ� 128���� ������ ������ �츮�� ��ƼŬ�� 1024���� ���� ��찡 ���ٰ� �Ǵ��߱� �����̴�.
// ���� �� ���� ������ �ʿ��ϴٸ�, �׷��� �߰��� �Ҵ� ������ �ȴ�.
// �̷��� ������ ���� ���� ������ 1024�� �����صξ��ٰ� 1���� �����ؼ� �׷��� �ϳ� �� �Ҵ� ���� ��,
// ������� �ʴ� 1023���� ��� ������ �� ���� ���̱� ���� ������ ���� ������ ���̴�.
[numthreads(128, 1, 1)]
void CS_ParticleUpdate(uint3 _id : SV_DispatchThreadID)
{
    // ���� �����带 1�������� ������̱� ������ x�� �����Ѵ�.
    if ((uint) PARTICLE_MAX <= _id.x)
        return;
    
    if (0 == Particle.iActive)
    {
        while (0 < ShareData.iAliveCount)
        {
            int iOriginValue = ShareData.iAliveCount;
            int iExchange = iOriginValue - 1;

            // ������ ����ȭ ó��
            // ���������� ��� �����尡 InterlockedExchange �Լ��� ȣ��������, ���� Exchange�Ǵ� �����尡 �����Ѵ�.
            // InterlockedExchange ����
            // ����1 : ������
            // ����2 : ������ - 1
            // ����3 : InterlockedExchange�� ����� �����ߴ��� Ȯ���� �� �� �ִ� ���� ���� ���´�.
            // ��, ����1�� ���´�.
            // �ش� �Լ��� ���������� ��� �����尡 ����1�� ���� ����2�� ����1 -1�� �־ ����Ѵ�.
            // ���� ���� ���� ������ ���� �����尡 ����3�� Ȯ���� ����1�� ��������� ����ϰ�, ���� ��������� ��ȯ�޴�
            // ����3�� ���� -1�� ���� ����ִ�.
            //���� ���ķ� �ش� �Լ��� ���������, ������ ���� ���� ����ǰ�, ���߿� ����Ǵ°� üũ�� ����������.
            
            //InterlockedExchange(ShareData.iAliveCount, iExchange, iExchange);

            // InterlockedCompareExchange()�� ����2�� ����1�� ���� ���� �ƴϸ� ����ó���� �ȴ�.
            InterlockedCompareExchange(ShareData.iAliveCount, iOriginValue, iExchange, iExchange);
            
            if (iOriginValue == iExchange)
            {
                Particle.iActive = 1;
                break;
            }
        }
        
        if (Particle.iActive)
        {
            // ���������� ��ġ�� ������ �����Ѵ�.
            // ���ø��� �õ��� UV�� ����Ѵ�.
            // �ش� Shader���� ��ƼŬ �ϳ� �� �����ϰ� �ٸ� ���� ������ ID�̴�. ���� ID���� �������� UV���� ����Ѵ�.
            float4 vRandom = (float4) 0.f;
            
            // �ʱ� UV.x�� ������ ���̵� ��ƼŬ �ִ� ������ ������ ���ϰ������� ������ ��ġ�ϵ��� ������־���.
            // �ʱ� UV.y�� 0.5�� �־��־���.
            float2 vUV = float2((float) _id.x / PARTICLE_MAX, 0.5f);
            vUV.x += g_fAccTime; // x�� �����ð��� ���� �ð��� �帧�� ���� ���������� UV���� �ٲ�� ���־���.
            // y�� x�� �����ð��� �߰��� ������ ���� �־� ���������� �ٲ�� ���ְ�, sin()�Լ��� �̿��ؼ�,
            // x���� ��ȭ�� ���� ���� �׷����� �׸����� ����� �־���.
            // �̶�, 2���̸� ���� UV 0~1���� ���� ���� �׷��� �ϳ��� �׷������� ���־���, 0.5�� ���� ������ �ٿ��־���.
            // 10�� ���� ���� �ش� ���α׷����� UV 0~1���̿� ��� �׷����� �������� �����ش�.
            vUV.y += sin((vUV.x + g_fAccTime) * 3.141592f * 2.f * 10.f) * 0.5;

            // ���� �츮�� ������� ������ �ؽ�ó�� 1���� ���� ���� ��ȯ�ϱ� ������ ���ڸ� ���ݾ� �ٲپ� 3���� ��ȯ �޴´�.
            // �׷��� x,y,z�� �������� ����.
            vRandom = float4(GaussianSample(vUV + float2(0.f, 0.f)).x, GaussianSample(vUV + float2(0.1f, 0.f)).x, GaussianSample(vUV + float2(0.2f, 0.f)).x, GaussianSample(vUV + float2(0.3f, 0.f)).x);
            
            vRandom.x = 87.l * vRandom.x;
            vRandom.x = frac(vRandom.x);
            
            vRandom.y = 87.l * vRandom.y;
            vRandom.y = frac(vRandom.y);
            
            vRandom.z = 87.l * vRandom.z;
            vRandom.z = frac(vRandom.z);

            // �簢�� ������ ����
            //Particle.vRelativePos.xyz = vRandom.xyz * 1000.f - 500.f;
            //Particle.vRelativePos.z = 100.f;
            
            // ���� ������ ����
            // �������� ���� ���� 0~1���� ���̱� ������ 2PI�� ���� 0~2PI ������ ������ ������ ���Ѵ�.
            float fTheta = vRandom.x * 3.141592f * 2.f;
            // ���� ���� �������� ��Ÿ�� ���� ��ǥ�� cos��Ÿ, sin��Ÿ�̴�.
            // ���� ��ǥ�� 0~1������ ���� ���� ����� ���� ������ ������ Ư�� ��ġ�� ����ش�.
            // ��> ��, cos��Ÿ, sin��Ÿ�� ������ ���� ���� ���� ������ ��ġ�� �ٲ��ش�.
            // ���� ������ ���� ���� ���� ��ǥ�� �ٲپ� �ش�.
            // Particle.vRelativePos.xyz = float3(float2(cos(fTheta), sin(fTheta)) * vRandom.y, vRandom.z * 2.f - 1.f) * SpawnRange;
            Particle.vRelativePos.xyz = float3(0.f, 0.f, 0.f);
            // Particle.vRelativePos.xyz = float3(1.f, 1.f, 0.f);
            
            // ��ƼŬ ����
            Particle.vDir.xyz = normalize(float3(vRandom.x - 0.5f, vRandom.y - 0.5f, vRandom.z - 0.5f));
            //Particle.vDir.xyz = float3(0.f, 1.f, 1.f);
            //Particle.vDir.xyz = normalize(Particle.vDir.xyz);
            
            // ��ƼŬ �ý��� ������Ʈ�� ������� ���󰡸� ����
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

            // ��ƼŬ �ӷ�
            Particle.fSpeed = vRandom.z * (MaxSpeed - MinSpeed) + MinSpeed;
            
            // ��ƼŬ LifeTime
            Particle.fCurTime = 0.f;
            Particle.fMaxTime = vRandom.w * (MaxLife - MinLife) + MinLife;
        }
    }
    
    // Ȱ��ȭ�� �� ��ƼŬ
    else
    {
        Particle.fCurTime += g_fDT;
        if (Particle.fMaxTime < Particle.fCurTime)
        {
            tParticle reset = (tParticle) 0.f;
            Particle = reset;
            Particle.iActive = 0;
        }
        else
        {
            // �ڽ��� �ӵ��� DT�� ���� �̹� DT��ŭ �̵����ϴµ�, ������ ���ؼ� �ش� �������� �̵��ϰ� ����� �־���.
            Particle.vRelativePos += Particle.vDir * Particle.fSpeed * g_fDT;
        }
    }
}


#endif