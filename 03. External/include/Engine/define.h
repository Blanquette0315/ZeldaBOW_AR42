#pragma once

typedef Vector2 Vec2;
typedef Vector3 Vec3;
typedef Vector4 Vec4;

#define SINGLE(type) public:\
						static type* GetInst()\
						{\
							static type mgr;\
							return &mgr;\
						}\
						private:\
							type();\
							~type();

#define DT CTimeMgr::GetInst()->GetEffectDeltaTime()
//#define EDT CTimeMgr::GetInst()->GetEffectDeltaTime()

#define DEVICE CDevice::GetInst()->GetDevice()
#define CONTEXT CDevice::GetInst()->GetContext()

#define KEY_PRESSED(key)	CKeyMgr::GetInst()->GetKeyState(key) == KEY_STATE::PRESS
#define KEY_TAP(key)		CKeyMgr::GetInst()->GetKeyState(key) == KEY_STATE::TAP
#define KEY_RELEASE(key)	CKeyMgr::GetInst()->GetKeyState(key) == KEY_STATE::RELEASE

#define CLONE(type) public: type* Clone() { return new type(*this); }
#define CLONE_DUMMY(type) public: type* Clone() { return nullptr; }
#define CLONE_ASSERT(type) public: type* Clone() { assert(nullptr); return nullptr; }

#define MAX_LAYER 32

// 스카이 박스는 큐브 매시와 Sphere 2가지로 만들 수 있다.
enum class SKYBOX_TYPE
{
	SPHERE,
	CUBE,
};

enum class SAMPLER_TYPE
{
	ANISOTROPIC, // 이방성
	POINT,

	END,
};

enum class CB_TYPE
{
	TRANSFORM,		// b0
	MATERIAL,		// b1
	GLOBAL,			// b2
	ANIMATION2D,	// b3

	END,
};

// Rasterizer State
enum class RS_TYPE
{
	CULL_BACK,
	CULL_FRONT,
	CULL_NONE,
	WIER_FRAME,
	END,
};

// DepthStencil State
enum class DS_TYPE
{
	LESS,				// Less, Depth Write
	LESS_EQUAL,			// Less Equal, Depth Write
	GREATER,			// Greater, Depth Write
	NO_WRITE,			// Less, Depth No Write
	NO_TEST_NO_WRITE,	// NoTest, Depth No Write

	END,
};

// BlendState Type
enum class BS_TYPE
{
	DEFAULT,
	ALPHABLEND,
	ONE_ONE,

	END,
};

enum PIPELINE_STAGE
{
	VS = 0x01,
	HS = 0x02,
	DS = 0x04,
	GS = 0x08,
	PS = 0x10,
	ALL_STAGE = VS | HS | DS | GS | PS,
};

enum class SHADER_DOMAIN
{
	DOMAIN_DEFERRED_OPAQIE,		// 지연 렌더링
	DOMAIN_DEFERRED_MASK,		// 지연 렌더링
	DOMAIN_DEFERRED_DECAL,		// 지연처리 데칼

	DOMAIN_OPAQUE,				// 불투명 단계
	DOMAIN_MASK,				// 출력, 비출력

	DOMAIN_DECAL,				// 데칼 처리 (Forward)
	DOMAIN_TRANSPARENT,			// 반투명

	DOMAIN_POST_PROCESS,		// 후처리

	// Rendering 목적이 아닌 Engine 차원에서 처리할 용도
	DOMAIN_LIGHT,				// 광원 처리
	NONE,
};

enum class RES_TYPE
{
	PREFAB,
	MESHDATA,
	COMPUTE_SHADER,

	MATERIAL,
	MESH,
	TEXTURE,
	SOUND,
	GRAPHICS_SHADER,

	END,

	LEVEL,
};

extern const char* RES_TYPE_CHAR[(UINT)RES_TYPE::END];
extern const wchar_t* RES_TYPE_WCHAR[(UINT)RES_TYPE::END];

enum class COMPONENT_TYPE
{
	TRANSFORM,
	CAMERA,
	COLLIDER2D,
	COLLIDER3D,
	ANIMATOR2D,
	ANIMATOR3D,
	LIGHT2D,
	LIGHT3D,

	// render
	MESHRENDER,
	TILEMAP,
	PARTICLESYSTEM,
	SKYBOX,
	DECAL,
	LANDSPACE,


	END,

	SCRIPT,
};

extern const char* COMPONENT_TYPE_CHAR[(UINT)COMPONENT_TYPE::END];
extern const wchar_t* COMPONENT_TYPE_WCHAR[(UINT)COMPONENT_TYPE::END];

enum class EVENT_TYPE
{
	CREATE_OBJECT,
	DELETE_OBJECT,
	CHANGE_LEVEL,
	ADD_CHILD,			// wParam : Child Adress, lParam : Parent Adress
	ADD_RES,			// wParam : RES_TYPE, lParam : Resource Adress
	DELETE_RES,			// wParam : RES_TYPE, lParam : Resource Adress
	CHANGE_PREFAB,
	CHANGE_LEVEL_STATE, //wParam : Next Level State
	DELETE_LEVEL,		// wParam : LevelAdress
	SLOWTIME,

	END,
};

enum class DEBUG_SHAPE
{
	// 2D
	RECT,
	CIRCLE,

	// 3D
	CUBE,
	SPHERE,
	CONE,
	FRUSTUM,

	END,
};

enum class COLLIDER2D_TYPE
{
	COLLIDER2D_RECT,
	COLLIDER2D_CIRCLE,
};

enum SCALAR_PARAM
{
	INT_0,
	INT_1,
	INT_2,
	INT_3,

	FLOAT_0,
	FLOAT_1,
	FLOAT_2,
	FLOAT_3,

	VEC2_0,
	VEC2_1,
	VEC2_2,
	VEC2_3,

	VEC4_0,
	VEC4_1,
	VEC4_2,
	VEC4_3,

	MAT_0,
	MAT_1,
	MAT_2,
	MAT_3,
};

enum TEX_PARAM
{
	TEX_0,
	TEX_1,
	TEX_2,
	TEX_3,
	TEX_4,
	TEX_5,
	TEX_6,
	TEX_7,

	TEX_CUBE_0,
	TEX_CUBE_1,

	TEX_ARR_0,
	TEX_ARR_1,

	TEX_END,
};

enum class DIR
{
	RIGHT,
	UP,
	FRONT,
	END,
};

// 구조화 버퍼의 타입(용도)
enum class SB_TYPE
{
	SRV_ONLY,	// ShaderResourceView만 지니고 있는 타입
	UAV_INC,	// ShaderResourceView 뿐 아니라 UnorderedAccessView도 지니고 있는 타입. (SRV없이 UAV만 지니고 있는게 말이 안되긴하다.)
};

enum class LIGHT_TYPE
{
	DIRECTIONAL,	// 방향성 광원			: 한 방향으로 비추는 빛
	POINT,			// 점 광원				: 한 점에서 모든 방향으로 발생하는 빛
	SPOT,			// 스포트 라이트 광원		: 특정 각도로 제한을 둬서 쏘는 빛
};

// 카메라 타입
enum class PROJ_TYPE
{
	PERSPECTIVE,
	ORTHOGRAPHICS,
};

// 레벨 상태
enum class LEVEL_STATE
{
	PLAY,
	STOP,
	PAUSE,
};

enum class MRT_TYPE
{
	SWAPCHAIN,
	DEFERRED,
	LIGHT,
	SHADOW,
	END,
};