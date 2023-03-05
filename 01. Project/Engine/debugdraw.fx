#ifndef _DEBUGDRAW
#define _DEBUGDRAW

#include "register.fx"

// ================================================
// DebugDrawShader
// Mesh					: Rect,Circle, Cube, Sphere -> 그떄 그때 타입에 따라 다르게 들어올 것이다.
// Rasterizer			: CULL_NONE					-> 태두리 라인만 그려야하기 때문에 CULL_NONE으로 해주었다. (뒤에 뼈대도 보여야 함)
// BlendState			: ALPHABLEND				-> 알파값 조절 가능
// DepthStencilState	: NO_WRITE					-> 디버그 용이기 때문에 깊이를 남기면 안된다.

// Material Scalar Parameter						-> 재질로부터 전달받을 정보 (Color)
#define COLOR g_vec4_0
// ================================================

float4 VS_DebugDraw(float3 _vPos : POSITION) : SV_POSITION
{
	// 텍스처를 받지 않기 떄문에 UV값은 필요 없다.
	// 컬러도 우리가 파라미터로 받아온 것을 이용할 것이기 때문에 필요 없다.
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