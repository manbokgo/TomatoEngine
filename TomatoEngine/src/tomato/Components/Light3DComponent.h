#pragma once
#include "Component.h"

namespace tomato
{
    class Light3DComponent final : public Component
    {
        friend class PropertiesPanel;

    public:
        Light3DComponent() = default;
        ~Light3DComponent() override = default;


        [[nodiscard]] const tLightInfo& GetInfo();

        void SetLightColor(Vec3 _vColor) { m_Info.vDiff = Vec4(_vColor, 0.f); }
        void SetLightSpecular(Vec3 _vSpec) { m_Info.vSpec = Vec4(_vSpec, 0.f);; }
        void SetLightAmbient(Vec3 _vColor) { m_Info.vAmb = Vec4(_vColor, 0.f);; }
        void SetLightType(eLightType _eType) { m_Info.iLightType = _eType; }

        void SetRadius(float _fRadius) { m_Info.fRadius = _fRadius; }
        void SetAngleInner(float _fAngle) { m_AngleInner = _fAngle; }
        void SetAngleOuter(float _fAngle) { m_AngleOuter = _fAngle; }

        Vec3 GetLightColor() { return m_Info.vDiff.XYZ(); }
        eLightType GetLightType(eLightType _eType) { return m_Info.iLightType; }
        float GetRadius(float _fRadius) { return m_Info.fRadius; }


    private:
        tLightInfo m_Info; // 광원 정보
        float m_AngleOuter;
        float m_AngleInner;
    };
}
