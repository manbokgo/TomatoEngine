#ifndef _FUNC
#define _FUNC

#include "struct.fx"
#include "register.fx"


void CalcLight2D(inout tLightColor _LightColor, float3 _vWorldPos, int iLightIdx)
{   
    // Directional Light
    if (0 == g_Light2DBuffer[iLightIdx].iLightType)
    {
        _LightColor.vDiff += g_Light2DBuffer[iLightIdx].color.vDiff;
    }
    
    // Point Light
    else if (1 == g_Light2DBuffer[iLightIdx].iLightType)
    {
        float fDistance = distance(_vWorldPos.xy, g_Light2DBuffer[iLightIdx].vWorldPos.xy);
            
        if (fDistance < g_Light2DBuffer[iLightIdx].fRadius)
        {
            float fRatio = 1.f - fDistance / g_Light2DBuffer[iLightIdx].fRadius;
                
            _LightColor.vDiff += g_Light2DBuffer[iLightIdx].color.vDiff * fRatio;
        }
    }   
    
    // Spot Light
    else
    {
        
    }
}


float3 CalcLight(float3 normal, float3 _vWorldPos, int iLightIdx)
{
    float3 L = 0;
    float attenuation = 1.0f;


    if (g_Light2DBuffer[iLightIdx].iLightType == 1 || g_Light2DBuffer[iLightIdx].iLightType == 2)
    {
        L = g_Light2DBuffer[iLightIdx].vWorldPos.xyz - _vWorldPos;
        float dist = length(L);
        attenuation = max(0.f, 1.0f - (dist / g_Light2DBuffer[iLightIdx].fRadius.x));
        L /= dist;
    }
    else if (g_Light2DBuffer[iLightIdx].iLightType == 0)
    {
        L = -g_Light2DBuffer[iLightIdx].vWorldDir.xyz;
    }

    float nDotL = saturate(dot(normal, L));
    float3 diffuse = nDotL * g_Light2DBuffer[iLightIdx].color.vDiff.rgb;
    
    return diffuse * attenuation;
}


static float GaussianFilter[5][5] =
{
    0.003f  , 0.0133f, 0.0219f, 0.0133f, 0.003f,
    0.0133f , 0.0596f, 0.0983f, 0.0596f, 0.0133f,
    0.0219f , 0.0983f, 0.1621f, 0.0983f, 0.0219f,
    0.0133f , 0.0596f, 0.0983f, 0.0596f, 0.0133f,
    0.003f  , 0.0133f, 0.0219f, 0.0133f, 0.003f,
};

float4 GaussianSample(float2 _vUV)
{
    float4 vOutColor = (float4) 0.f;
    
    if (1.f < _vUV.x)
        _vUV.x = frac(_vUV.x);    
    else if (_vUV.x < 0.f)
        _vUV.x = 1.f + frac(_vUV.x);
    
    if (1.f < _vUV.y)
        _vUV.y = frac(_vUV.y);
    else if (_vUV.y < 0.f)
        _vUV.x = 1.f + frac(_vUV.x);
    
    int2 iUV = _vUV * g_vNoiseResolution;
    iUV -= int2(2, 2);
    
    for (int j = 0; j < 5; ++j)
    {
        for (int i = 0; i < 5; ++i)
        {
            int2 idx = int2(iUV.y + i, iUV.x + j);
            vOutColor += g_Noise[idx] * GaussianFilter[j][i];
        }
    }
    
    return vOutColor;
}


// 0 ~ 1 사이 값을 반환
float Random(int key)
{
    float fValue = 0.f;
    
    return fValue;
}

bool IsBinding(in Texture2D _tex)
{    
    uint width, height;
    _tex.GetDimensions(width, height);
    
    if (width == 0 || height == 0)
        return false;
    
    return true;
}


#endif


