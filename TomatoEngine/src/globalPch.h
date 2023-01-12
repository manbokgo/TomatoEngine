#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXTex.h>
#include <SimpleMath.h>

#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

#include <FMOD/fmod.hpp>
#include <JSON/json.hpp>
using json = nlohmann::json;

#include <fmt/core.h>

#include <filesystem>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <stack>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <regex>

using namespace DirectX;
using namespace DirectX::SimpleMath;

#include <wrl.h>
using Microsoft::WRL::ComPtr;

using std::vector;
using std::map;
using std::unordered_map;
using std::list;
using std::array;
using std::pair;
using std::make_pair;
using std::stack;

using std::string;
using std::string_view;
using std::wstring;
using std::wstring_view;

using std::unique_ptr;
using std::make_unique;

using std::function;

using std::swap;
using std::numeric_limits;

namespace fs = std::filesystem;

using Vec2 = DirectX::SimpleMath::Vector2;
using Vec3 = DirectX::SimpleMath::Vector3;
using Vec4 = DirectX::SimpleMath::Vector4;

#include "singleton.h"
#include "define.h"
#include "struct.h"
#include "func.h"


#ifdef NDEBUG
# define ASSERT(e)    ( __assume(e) )
#else
# include <cassert>
# define ASSERT(e)    ( assert(e) )
#endif

#define BIT(x) (1 << x)