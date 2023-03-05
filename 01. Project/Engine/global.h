#pragma once

#include <vector>
#include <list>
#include <map>
#include <string>
#include <queue>

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

#include "singleton.h"

#include "define.h"
#include "struct.h"
#include "func2.h"