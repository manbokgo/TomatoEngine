#ifndef _AAA
#define _AAA



#include "register.fx"
#include "func.fx"

StructuredBuffer<Renderer2DConstant> WorldAndSpriteInfos : register(t44);

struct VTX_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
    uint   iInstance : SV_InstanceID;
};

struct VTX_OUT
{
    float4 vPos : SV_Position;
    float3 vWorldPos : POSITION;
    float2 vUV : TEXCOORD;
    uint   iInstance : SV_InstanceID;
};

#define AtlasTex g_tex_0
#define NormalTex g_tex_1
#define PaletteTex g_tex_2

VTX_OUT VS_My(VTX_IN _in)
{
    VTX_OUT output;

    matrix WVP = mul(WorldAndSpriteInfos[_in.iInstance].WorldMat, g_matVP);

    output.vPos = mul(float4(_in.vPos, 1.f), WVP);
    output.vWorldPos = mul(float4(_in.vPos, 1.f), WorldAndSpriteInfos[_in.iInstance].WorldMat).xyz;
    output.vUV = _in.vUV;
    output.iInstance = _in.iInstance;

    return output;
}

float4 PS_My(VTX_OUT _in) : SV_Target
{
    float4 vOutColor = float4(1.f, 0.f, 1.f, 1.f);

    tSpriteInfo spriteInfo = WorldAndSpriteInfos[_in.iInstance].g_btex_0;

    float2 vUV = (spriteInfo.g_vLeftTop - spriteInfo.g_vOffset) + (spriteInfo.g_vFullSize * _in.vUV);
    
    if (vUV.x < spriteInfo.g_vLeftTop.x || spriteInfo.g_vLeftTop.x + spriteInfo.g_vSlice.x < vUV.x
        || vUV.y < spriteInfo.g_vLeftTop.y || spriteInfo.g_vLeftTop.y + spriteInfo.g_vSlice.y < vUV.y)
    {
        discard;
    }

    vOutColor = AtlasTex.Sample(g_sam_1, vUV);

    if (vOutColor.a <= 0.f)
    {
        discard;
    }

    return vOutColor;
}

#endif
