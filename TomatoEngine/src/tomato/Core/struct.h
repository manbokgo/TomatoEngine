#pragma once

namespace tomato
{
    class Texture;
    struct Vtx
    {
        Vec3 vPos;
        Vec4 vColor;
        Vec2 vUV;
    };


    struct tEvent
    {
        eEventType eType;
        DWORD_PTR  wParam;
        DWORD_PTR  lParam;
    };

    struct tLightInfo
    {
        Vec4 vDiff; // 난반사 광
        Vec4 vSpec; // 반사광 계수
        Vec4 vEmb;  // 환경광

        Vec4       vWorldPos;  // 광원 월드 위치
        Vec4       vWorldDir;  // 광원의 방향
        eLightType iLightType; // 광원 타입
        float      fRadius;    // 광원 반경 (POINT, SPOT)
        float      fAngle;     // 광원 각도 (SPOT)
        int        pad;
    };


    // ================
    // Animation 구조체
    // ================
    struct SpriteFrm
    {
        Vec2 LeftTop;
        Vec2 Size;
        Vec2 Offset;
        Vec2 OrigSize;
    };

    using AnimEvent = std::function<void()>;
    struct AnimationFrm
    {
        float        Duration = 0.f;
        SpriteFrm    Sprite;
        wstring      SpriteName;
        Ref<Texture> SpriteTexture = nullptr;
        Ref<Texture> NormalTexture = nullptr;

        vector<AnimEvent> Events;
        int               Index = 0;
    };


    // ===============
    // Particle 구조체
    // ===============
    struct tParticle
    {
        Vec4 vRelativePos;
        Vec4 vDir;

        float fMaxTime;
        float fCurTime;
        float fSpeed;
        UINT  iActive;
    };

    struct tParticleShare
    {
        UINT iAliveCount;
        UINT iPadding[3];
    };

    // ==============
    // TileMap 구조체
    // ==============
    struct tTile
    {
        Vec2 vLeftTop; // UV
        Vec2 vSlice;
    };


    // ================
    // Debug Shape Info
    // ================
    struct tDebugShapeInfo
    {
        eDebugShape eShape;
        Vec4        vColor;
        Vec3        vPosition;
        Vec3        vScale;
        Vec3        vRot;
        float       fRadius;
        float       fDuration;
        float       fCurTime;
    };


    // ================
    // Shader Parameter
    // ================
    struct tScalarParam
    {
        eScalarParam eParam;
        string       strName;
    };

    struct tTextureParam
    {
        eTexParam eParam;
        string    strName;
    };


    // ================
    // 상수버퍼용 구조체
    // ================
    struct tTransform
    {
        Matrix matWorld;
        Matrix matView;
        Matrix matProj;

        Matrix matWV;
        Matrix matWVP;
        Matrix matVP;
    };

    extern tTransform g_transform;

    struct tMtrlConst
    {
        int    iArr[4];
        float  fArr[4];
        Vec2   v2Arr[4];
        Vec4   v4Arr[4];
        Matrix matArr[4];

        array<SpriteFrm, (UINT)eTexParam::End> SpriteInfo;
    };

    struct tGlobalData
    {
        Vec2 vRenderResolution;
        Vec2 vNoiseResolution;
        int  iLight2DCount;
        int  iLight3DCount;

        float fAccTime;
        float fDT;
    };

    extern tGlobalData g_global;

    struct tAnim2DInfo
    {
        Vec2 leftTop;
        Vec2 size;
        Vec2 offset;
        Vec2 origSize;

        int iAnim2DUse;
        int iPadding[3];
    };
}
