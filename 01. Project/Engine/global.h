#pragma once

#include <vector>
#include <list>
#include <map>
#include <string>
#include <queue>
#include <fstream>

using std::vector;
using std::list;
using std::map;
using std::make_pair;

using std::string;
using std::wstring;
using std::queue;

#include <typeinfo>

// FileSystem
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
using namespace std::experimental;
using namespace std::experimental::filesystem;

// yaml
#include <yaml-cpp/yaml.h>

#ifdef _DEBUG
#pragma comment(lib, "yaml_cpp/libyaml-cppmdd.lib")
#else
#pragma comment(lib, "yaml_cpp/yaml-cpp.lib")
#endif

// PhysX Engine
#ifdef _DEBUG
#pragma comment(lib, "PhysEngine/PhysEngine_x64d.lib")
#else
#pragma comment(lib, "yaml_cpp/yaml-cpp.lib")
#endif


#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <wrl.h>

#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace Microsoft::WRL;

#include "SimpleMath.h"
using namespace DirectX::SimpleMath;

// Fbx Loader
#include <FBXLoader/fbxsdk.h>

#ifdef _DEBUG
#pragma comment(lib, "FBXLoader/x64/debug/libfbxsdk-md.lib")
#else
#pragma comment(lib, "FBXLoader/x64/release/libfbxsdk-md.lib")
#endif

#include "singleton.h"

#include "define.h"
#include "struct.h"
#include "func2.h"
#include "YAML.h"