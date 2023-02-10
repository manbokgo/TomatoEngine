#include "pch.h"

#include "globalPch.h"

namespace tomato
{
    tTransform  g_transform = {};
    tGlobalData g_global = {};

    extern const char* RES_TYPE_CHAR[(UINT)eResType::End] =
    {
        "PREFAB",
        "MESHDATA",
        "COMPUTE_SHADER",
        "MATERIAL",
        "MESH",
        "TEXTURE",
        "SOUND",
        "GRAPHICS_SHADER",
        "ANIM CLIP",
        "ANIM CONTROLLER",
    };

    extern const wchar_t* RES_TYPE_WCHAR[(UINT)eResType::End] =
    {
        L"PREFAB",
        L"MESHDATA",
        L"COMPUTE_SHADER",
        L"MATERIAL",
        L"MESH",
        L"TEXTURE",
        L"SOUND",
        L"GRAPHICS_SHADER",
    };

    extern const char* TEXTURE_TYPE_CHAR[(UINT)eTextureType::End] =
    {
        "Single",
        "Multiple",
    };

    extern const wchar_t* TEXTURE_TYPE_WCHAR[(UINT)eTextureType::End] =
    {
        L"Single",
        L"Multiple",
    };
}
