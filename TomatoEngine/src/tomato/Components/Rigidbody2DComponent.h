#pragma once
#include "Component.h"

namespace tomato
{
    class Rigidbody2DComponent : public Component
    {
    public:
        Rigidbody2DComponent();
        Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
    
        enum class BodyType { Static = 0, Kinematic, Dynamic };

        BodyType Type = BodyType::Dynamic;
        bool     AutoMass = true;
        float    Mass = 1.0f;
        float    LinearDrag = 0.0f;
        float    AngularDrag = 0.05f;
        float    GravityScale = 1.0f;
        bool     AllowSleep = true;
        bool     Awake = true;
        bool     Continuous = false;
        bool     Interpolation = true;
        bool     FreezeRotation = false;

        void* RuntimeBody = nullptr;

        // For interpolation/extrapolation
        Vec3 PreviousTranslationRotation;
        Vec3 TranslationRotation;

        void OnLateUpdate() override {};
        CLONE_DUMMY(Rigidbody2DComponent);
    };
    
}
