#ifndef _STD3D
#define _STD3D

#include "register.fx"
#include "func.fx"

// ===========================
// Std3DShader
// g_float_0 : Spec Coefficent
// ===========================
struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;        
    float3 vNormal : NORMAL;

    uint   iInstance : SV_InstanceID;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    
    float3 vViewPos : POSITION;
    float3 vViewNormal : NORMAL;    
};


StructuredBuffer<Renderer3DConstant> WorldAndSpriteInfos : register(t44);


VS_OUT VS_Std3D(VS_IN _in)
{
    VS_OUT output;

    matrix WV = mul(WorldAndSpriteInfos[_in.iInstance].WorldMat, g_matView);
    matrix WVP = mul(WV, g_matProj);

    output.vPosition = mul(float4(_in.vPos, 1.f), WVP);
    output.vUV = _in.vUV;

    output.vViewPos = mul(float4(_in.vPos, 1.f), WV).xyz;
    output.vViewNormal = normalize(mul(float4(_in.vNormal, 0.f), WV).xyz);;

    return output;
}


float4 PS_Std3D(VS_OUT _in) : SV_Target
{
    float4 vOutColor = float4(0.6f, 0.6f, 0.6f, 1.f);
    
    // 표면(픽셀) 이 받는 빛의 총량을 저장할 변수
    tLightColor lightColor = (tLightColor) 0.f;
    
    for (int i = 0; i < g_iLight3DCount; ++i)
    {
        CalcLight3D(_in.vViewPos, _in.vViewNormal, i, lightColor);
    }
    
    // 물체의 색상에 광원을 적용
    vOutColor.rgb = (vOutColor.rgb * lightColor.vDiff.rgb)
                    + lightColor.vSpec.rgb * g_float_0
                    + (vOutColor.rgb * lightColor.vAmb.rgb);
    
    return vOutColor;
}





#endif