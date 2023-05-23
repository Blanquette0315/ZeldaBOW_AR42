#include "pch.h"

#include "global.h"

tTransform g_transform = {};
tGlobalData g_global = {};
tTimeSlow g_timeslow = { false, 2.f };

std::random_device rd;
std::mt19937 g_gen(rd());
std::uniform_int_distribution<> g_random(0, 1000);


// const Vector3 Vector3::Zero = Vector3(0.f, 0.f, 0.f);

extern const char* RES_TYPE_CHAR[(UINT)RES_TYPE::END]
{
	"PREFAB",
	"MESHDATA",
	"COMPUTE_SHADER",
	"MATERIAL",
	"MESH",
	"TEXTURE",
	"SOUND",
	"GRAPHICS_SHADER"
};

extern const wchar_t* RES_TYPE_WCHAR[(UINT)RES_TYPE::END]
{
	L"PREFAB",
	L"MESHDATA",
	L"COMPUTE_SHADER",
	L"MATERIAL",
	L"MESH",
	L"TEXTURE",
	L"SOUND",
	L"GRAPHICS_SHADER"
};

extern const char* COMPONENT_TYPE_CHAR[(UINT)COMPONENT_TYPE::END]
{
	"TRANSFORM",
	"CAMERA",
	"COLLIDER",
	"ANIMATOR2D",
	"ANIMATOR3D",
	"LIGHT2D",
	"LIGHT3D",
	"RIGIDBODY",
	"MESHRENDER",
	"TILEMAP",
	"PARTICLESYSTEM",
	"SKYBOX",
	"DECAL",
	"LANDSCAPE"
};

extern const wchar_t* COMPONENT_TYPE_WCHAR[(UINT)COMPONENT_TYPE::END]
{
	L"TRANSFORM",
	L"CAMERA",
	L"COLLIDER",
	L"ANIMATOR2D",
	L"ANIMATOR3D",
	L"LIGHT2D",
	L"LIGHT3D",
	L"RIGIDBODY",
	L"MESHRENDER",
	L"TILEMAP",
	L"PARTICLESYSTEM",
	L"SKYBOX",
	L"DECAL",
	L"LANDSCAPE"
};