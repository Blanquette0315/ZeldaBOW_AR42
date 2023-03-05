#ifndef _DEBUGDRAW
#define _DEBUGDRAW

#include "register.fx"

// ================================================
// DebugDrawShader
// Mesh					: Rect,Circle, Cube, Sphere -> �׋� �׶� Ÿ�Կ� ���� �ٸ��� ���� ���̴�.
// Rasterizer			: CULL_NONE					-> �µθ� ���θ� �׷����ϱ� ������ CULL_NONE���� ���־���. (�ڿ� ���뵵 ������ ��)
// BlendState			: ALPHABLEND				-> ���İ� ���� ����
// DepthStencilState	: NO_WRITE					-> ����� ���̱� ������ ���̸� ����� �ȵȴ�.

// Material Scalar Parameter						-> �����κ��� ���޹��� ���� (Color)
#define COLOR g_vec4_0
// ================================================

float4 VS_DebugDraw(float3 _vPos : POSITION) : SV_POSITION
{
	// �ؽ�ó�� ���� �ʱ� ������ UV���� �ʿ� ����.
	// �÷��� �츮�� �Ķ���ͷ� �޾ƿ� ���� �̿��� ���̱� ������ �ʿ� ����.
	float4 vPosition = (float4)0.f;

	vPosition = mul(float4(_vPos, 1.f), g_matWVP);

	return vPosition;
}

float4 PS_DebugDraw(float4 _vPosition : SV_Position) : SV_Target
{
	float4 vOutColor = COLOR;

	return vOutColor;
}

#endif