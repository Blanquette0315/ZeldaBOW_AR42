#ifndef _COMPUTE
#define _COMPUTE

#include "register.fx"

// �ش� Shader�� Texture�� u�������Ϳ� ���ε� �ɸ� ���� ����Ѵ�.
// u�������� ������ t�������Ϳ� �ٸ��� RWTexture2D<>�� �����Ѵ�.
RWTexture2D<float4> tex : register(u0);

#define Color       g_vec4_0
#define Resolution  g_vec2_0

// SV_GroupID           : �����尡 ���� �׷��� ��ǥ
// SV_GroupThreadID     : �׷� ������, �������� ��ǥ
// SV_GroupIndex        : �׷� ������, �������� �ε��� (1����)
// SV_DispatchThreadID  : ��ü ������(��� �׷� ����) ��������, ȣ��� �������� ��ǥ

// �׷� �� ������ ������ 1,2,3�������� ǥ���� �� �ִ�. �츮�� ���� 1���� Ȥ�� 2�������� ����Ѵ�.
// �ش� ������� Shader 5.0 ���� ���� 1024���� �ִ� ��ġ�̴�.
// �׷�� ������ ������ ������ �ɷ�������, �׷��� ��� ��������� ����� ������ ������ �� �ִ�.


[numthreads(32,32,1)] // �׷� �� ������ ���� (Shader ���� 5.0 ���� �ִ� 1024�� ���� ���� ����)
void CS_Paint(uint3 _id : SV_DispatchThreadID)
{
    // �츮�� u�������Ϳ� ���ε��� �� Texture�� 1024 * 1024 �ؽ��Ĵ�.
    // �̶� �츮�� �׷�� �����带 1024�� ������̱� ������ �� �׷��� ������ 1���� Texture�� pixel �Ѱ��� ����ϰ� �ȴ�.
    // ��, DispatchTreadID�� Pixel�� ��ǥ�� ����������.
  
    // �γ��� ������ �����带 �������־��� ������ �ؽ��� ũ�⺸�� �� ū ������ ������� �����ش�.
    if ((uint) Resolution.x <= _id.x || (uint) Resolution.y <= _id.y)
    {
        return;
    }
    
    tex[_id.xy] = Color;
}

#endif