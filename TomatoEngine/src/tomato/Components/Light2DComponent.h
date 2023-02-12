#pragma once
#include "Component.h"

namespace tomato
{
    class Light2DComponent final : public Component
    {
    private:
        tLightInfo m_Info; // ±¤¿ø Á¤º¸

    public:
        const tLightInfo& GetLightInfo() { return m_Info; }

        void SetLightColor(Vec3 _vColor) { m_Info.vDiff = { _vColor, 0.f }; }
        void SetLightEmbient(Vec3 _vColor) { m_Info.vAmb = { _vColor, 0.f }; }

        void SetLightType(eLightType _eType) { m_Info.iLightType = _eType; }

        void SetRadius(float _fRadius) { m_Info.fRadius = _fRadius; }
        void SetAngle(float _fAngle) { m_Info.fAngle = _fAngle; }

        Vec3       GetLightColor() { return m_Info.vDiff.XYZ(); }
        eLightType GetLightType(eLightType _eType) { return m_Info.iLightType; }
        float      GetRadius(float _fRadius) { return m_Info.fRadius; }
        float      GetAngle(float _fAngle) { return m_Info.fAngle; }


    public:
        virtual void OnLateUpdate() override;


    public:
        CLONE(Light2DComponent)

    public:
        Light2DComponent();
        ~Light2DComponent();
    };
}
