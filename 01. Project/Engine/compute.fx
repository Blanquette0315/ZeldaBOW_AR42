#ifndef _COMPUTE
#define _COMPUTE

#include "register.fx"

// 해당 Shader는 Texture를 u레지스터에 바인딩 걸린 것을 사용한다.
// u레지스터 선언은 t레지스터와 다르게 RWTexture2D<>로 선언한다.
RWTexture2D<float4> tex : register(u0);

#define Color       g_vec4_0
#define Resolution  g_vec2_0

// SV_GroupID           : 스레드가 속한 그룹의 좌표
// SV_GroupThreadID     : 그룹 내에서, 스레드의 좌표
// SV_GroupIndex        : 그룹 내에서, 스레드의 인덱스 (1차원)
// SV_DispatchThreadID  : 전체 스레드(모든 그룹 통합) 기준으로, 호출된 스레드의 좌표

// 그룹 다 스레드 선언은 1,2,3차원으로 표현할 수 있다. 우리는 보통 1차원 혹은 2차원으로 사용한다.
// 해당 스레드는 Shader 5.0 버전 기준 1024개가 최대 수치이다.
// 그룹당 스레드 갯수는 제한이 걸려있지만, 그룹을 몇개를 사용할지는 사용자 마음껏 지정할 수 있다.


[numthreads(32,32,1)] // 그룹 당 스레드 갯수 (Shader 버전 5.0 기준 최대 1024개 까지 지정 가능)
void CS_Paint(uint3 _id : SV_DispatchThreadID)
{
    // 우리가 u레지스터에 바인딩을 건 Texture는 1024 * 1024 텍스쳐다.
    // 이때 우리는 그룹당 쓰레드를 1024개 사용중이기 때문에 한 그룹의 쓰레드 1개는 Texture의 pixel 한개를 담당하게 된다.
    // 즉, DispatchTreadID와 Pixel의 좌표가 동일해진다.
  
    // 널널한 범위로 쓰레드를 지정해주었기 때문에 텍스쳐 크기보다 더 큰 범위의 쓰레드는 버려준다.
    if ((uint) Resolution.x <= _id.x || (uint) Resolution.y <= _id.y)
    {
        return;
    }
    
    tex[_id.xy] = Color;
}

#endif