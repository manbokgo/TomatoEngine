#ifndef _STD2D
#define _STD2D

#include "register.fx"
#include "func.fx"


// HLSL 로 VertexShader 작성하기
struct VTX_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};


struct VTX_OUT
{
    float4 vPos : SV_Position;
    float3 vWorldPos : POSITION;
    float2 vUV : TEXCOORD;
};

// =======================
// Std2DShader
#define AtlasTex g_tex_0
#define NormalTex g_tex_1
#define PaletteTex g_tex_2
// g_tex_0 : OutputTexture
// 
// =======================

VTX_OUT VS_Std2D(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT)0.f;

    output.vPos      = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld).xyz;
    output.vUV       = _in.vUV;

    return output;
}

float4 PS_Std2D(VTX_OUT _in) : SV_Target
{
    float4 vOutColor = float4(1.f, 0.f, 1.f, 1.f);

    float2 vUV = (g_btex_0.g_vLeftTop - g_btex_0.g_vOffset) + (g_btex_0.g_vFullSize * _in.vUV);

    if (vUV.x < g_btex_0.g_vLeftTop.x || g_btex_0.g_vLeftTop.x + g_btex_0.g_vSlice.x < vUV.x
        || vUV.y < g_btex_0.g_vLeftTop.y || g_btex_0.g_vLeftTop.y + g_btex_0.g_vSlice.y < vUV.y)
    {
        discard;
    }

    vOutColor = AtlasTex.Sample(g_sam_1, vUV);

    // 노이즈 텍스쳐 Burn 효과
    //if ( 0.f < g_float_0)
    //{
    //    vOutColor.a -= g_Noise.Sample(g_sam_0, _in.vUV).x * 0.5f + g_float_0 * 0.5f;
    //}    

    if (vOutColor.a <= 0.f)
        discard;

    vOutColor *= g_vec4_0;


    // 광원 처리
    // tLightColor color = (tLightColor)0.f;
    //
    // for (int i = 0; i < g_iLight2DCount; ++i)
    // {
    //     CalcLight2D(color, _in.vWorldPos, i);
    // }
    //
    // vOutColor *= color.vDiff;

    return vOutColor;
}


float4 PS_Player(VTX_OUT _in) : SV_Target
{
    float4 vOutColor = float4(1.f, 0.f, 1.f, 1.f);

    float2 vUV = (g_btex_0.g_vLeftTop - g_btex_0.g_vOffset) + (g_btex_0.g_vFullSize * _in.vUV);

    if (vUV.x < g_btex_0.g_vLeftTop.x || g_btex_0.g_vLeftTop.x + g_btex_0.g_vSlice.x < vUV.x
        || vUV.y < g_btex_0.g_vLeftTop.y || g_btex_0.g_vLeftTop.y + g_btex_0.g_vSlice.y < vUV.y)
    {
        discard;
    }

    float4 paletteCoord = AtlasTex.Sample(g_sam_1, vUV);
    if (paletteCoord.a <= 0.f)
    {
        discard;
    }

    float2 colorUV = {paletteCoord.r, paletteCoord.g};
    vOutColor      = PaletteTex.Sample(g_sam_1, colorUV);


    if (vOutColor.a <= 0.f)
        discard;



    if (g_btex_1.g_vSlice.x)
    {
        float3 normal = normalize(NormalTex.Sample(g_sam_1, vUV) * 2.f - 1.f);
        normal.z *= -1.f;

        // 광원 처리
        float3 lighting;

        for (int i = 0; i < g_iLight2DCount; ++i)
        {
            lighting += CalcLight(normal, _in.vWorldPos, i);
        }

        vOutColor *= float4(lighting.xyz, 1.f);
    }


    // 노이즈 텍스쳐 Burn 효과
    //if ( 0.f < g_float_0)
    //{
    //    vOutColor.a -= g_Noise.Sample(g_sam_0, _in.vUV).x * 0.5f + g_float_0 * 0.5f;
    //}    




    return vOutColor;
}


// =======================
// Std2DAlphaBlendShader
// g_tex_0 : OutputTexture
// =======================

float4 PS_Std2D_AlphaBlend(VTX_OUT _in) : SV_Target
{
    float4 vOutColor = float4(1.f, 0.f, 1.f, 1.f);


    float2 vUV = (g_btex_0.g_vLeftTop - g_btex_0.g_vOffset) + (g_btex_0.g_vFullSize * _in.vUV);

    if (vUV.x < g_btex_0.g_vLeftTop.x || g_btex_0.g_vLeftTop.x + g_btex_0.g_vSlice.x < vUV.x
        || vUV.y < g_btex_0.g_vLeftTop.y || g_btex_0.g_vLeftTop.y + g_btex_0.g_vSlice.y < vUV.y)
    {
        discard;
    }

    vOutColor = AtlasTex.Sample(g_sam_0, vUV);
    vOutColor *= g_vec4_0;

    return vOutColor;
}


#endif
