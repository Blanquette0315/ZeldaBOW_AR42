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

#define SAFE_LOAD_FROM_YAML(type, variable, node) if(node.IsDefined()) { variable = node.as<type>(); }

// ��ī�� �ڽ��� ť�� �Žÿ� Sphere 2������ ���� �� �ִ�.
enum class SKYBOX_TYPE
{
	SPHERE,
	CUBE,
};

enum class SAMPLER_TYPE
{
	ANISOTROPIC, // �̹漺
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
	DOMAIN_DEFERRED_OPAQIE,		// ���� ������
	DOMAIN_DEFERRED_MASK,		// ���� ������
	DOMAIN_DEFERRED_DECAL,		// ����ó�� ��Į

	DOMAIN_OPAQUE,				// ������ �ܰ�
	DOMAIN_MASK,				// ���, �����

	DOMAIN_DECAL,				// ��Į ó�� (Forward)
	DOMAIN_TRANSPARENT,			// ������

	DOMAIN_POST_PROCESS,		// ��ó��

	// Rendering ������ �ƴ� Engine �������� ó���� �뵵
	DOMAIN_LIGHT,				// ���� ó��
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
	LANDSCAPE,


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

// ����ȭ ������ Ÿ��(�뵵)
enum class SB_TYPE
{
	SRV_ONLY,	// ShaderResourceView�� ���ϰ� �ִ� Ÿ��
	UAV_INC,	// ShaderResourceView �� �ƴ϶� UnorderedAccessView�� ���ϰ� �ִ� Ÿ��. (SRV���� UAV�� ���ϰ� �ִ°� ���� �ȵǱ��ϴ�.)
};

enum class LIGHT_TYPE
{
	DIRECTIONAL,	// ���⼺ ����			: �� �������� ���ߴ� ��
	POINT,			// �� ����				: �� ������ ��� �������� �߻��ϴ� ��
	SPOT,			// ����Ʈ ����Ʈ ����		: Ư�� ������ ������ �ּ� ��� ��
};

// ī�޶� Ÿ��
enum class PROJ_TYPE
{
	PERSPECTIVE,
	ORTHOGRAPHICS,
};

// ���� ����
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