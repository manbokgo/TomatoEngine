#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include <d3d11.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")


#pragma warning(push)
#pragma warning(disable : 4819 6285 26437 26450 26451 26498 4251 26800 26495 4275 26813)

#include <DirectXTex.h>
#include <SimpleMath.h>

#include <fmod.hpp>
#include <imgui.h>
#include <ImGuizmo.h>
#include <spdlog/spdlog.h>


#pragma warning(pop)


#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <queue>
#include <regex>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

using namespace DirectX;
using namespace DirectX::SimpleMath;
namespace fs = std::filesystem;

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

using std::function;
using std::swap;
using std::numeric_limits;

template <typename T>
using Scope = std::unique_ptr<T>;
template <typename T>
using Ref = std::shared_ptr<T>;

template <typename T, typename ... Args>
constexpr Scope<T> CreateScope(Args&& ...args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T, typename ... Args>
constexpr Ref<T> CreateRef(Args&& ...args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

#include "tomato/Core/Macro.h"

#include "tomato/Core/Log.h"
#include "tomato/Core/Singleton.h"
#include "tomato/Core/Vector.h"

#include "tomato/Core/define.h"
#include "tomato/Core/func.h"
#include "tomato/Core/struct.h"

