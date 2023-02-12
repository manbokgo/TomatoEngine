#pragma once

namespace tomato
{
    enum class eSamplerType
    {
        Anisotropic,
        Point,		 
        End,
    };

    enum class eCBType
    {
        Transform, // b0
        Material,  // b1
        Global,    // b2

        End,
    };

    // Rasterizer State
    enum class eRSType
    {
        CullBack,
        CullFront,
        CullNone,
        WireFrame,

        End,
    };

    enum class eDSType
    {
        Less,          // Less, Depth Write
        Greater,       // Greater, Depth Write
        NoWrite,       // Less, Depth No Write
        NoTestNoWrite, // NoTest, Depth No Write

        End,
    };

    // BlendState Type
    enum class eBSType
    {
        Default,
        AlphaBlend,
        OneOne,

        End,
    };


    enum ePipelineStage
    {
        VS = 1 << 0,
        HS = 1 << 1,
        DS = 1 << 2,
        GS = 1 << 3,
        PS = 1 << 4,
        AllStage = VS | HS | DS | GS | PS,
    };

    enum class eShaderDomain
    {
        Opaque,      // 불투명 단계
        Mask,        // 출력, 비출력
        Transparent, // 반투명
        PostProcess, // 후처리	    
        None,
    };

    enum class eResType
    {
        Prefab,
        MeshData,
        ComputeShader,

        Material,
        Mesh,
        Texture,
        Sound,
        GraphicsShader,
        AnimClip,
        AnimController,

        End,
    };

    enum class eTextureType
    {
        Single,
        Multiple,

        End,
    };

    extern const char*    RES_TYPE_CHAR[(UINT)eResType::End];
    extern const wchar_t* RES_TYPE_WCHAR[(UINT)eResType::End];

    extern const char*    TEXTURE_TYPE_CHAR[(UINT)eTextureType::End];
    extern const wchar_t* TEXTURE_TYPE_WCHAR[(UINT)eTextureType::End];
    
    enum class eEventType
    {
        CreateObject, // wParam : Object Adress, lParam : Layer Index
        DeleteObject, // wParam : Object Adress,
        AddChild,     // wParam : Child Adress, lParam : Parent Adress
        DeleteRes,    // wParam : RES_TYPE, lParam : Resource Adress
        ChangeLevel,
        ChangeLevelState, // wParam : Next Level State
        End,
    };

    enum class eDebugShape
    {
        // 2D
        Rect,
        Circle,

        // 3D
        Cube,
        Sphere,

        End,
    };

    enum class eLayer
    {
        General,
        General2,

        End,
    };

    enum class eTag
    {
        EnemyHitBox,
        PlayerHitBox,

        End,
    };

    enum class eCollider2DType
    {
        Rect,
        Circle,
    };



    enum class eScalarParam
    {
        Int0,
        Int1,
        Int2,
        Int3,

        Float0,
        Float1,
        Float2,
        Float3,

        Vec2_0,
        Vec2_1,
        Vec2_2,
        Vec2_3,

        Vec4_0,
        Vec4_1,
        Vec4_2,
        Vec4_3,

        Mat0,
        Mat1,
        Mat2,
        Mat3,
    };

    enum class eTexParam
    {
        Tex0,
        Tex1,
        Tex2,
        Tex3,
        Tex4,
        Tex5,
        Tex6,
        Tex7,

        TexCube0,
        TexCube1,

        TexArr0,
        TexArr1,

        End,
    };


    enum class eDir
    {
        Right,
        Up,
        Forward,
        End,
    };

    enum class eSBType
    {
        SRVOnly,
        UAVInc,
    };

    enum class eLightType
    {
        Directional, // 방향성
        Point,       // 점광원
        Spot,        // 스포트 라이트
    };

    enum class eLevelState
    {
        Edit,
        Play,
        Pause,
        Step,
    };
    
}
