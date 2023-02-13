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

void CalcLight3D(float3 _vViewPos, float3 _vViewNormal, int _LightIdx, inout tLightColor _lightcolor)
{
    tLightInfo lightinfo = g_Light3DBuffer[_LightIdx];
       
    float3 ViewLightDir = (float3) 0.f;
    float fDiffPow = (float) 0.f;
    float fSpecPow = (float) 0.f;
    
    // Directional Light
    if (lightinfo.iLightType == 0)
    {
        ViewLightDir = normalize(mul(float4(lightinfo.vWorldDir.xyz, 0.f), g_matView));
    
        // View �����̽� �󿡼� ǥ���� ���� ���⸦ ����
        fDiffPow = saturate(dot(-ViewLightDir, _vViewNormal));        
    
        // �ݻ籤 ���⸦ ����
        // ǥ���� ���� �ݻ纤��       
        float3 vViewReflect = normalize(ViewLightDir + 2.f * dot(-ViewLightDir, _vViewNormal) * _vViewNormal);
        
        // �������� ǥ���� ���ϴ� ����
        float3 vEye = normalize(_vViewPos);
        
        // �ü� ���Ϳ� �ݻ纤�͸� �����ؼ� �ݻ籤�� ���⸦ ����
        fSpecPow = saturate(dot(-vEye, vViewReflect));
        fSpecPow = pow(fSpecPow, 20);
    }
    
    // Point Light
    else if (lightinfo.iLightType ==  1)
    {
        // ViewSpace �󿡼� ������ ��ġ�� �˾Ƴ���.
        float3 vLightViewPos = mul(float4(lightinfo.vWorldPos.xyz, 1.f), g_matView);        
        
        // ������ ��ġ���� ǥ���� ���ϴ� ���͸� ���Ѵ�.
        ViewLightDir = _vViewPos - vLightViewPos;
        
        // �������� ǥ������� �Ÿ��� ���Ѵ�.
        float fDist = length(ViewLightDir);
        
        // �������� ǥ���� ���ϴ� �������͸� ���Ѵ�.        
        ViewLightDir = normalize(ViewLightDir);
        
        // �ݰ��� �Ÿ��� ���� ���� ���� ����
        //float fRatio = saturate(1.f - (fDist / lightinfo.fRadius));        
        float fRatio = cos(saturate(fDist / lightinfo.fRadius) * 3.1415926535f * 0.5f);
                        
        // View �����̽� �󿡼� ǥ���� ���� ���⸦ ����        
        fDiffPow = saturate(dot(-ViewLightDir, _vViewNormal)) * fRatio;

        // �ݻ籤 ���⸦ ����
        // ǥ���� ���� �ݻ纤��       
        float3 vViewReflect = normalize(ViewLightDir + 2.f * dot(-ViewLightDir, _vViewNormal) * _vViewNormal);
        
        // �������� ǥ���� ���ϴ� ����
        float3 vEye = normalize(_vViewPos);
        
        // �ü� ���Ϳ� �ݻ纤�͸� �����ؼ� �ݻ籤�� ���⸦ ����
        fSpecPow = saturate(dot(-vEye, vViewReflect));
        fSpecPow = pow(fSpecPow, 20);        
    }
    
    // SpotLight
    else
    {
        // ViewSpace �󿡼� ������ ��ġ�� �˾Ƴ���.
        float3 vLightViewPos = mul(float4(lightinfo.vWorldPos.xyz, 1.f), g_matView);

        // ������ ��ġ���� ǥ���� ���ϴ� ���͸� ���Ѵ�.
        ViewLightDir = _vViewPos - vLightViewPos;

        float3 ViewLightCenterDir = normalize(mul(float4(lightinfo.vWorldDir.xyz, 0.f), g_matView));


        // �������� ǥ������� �Ÿ��� ���Ѵ�.
        float fDist = length(ViewLightDir);

        // �������� ǥ���� ���ϴ� �������͸� ���Ѵ�.        
        ViewLightDir = normalize(ViewLightDir);

        // �ݰ��� �Ÿ��� ���� ���� ���� ����
        //float fRatio = saturate(1.f - (fDist / lightinfo.fRadius));        
        float fRatio = cos(saturate(fDist / lightinfo.fRadius) * 3.1415926535f * 0.5f);

        float kD = saturate(dot(-ViewLightDir, _vViewNormal));

        // pow ���
        // float kS = pow(saturate(dot(ViewLightDir, ViewLightCenterDir)), 8);

        float rho = dot(ViewLightDir, ViewLightCenterDir);
        float kS = saturate((rho - lightinfo.fAngleOuter) / (lightinfo.fAngleInner - lightinfo.fAngleOuter));

        fDiffPow = kS * kD * fRatio;

        // �ݻ籤 ���⸦ ����
        // ǥ���� ���� �ݻ纤��       
        float3 vViewReflect = normalize(ViewLightDir + 2.f * dot(-ViewLightDir, _vViewNormal) * _vViewNormal);

        // �������� ǥ���� ���ϴ� ����
        float3 vEye = normalize(_vViewPos);

        // �ü� ���Ϳ� �ݻ纤�͸� �����ؼ� �ݻ籤�� ���⸦ ����
        fSpecPow = saturate(dot(-vEye, vViewReflect));
        fSpecPow = pow(fSpecPow, 20);
    }    
    
    // ���� ���ݻ籤
    _lightcolor.vDiff += lightinfo.color.vDiff * fDiffPow;
    
    // ���� �ݻ籤 
    _lightcolor.vSpec += lightinfo.color.vSpec * fSpecPow;

    // ���� ȯ�汤
    _lightcolor.vAmb += lightinfo.color.vAmb;
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


// 0 ~ 1 ���� ���� ��ȯ
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


