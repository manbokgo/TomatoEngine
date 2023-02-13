#ifndef _STRUCT
#define _STRUCT


struct tSpriteInfo
{
    float2 g_vLeftTop;
    float2 g_vSlice;
    float2 g_vOffset;
    float2 g_vFullSize;
};


struct tLightColor
{
    float4 vDiff; // ³­¹Ý»ç ±¤
    float4 vSpec; // ¹Ý»ç±¤ °è¼ö
    float4 vAmb;  // È¯°æ±¤
};


// LightInfo
struct tLightInfo
{
    tLightColor color;
    float4      vWorldPos;   // ±¤¿ø ¿ùµå À§Ä¡
    float4      vWorldDir;   // ±¤¿øÀÇ ¿ùµå ¹æÇâ
    int         iLightType;  // ±¤¿ø Å¸ÀÔ
    float       fRadius;     // ±¤¿ø ¹Ý°æ (POINT, SPOT)
    float       fAngleInner; // ±¤¿ø °¢µµ (SPOT)
    float       fAngleOuter;
};


struct tParticle
{
    float4 vRelativePos;
    float4 vDir;

    float fMaxTime;
    float fCurTime;
    float fSpeed;
    uint  iActive;
};

struct tParticleShare
{
    uint  iAliveCount;
    uint3 iPadding;
};

struct Renderer2DConstant
{
    matrix WorldMat;

    tSpriteInfo g_btex_0;
    tSpriteInfo g_btex_1;
    tSpriteInfo g_btex_2;
    tSpriteInfo g_btex_3;
    tSpriteInfo g_btex_4;
    tSpriteInfo g_btex_5;
    tSpriteInfo g_btex_6;
    tSpriteInfo g_btex_7;

    tSpriteInfo g_btexcube_0;
    tSpriteInfo g_btexcube_1;

    tSpriteInfo g_btexarr_0;
    tSpriteInfo g_btexarr_1;
};

struct Renderer3DConstant
{
    matrix WorldMat;
};


#endif
