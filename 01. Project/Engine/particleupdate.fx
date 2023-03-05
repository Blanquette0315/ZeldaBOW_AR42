#ifndef _PARTICLEUPDATE
#define _PARTICLEUPDATE

#include "register.fx"
#include "func.fx"

// u레지스터에 구조화 버퍼를 바인딩 걸 경우 선언을 다음과 같이 RWStructuredBuffer로 한다.
RWStructuredBuffer<tParticle> ParticleBuffer : register(u0);
RWStructuredBuffer<tParticleShare> ParticleShare : register(u1);
#define PARTICLE_MAX g_int_0
#define Particle ParticleBuffer[_id.x]
// 우리가 만든 ParticleShare는 파티클 시스템 전체당 하나이기 때문에 0번 인덱스만 존재한다.
#define ShareData           ParticleShare[0]


#define SpawnRange          g_float_0
#define IsWorldSpawn        g_int_1
#define ObjectWorldPos      g_vec4_0
#define MinSpeed            g_vec2_0.x
#define MaxSpeed            g_vec2_0.y
#define MinLife             g_vec2_1.x
#define MaxLife             g_vec2_1.y

// 1024개가 아닌 128개만 지정한 이유는 우리가 파티클을 1024개나 만들 경우가 없다고 판단했기 때문이다.
// 만약 더 많은 갯수가 필요하다면, 그룹을 추가로 할당 받으면 된다.
// 이렇게 단위를 적게 만든 이유는 1024로 배정해두었다가 1개가 부족해서 그룹을 하나 더 할당 받을 떄,
// 사용하지 않는 1023개가 비기 떄문에 이 수를 줄이기 위해 단위를 낮게 설정한 것이다.
[numthreads(128,1,1)]
void CS_ParticleUpdate(uint3 _id : SV_DispatchThreadID)
{
    // 지금 스레드를 1차원으로 사용중이기 때문에 x만 증가한다.
    if ((uint) PARTICLE_MAX <= _id.x)
        return;
    
    if(0 == Particle.iActive)
    {
        while (0 < ShareData.iAliveCount)
        {
            int iOriginValue = ShareData.iAliveCount;
            int iExchange = iOriginValue - 1;

            // 스레드 동기화 처리
            // 병렬적으로 모든 스레드가 InterlockedExchange 함수를 호출하지만, 먼저 Exchange되는 스레드가 존재한다.
            // InterlockedExchange 인자
            // 인자1 : 목적지
            // 인자2 : 목적지 - 1
            // 인자3 : InterlockedExchange가 제대로 동작했는지 확인해 볼 수 있는 원래 값이 들어온다.
            // 즉, 인자1이 들어온다.
            // 해당 함수는 병렬적으로 모든 스레드가 인자1이 들어와 인자2에 인자1 -1을 넣어서 대기한다.
            // 그중 가장 먼저 수행이 끝난 스레드가 인자3을 확인해 인자1이 들어있으면 통과하고, 이후 스레드들은 반환받는
            // 인자3의 값이 -1된 값이 들어있다.
            //따라서 병렬로 해당 함수가 수행되지만, 스레드 별로 먼저 수행되고, 나중에 수행되는게 체크가 가능해진다.
            
            //InterlockedExchange(ShareData.iAliveCount, iExchange, iExchange);

            // InterlockedCompareExchange()는 인자2와 인자1을 비교해 참이 아니면 실패처리가 된다.
            InterlockedCompareExchange(ShareData.iAliveCount, iOriginValue, iExchange, iExchange);
            
            if (iOriginValue == iExchange)
            {
                Particle.iActive = 1;
                break;
            }
        }
        
        if(Particle.iActive)
        {
            // 랜덤값으로 위치와 방향을 설정한다.
            // 샘플링을 시도할 UV를 계산한다.
            // 해당 Shader에서 파티클 하나 당 유일하게 다른 것은 스레드 ID이다. 따라서 ID값을 기준으로 UV값을 계산한다.
            float4 vRandom = (float4) 0.f;
            
            // 초기 UV.x는 스레드 아이디를 파티클 최대 갯수로 나누어 동일간격으로 띄엄띄엄 위치하도록 만들어주었다.
            // 초기 UV.y는 0.5를 넣어주었다.
            float2 vUV = float2((float) _id.x / PARTICLE_MAX, 0.5f);
            vUV.x += g_fAccTime; // x에 누적시간을 더해 시간의 흐름에 따라 지속적으로 UV값이 바뀌도록 해주었다.
            // y는 x에 누적시간이 추가로 더해진 값을 넣어 지속적으로 바뀌도록 해주고, sin()함수를 이용해서,
            // x값의 변화에 따른 사인 그래프를 그리도록 만들어 주었다.
            // 이때, 2파이를 곱해 UV 0~1사이 동안 사인 그래프 하나가 그려지도록 해주었고, 0.5를 곱해 절반을 줄여주었다.
            // 10을 곱한 것은 해당 사인그래프가 UV 0~1사이에 몇개의 그래프가 들어올지를 정해준다.
            vUV.y += sin((vUV.x + g_fAccTime) * 3.141592f * 2.f * 10.f) * 0.5;

            // 지금 우리가 사용중인 노이즈 텍스처는 1개의 랜덤 값만 반환하기 때문에 인자를 조금씩 바꾸어 3개를 반환 받는다.
            // 그래야 x,y,z가 랜덤으로 들어간다.
            vRandom = float4(GaussianSample(vUV + float2(0.f, 0.f)).x, GaussianSample(vUV + float2(0.1f, 0.f)).x, GaussianSample(vUV + float2(0.2f, 0.f)).x, GaussianSample(vUV + float2(0.3f, 0.f)).x);

            // 사각형 범위로 생성
            //Particle.vRelativePos.xyz = vRandom.xyz * 1000.f - 500.f;
            //Particle.vRelativePos.z = 100.f;
            
            // 원형 범위로 생성
            // 랜덤으로 얻어온 값은 0~1사이 값이기 때문에 2PI를 곱해 0~2PI 사이의 랜덤한 각도를 구한다.
            float fTheta = vRandom.x * 3.141592f * 2.f;
            // 단위 원을 기준으로 세타에 따른 좌표는 cos세타, sin세타이다.
            // 얻은 좌표에 0~1사이의 값인 랜덤 계수를 곱해 단위원 내에서 특정 위치를 잡아준다.
            // ㄴ> 즉, cos세타, sin세타와 원점을 이은 직선 상의 랜덤한 위치로 바꿔준다.
            // 생성 범위를 곱해 실제 범위 좌표로 바꾸어 준다.
            Particle.vRelativePos.xy = float2(cos(fTheta), sin(fTheta)) * vRandom.y * SpawnRange;
            Particle.vRelativePos.z = 0.f;

            // 파티클 방향
            Particle.vDir.xy = normalize(float2(Particle.vRelativePos.xy));
            
            // 파티클 시스템 오브젝트의 월드포즈를 따라가며 생성
            if (IsWorldSpawn)
            {
                Particle.vRelativePos.xyz += ObjectWorldPos.xyz;
            }

            // 파티클 속력
            Particle.fSpeed = vRandom.z * (MaxSpeed - MinSpeed) + MinSpeed;
            
            // 파티클 LifeTime
            Particle.fCurTime = 0.f;
            Particle.fMaxTime = 2.f;  //vRandom.w * (MaxLife - MinLife) + MinLife;
        }
    }
    
    // 활성화가 된 파티클
    else
    {
        Particle.fCurTime += g_fDT;
        if (Particle.fMaxTime < Particle.fCurTime)
        {
            Particle.iActive = 0;
        }
        else
        {
            // 자신의 속도에 DT를 곱해 이번 DT만큼 이동을하는데, 방향을 곱해서 해당 방향으로 이동하게 만들어 주었다.
            Particle.vRelativePos += ParticleBuffer[_id.x].vDir * ParticleBuffer[_id.x].fSpeed * g_fDT;
        }
    }
}


#endif